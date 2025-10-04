#include "ws_server.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_log.h"
#include <esp_vfs.h>
#include <esp_spiffs.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "ms5837_sensor.h"
#include <esp32/rom/crc.h>

static const char *TAG = "WS_SERVER";

#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + CONFIG_SPIFFS_OBJ_NAME_LEN)
#define FILE_HASH_SUFFIX ".crc"
#define IS_FILE_EXT(filename, ext) \
    (strcasecmp(&filename[strlen(filename) - sizeof(ext) + 1], ext) == 0)
#define ERROR_ACTION(TAG, condition, action, format, ...)                                      \
    if ((condition))                                                                           \
    {                                                                                          \
        ESP_LOGE(TAG, "%s:%d (%s): " format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
        action;                                                                                \
    }
#define WWW_PARTITION_PATH "/www"
#define WWW_PARTITION_LABEL "www"

static httpd_handle_t server = NULL;
static int ws_fd = -1; // WebSocket client connection
static char *buffer;
#define BUFFER_SIZE 2048

// WebSocket event handler
static esp_err_t ws_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "Handshake done, new WS client connected");
        ws_fd = httpd_req_to_sockfd(req);
        return ESP_OK;
    }

    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    // Just echo back
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    return ret;
}

// Periodic task: send sensor data
static void send_sensor_data(void *arg)
{
    if (server && ws_fd >= 0)
    {
        char buffer[128];
        // Get real sensor values
        float temperature = get_temperature_celsius();
        float pressure = get_pressure_mbar();

        int len = snprintf(buffer, sizeof(buffer),
                           "{\"temperature\":%.2f,\"pressure\":%.2f}",
                           temperature, pressure);

        httpd_ws_frame_t ws_pkt = {
            .final = true,
            .fragmented = false,
            .type = HTTPD_WS_TYPE_TEXT,
            .payload = (uint8_t *)buffer,
            .len = len};

        httpd_ws_send_frame_async(server, ws_fd, &ws_pkt);
    }
}

esp_err_t www_spiffs_init()
{
    ESP_LOGD(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = WWW_PARTITION_PATH,
        .partition_label = WWW_PARTITION_LABEL,
        .max_files = 10,
        .format_if_mount_failed = false};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(WWW_PARTITION_LABEL, &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "Partition size: total: %d, used: %d", total, used);
    return ESP_OK;
}

// Static file handler for serving HTML from SPIFFS
// static esp_err_t static_file_handler(httpd_req_t *req)
// {
//     char filepath[520];
//     const char *uri = req->uri;
//     if (strcmp(uri, "/") == 0)
//     {
//         uri = "/index.html";
//     }
//     // Check for overflow
//     if (strlen(WWW_PARTITION_PATH) + strlen(uri) + 1 > sizeof(filepath))
//     {
//         ESP_LOGE(TAG, "Requested URI too long");
//         httpd_resp_send_err(req, HTTPD_414_URI_TOO_LONG, "URI too long");
//         return ESP_FAIL;
//     }
//     snprintf(filepath, sizeof(filepath), "%s%s", WWW_PARTITION_PATH, uri);

//     FILE *file = fopen(filepath, "r");
//     if (!file)
//     {
//         ESP_LOGE(TAG, "Failed to open file: %s", filepath);
//         httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
//         return ESP_FAIL;
//     }

//     char chunk[1024];
//     size_t chunksize;
//     httpd_resp_set_type(req, "text/html");
//     do
//     {
//         chunksize = fread(chunk, 1, sizeof(chunk), file);
//         if (chunksize > 0)
//         {
//             if (httpd_resp_send_chunk(req, chunk, chunksize) != ESP_OK)
//             {
//                 fclose(file);
//                 httpd_resp_sendstr_chunk(req, NULL);
//                 return ESP_FAIL;
//             }
//         }
//     } while (chunksize > 0);
//     fclose(file);
//     httpd_resp_sendstr_chunk(req, NULL);
//     return ESP_OK;
// }
// Handler phục vụ file tĩnh
// Set HTTP response content type according to file extension
static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filename)
{
    if (IS_FILE_EXT(filename, ".html"))
    {
        return httpd_resp_set_type(req, "text/html");
    }
    else if (IS_FILE_EXT(filename, ".js"))
    {
        return httpd_resp_set_type(req, "application/javascript");
    }
    else if (IS_FILE_EXT(filename, ".css"))
    {
        return httpd_resp_set_type(req, "text/css");
    }
    else if (IS_FILE_EXT(filename, ".ico"))
    {
        return httpd_resp_set_type(req, "image/x-icon");
    }
    /* This is a limited set only */
    /* For any other type always set as plain text */
    return httpd_resp_set_type(req, "text/plain");
}

static char *get_path_from_uri(char *dest, const char *base_path, const char *uri, size_t destsize)
{
    const size_t base_pathlen = strlen(base_path);
    size_t pathlen = strlen(uri);

    const char *quest = strchr(uri, '?');
    if (quest)
    {
        pathlen = MIN(pathlen, quest - uri);
    }
    const char *hash = strchr(uri, '#');
    if (hash)
    {
        pathlen = MIN(pathlen, hash - uri);
    }

    if (base_pathlen + pathlen + 1 > destsize)
    {
        // Full path string won't fit into destination buffer
        return NULL;
    }

    // Construct full path (base + path)
    strcpy(dest, base_path);
    strlcpy(dest + base_pathlen, uri, pathlen + 1);

    // Return pointer to path, skipping the base
    return dest + base_pathlen;
}

