

/***********************************************************************************************************************
 * Pragma directive
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes <System Includes>
 ***********************************************************************************************************************/
#include "user_wifi.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "regex.h"
#include "ui.h"
#include "f9_event.h"
#include "common.h"
/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/
#define TAG "USER_WIFI"

/***********************************************************************************************************************
 * Typedef definitions
 ***********************************************************************************************************************/
static bool wifi_initialized = false;
static bool is_wifi_connected = false;
static esp_netif_t *wifi_netif_sta = NULL;
static esp_netif_t *wifi_netif_ap = NULL;
static bool is_scanning = false;
/***********************************************************************************************************************
 * Private global variables and functions
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 ***********************************************************************************************************************/
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void wifi_scan_task(void *pvParameters);
/***********************************************************************************************************************
 * Imported global variables and functions (from other files)
 ***********************************************************************************************************************/
void wifi_net_init(void)
{
    // Init TCP/IP and WiFi
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_netif_sta = esp_netif_create_default_wifi_sta();
    wifi_netif_ap = esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_START, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_STOP, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &wifi_event_handler, NULL));
}

void wifi_init_sta(const char *ssid, const char *pass)
{
    is_wifi_connected = false;
    ESP_LOGI(TAG, "Switching to STA mode");

    esp_wifi_stop();
    esp_wifi_set_mode(WIFI_MODE_STA);

    wifi_config_t wifi_config = {0};
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, pass, sizeof(wifi_config.sta.password));

    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
}

void wifi_init_ap(const char *ssid, const char *pass)
{
    is_wifi_connected = false;
    ESP_LOGI(TAG, "Switching to AP mode");

    esp_wifi_stop();
    esp_wifi_set_mode(WIFI_MODE_AP);

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "",
            .ssid_len = 0,
            .channel = 1,
            .password = "",
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK},
    };

    strncpy((char *)wifi_config.ap.ssid, ssid, sizeof(wifi_config.ap.ssid));
    wifi_config.ap.ssid_len = strlen(ssid);
    strncpy((char *)wifi_config.ap.password, pass, sizeof(wifi_config.ap.password));

    if (strlen(pass) == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    esp_wifi_start();
}

void wifi_off(void)
{
    ESP_LOGI(TAG, "Turning off WiFi");
    esp_wifi_stop();
}

void wifi_scan_action(void)
{
    // Create a task to scan for WiFi networks
    xTaskCreate(&wifi_scan_task, "wifi_scan_task", 4096, NULL, 5, NULL);
}

void wifi_user_disconnect(void)
{
    // if (is_wifi_connected)
    // {
        esp_wifi_disconnect();
        is_wifi_connected = false;
    // }
}

void wifi_user_connect(const char *ssid, const char *pass)
{
    if (is_wifi_connected)
    {
        esp_wifi_disconnect();
        is_wifi_connected = false;
    }
    wifi_init_sta(ssid, pass);
}

bool get_is_wifi_connected(void)
{
    return is_wifi_connected;
}
/***********************************************************************************************************************
 * static functions
 ***********************************************************************************************************************/
static void wifi_scan_task(void *pvParameters)
{
    while (1)
    {
        // disconnect and start scan
        is_scanning = true;
        wifi_user_disconnect();
        uint16_t number = 15;
        wifi_ap_record_t ap_info[15];
        uint16_t ap_count = 0;
        // This function can be used to initiate a WiFi scan if needed
        // For example, you can call esp_wifi_scan_start() here
        // and handle the results in the event handler
        esp_wifi_scan_start(NULL, true);
        ESP_LOGI(TAG, "Max AP number ap_info can hold = %u", number);
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
        ESP_LOGI(TAG, "Total APs scanned = %u, actual AP number ap_info holds = %u", ap_count, number);
        for (int i = 0; i < number; i++)
        {
            ESP_LOGI(TAG, "SSID \t\t%s", ap_info[i].ssid);
            ESP_LOGI(TAG, "RSSI \t\t%d", ap_info[i].rssi);
            // add to linked list or array for UI display
            lv_dropdown_add_option(objects.input_wifi_list, (const char *)ap_info[i].ssid, i);
        }
        if (is_wifi_connected == false)
        {
            wifi_user_connect(device_system.wifi_ssid, device_system.wifi_pass);
        }
        is_scanning = false;
        vTaskDelay(10000 / portTICK_PERIOD_MS); // Scan every 10 seconds
        vTaskDelete(NULL);                      // Delete the task after one scan
    }
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        if (is_scanning == false)
            esp_wifi_connect();
        is_wifi_connected = false;
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        ESP_LOGW(TAG, "WiFi disconnected, retrying...");
        ESP_LOGW(TAG, "WiFi disconnected, is_scanning=%d", is_scanning);
        is_wifi_connected = false;
        if (is_scanning == false)
            esp_wifi_connect();
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        char ip_str[16];
        esp_ip4addr_ntoa(&event->ip_info.ip, ip_str, sizeof(ip_str));
        ESP_LOGI(TAG, "WiFi connected, got IP: %s", ip_str);
        snprintf(device_system.wifi_ip, sizeof(device_system.wifi_ip), "%s", ip_str);
        // start web server here if needed
        f9_event_post(E_EVENT_WIFI_GOT_IP, NULL, 0);
        is_wifi_connected = true;
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG, "Device connected to AP: AID=%d, MAC=%02x:%02x:%02x:%02x:%02x:%02x",
                 event->aid,
                 event->mac[0], event->mac[1], event->mac[2],
                 event->mac[3], event->mac[4], event->mac[5]);
        // You can add your custom logic here (e.g., start webserver, notify UI, etc.)
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_START)
    {
        ESP_LOGI(TAG, "WiFi AP started");
        // You can add your custom logic here (e.g., start webserver, notify UI, etc.)
        f9_event_post(E_EVENT_WIFI_AP_STARTED, NULL, 0);
    }
    else if (event_id == WIFI_EVENT_AP_STOP)
    {
        ESP_LOGI(TAG, "WiFi AP stopped");
        // You can add your custom logic here (e.g., stop webserver, notify UI, etc.)
        f9_event_post(E_EVENT_WIFI_AP_STOPPED, NULL, 0);
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI("WIFI", "Device disconnected: AID=%d, MAC=%02x:%02x:%02x:%02x:%02x:%02x",
                 event->aid,
                 event->mac[0], event->mac[1], event->mac[2],
                 event->mac[3], event->mac[4], event->mac[5]);
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_AP_STAIPASSIGNED)
    {
        ip_event_ap_staipassigned_t *event = (ip_event_ap_staipassigned_t *)event_data;
        ESP_LOGI("WIFI", "Client assigned IP: " IPSTR, IP2STR(&event->ip));
    }
}
/***********************************************************************************************************************
 * End of file
 ***********************************************************************************************************************/