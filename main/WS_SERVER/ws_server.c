#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_log.h"
#include <esp_vfs.h>
#include <esp_spiffs.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "ms5837_sensor.h"

#define WWW_PARTITION_PATH "/www"
#define WWW_PARTITION_LABEL "www"

static const char *TAG = "WS_SERVER";
static httpd_handle_t server = NULL;
static int ws_fd = -1; // WebSocket client connection

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
static esp_err_t static_file_handler(httpd_req_t *req)
{
    char filepath[520];
    const char *uri = req->uri;
    if (strcmp(uri, "/") == 0)
    {
        uri = "/index.html";
    }
    // Check for overflow
    if (strlen(WWW_PARTITION_PATH) + strlen(uri) + 1 > sizeof(filepath))
    {
        ESP_LOGE(TAG, "Requested URI too long");
        httpd_resp_send_err(req, HTTPD_414_URI_TOO_LONG, "URI too long");
        return ESP_FAIL;
    }
    snprintf(filepath, sizeof(filepath), "%s%s", WWW_PARTITION_PATH, uri);

    FILE *file = fopen(filepath, "r");
    if (!file)
    {
        ESP_LOGE(TAG, "Failed to open file: %s", filepath);
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
        return ESP_FAIL;
    }

    char chunk[1024];
    size_t chunksize;
    httpd_resp_set_type(req, "text/html");
    do
    {
        chunksize = fread(chunk, 1, sizeof(chunk), file);
        if (chunksize > 0)
        {
            if (httpd_resp_send_chunk(req, chunk, chunksize) != ESP_OK)
            {
                fclose(file);
                httpd_resp_sendstr_chunk(req, NULL);
                return ESP_FAIL;
            }
        }
    } while (chunksize > 0);
    fclose(file);
    httpd_resp_sendstr_chunk(req, NULL);
    return ESP_OK;
}

// Start webserver
static httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 5; // TODO: this is config max uri handlers
    config.lru_purge_enable = true;
    config.stack_size = 8192; // Increase stack size if needed
    config.core_id = 0; // Run on core 0

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
            .user_ctx = NULL};
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

static void connect_handler(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data)
{
    if (server == NULL)
    {
        ESP_LOGI(TAG, "Starting webserver");
        server = start_webserver();
    }
}

void ws_server_init(void)
{
    esp_err_t ret = www_spiffs_init();
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, NULL));
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