static esp_err_t file_check_etag_hash(httpd_req_t *req, char *file_hash_path, char *etag, size_t etag_size)
{
    struct stat file_hash_stat;
    if (stat(file_hash_path, &file_hash_stat) == -1)
    {
        // Hash file not created yet
        return ESP_ERR_NOT_FOUND;
    }

    FILE *fd_hash = fopen(file_hash_path, "r+");

    // Ensure hash file was opened

    // Read existing hash
    uint32_t crc;
    int read = fread(&crc, sizeof(crc), 1, fd_hash);
    fclose(fd_hash);

    snprintf(etag, etag_size, "\"%08lX\"", crc);

    // Compare to header sent by client
    size_t if_none_match_length = httpd_req_get_hdr_value_len(req, "If-None-Match") + 1;
    if (if_none_match_length > 1)
    {
        char *if_none_match = malloc(if_none_match_length);
        httpd_req_get_hdr_value_str(req, "If-None-Match", if_none_match, if_none_match_length);

        bool header_match = strcmp(etag, if_none_match) == 0;
        free(if_none_match);

        // Matching ETag, return not modified
        if (header_match)
        {
            return ESP_OK;
        }
        else
        {
            ESP_LOGW(TAG, "ETag for file %s sent by client does not match (%s != %s)", file_hash_path, etag, if_none_match);
            return ESP_ERR_INVALID_CRC;
        }
    }

    return ESP_ERR_INVALID_ARG;
}

esp_err_t static_file_handler(httpd_req_t *req)
{
    const char *uri = req->uri;

    if (strcmp(uri, "/") == 0)
    {
        uri = "/index.html"; // map "/" về index.html
    }

    char filepath[520];
    // snprintf(filepath, sizeof(filepath), "/spiffs%s", uri); // file nằm trong SPIFFS
    snprintf(filepath, sizeof(filepath), "%s%s", WWW_PARTITION_PATH, uri);
    ESP_LOGI(TAG, "Requested file: %s", filepath);
    // Mở file
    FILE *f = fopen(filepath, "r");
    if (!f)
    {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
        return ESP_FAIL;
    }

    // xác định content-type
    if (strstr(uri, ".html"))
    {
        httpd_resp_set_type(req, "text/html");
    }
    else if (strstr(uri, ".js"))
    {
        httpd_resp_set_type(req, "application/javascript");
    }
    else if (strstr(uri, ".css"))
    {
        httpd_resp_set_type(req, "text/css");
    }
    else
    {
        httpd_resp_set_type(req, "text/plain");
    }

    char chunk[512];
    size_t chunksize;
    while ((chunksize = fread(chunk, 1, sizeof(chunk), f)) > 0)
    {
        if (httpd_resp_send_chunk(req, chunk, chunksize) != ESP_OK)
        {
            fclose(f);
            httpd_resp_sendstr_chunk(req, NULL);
            return ESP_FAIL;
        }
    }
    fclose(f);
    httpd_resp_sendstr_chunk(req, NULL);
    return ESP_OK;
}

// Start webserver
static httpd_handle_t start_webserver_handler(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 10; // TODO: this is config max uri handlers
    config.lru_purge_enable = true;
    config.stack_size = 8192; // Increase stack size if needed
    config.core_id = 0;       // Run on core 0
    config.uri_match_fn = httpd_uri_match_wildcard; // <--- quan trọng
    config.server_port = 8080;
    
    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_uri_t ws_uri = {
            .uri = "/ws",
            .method = HTTP_GET,
            .handler = ws_handler,
            .is_websocket = true};
        httpd_register_uri_handler(server, &ws_uri);

        // Register handler for root '/'
        httpd_uri_t root_uri = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = static_file_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &root_uri);

        // Static file handler for all other GET requests
        httpd_uri_t static_uri = {
            .uri = "/*",
            .method = HTTP_GET,
            .handler = static_file_handler,
            .user_ctx = NULL,
             // <--- quan trọng
        };
        httpd_register_uri_handler(server, &static_uri);

        // Timer gửi dữ liệu mỗi 2s
        const esp_timer_create_args_t periodic_timer_args = {
            .callback = &send_sensor_data,
            .name = "ws_sensor_sender"};
        esp_timer_handle_t periodic_timer;
        esp_timer_create(&periodic_timer_args, &periodic_timer);
        esp_timer_start_periodic(periodic_timer, 2000000); // 2s
        return server;
    }
    return NULL;
}

void ws_server_init(void)
{
    esp_err_t ret = www_spiffs_init();
}

void ws_server_task(void *pvParameters)
{
    ESP_LOGI(TAG, "ws_server_task");
    server = start_webserver_handler();
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelete(NULL);
}

void ws_server_start(void)
{
    if (server)
    {
        ESP_LOGW(TAG, "Webserver already running");
        return;
    }
    xTaskCreate(&ws_server_task, "ws_server_task", 8192, NULL, 5, NULL);
}

bool is_ws_init_done(void)
{
    ESP_LOGI(TAG, "Webserver initialization done");
    return server != NULL;
}

void ws_server_stop(void)
{
    if (server)
    {
        ESP_LOGI(TAG, "Stopping webserver");
        httpd_stop(server);
        server = NULL;
    }
    ws_fd = -1;
}