#include "ST7789.h"
#include "PCF85063.h" // RTC

#include "Wireless.h"

#include "PWR_Key.h"
#include "ui.h"
#include "ui_update.h"
#include "common.h"
#include "ws_server.h"
#include "ms5837_sensor.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"

#define TAG "main"

static void show_graphics(void);
static void common_data_init(void);
static void wifi_init_sta(void);
static void wifi_init_ap(void);
bool is_start_webserver = false;
FlowGlobalVariables_t flow_global_variables;
System_t device_system;

void Driver_Loop(void *parameter)
{
    // Wireless_Init();
    // wifi_init_sta();
    wifi_init_ap();
    while (1)
    {
        PCF85063_Loop();
        if (is_start_webserver && is_ws_init_done() == false)
        {
            vTaskDelay(pdMS_TO_TICKS(10000));
            ws_server_init();
            is_start_webserver = false;
        }
        PWR_Loop();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelete(NULL);
}

void Display_Loop(void)
{
    while (1)
    {
        // raise the task priority of LVGL and/or reduce the handler period can improve the performance
        static uint32_t last_wake_time = 0;
        if (xTaskGetTickCount() - last_wake_time > pdMS_TO_TICKS(1000))
        {
            last_wake_time = xTaskGetTickCount();
            show_graphics();
        }
        static uint32_t update_rtc_time = 0;
        if (xTaskGetTickCount() - update_rtc_time > pdMS_TO_TICKS(1000))
        {
            update_rtc_time = xTaskGetTickCount();
            datetime_t datetime;
            datetime = PCF85063_GetDatetime();
            ui_update_time(flow_global_variables.current_screen_id,
                           datetime.hour, datetime.minute, datetime.second);
        }

        static uint32_t update_wifi_settings_screen_time = 0;
        if (xTaskGetTickCount() - update_wifi_settings_screen_time > pdMS_TO_TICKS(100))
        {
            update_wifi_settings_screen_time = xTaskGetTickCount();
            if (flow_global_variables.current_screen_id == SCREEN_ID_SCREEN_WIFI_SETTINGS)
                update_wifi_settings_screen();
        }

        if (lv_disp_get_default() != NULL)
        {
            lv_timer_handler();
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void Driver_Init(void)
{

    PWR_Init();
    I2C_Init();
    PCF85063_Init();
    ms5837_sensor_init();
    xTaskCreate(Driver_Loop, "Driver_Loop", 4096 * 2, NULL, 5, NULL);
}

void app_main(void)
{
    nvs_flash_init();
    common_data_init();
    Driver_Init();

    LCD_Init();
    LVGL_Init(); // returns the screen object

    /********************* Demo *********************/
    // Lvgl_Example1();
    lv_disp_set_rotation(lv_disp_get_default(), LV_DISP_ROT_270); // 设置显示器旋转
    ui_init();
    ui_chart_init_extra(); // Initialize chart extra settings
    loadScreen(SCREEN_ID_MAIN);
    xTaskCreate(Display_Loop, "Display_Loop", 4096 * 2, NULL, 1, NULL);
}

static void show_graphics(void)
{
    static uint32_t interval_time_read_sensor = 0;
    static bool popup_displayed = false;
    // Serial.println("Reading sensor data...");
    ESP_LOGI(TAG, "Reading sensor data...");
    int ret = ms5837_sensor_loop(); // Read sensor data
    if (ret == false)
    {
        ESP_LOGW(TAG, "Sensor read failed, skipping UI update.");
        // show pop-up message on the screen if needed
        show_popup("Error", "Sensor Disconnected from board", 0);
        popup_displayed = true;
        return; // Skip UI update if sensor read fails
    }
    else
    {
        // If there was a popup, close it
        if (popup_displayed)
        {
            popup_displayed = false;
            close_popup();
        }
    }
    float pressure_psi = get_pressure_psi();
    float pressure_mbar = get_pressure_mbar();
    float temperature_celsius = get_temperature_celsius();
    float temperature_fahrenheit = get_temperature_fahrenheit();
    ui_screen_update(flow_global_variables.current_screen_id, pressure_psi, pressure_mbar, temperature_celsius, temperature_fahrenheit);
    if (flow_global_variables.current_screen_id == SCREEN_ID_MAIN)
        chart_add_data(objects.main_chart, pressure_psi, SCREEN_ID_MAIN, flow_global_variables.pressure_unit);
    else if (flow_global_variables.current_screen_id == SCREEN_ID_SCREEN_PRESSURE)
        chart_add_data(objects.pressure_chart, pressure_psi, SCREEN_ID_SCREEN_PRESSURE, flow_global_variables.pressure_unit);
    else if (flow_global_variables.current_screen_id == SCREEN_ID_SCREEN_TEMPERATURE)
        chart_add_data(objects.temperature_chart, temperature_celsius, SCREEN_ID_SCREEN_TEMPERATURE, flow_global_variables.temperature_unit);
    // Update device data
    // shows_device_data("%s\n", get_device_data_packet(get_current_time_stamp(), temperature_celsius, pressure_mbar));
    // printf("createSensorDataText: %s\n", createSensorDataText());
}

static void common_data_init(void)
{
    flow_global_variables.pressure_unit = PRESSURE_UNIT_PSI;
    flow_global_variables.temperature_unit = TEMPERATURE_UNIT_CELSIUS;
    flow_global_variables.current_screen_id = SCREEN_ID_MAIN; // Initialize to main screen
}

// Cấu hình WiFi
#define WIFI_SSID "1610 B2"
#define WIFI_PASS "123456789"

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        ESP_LOGW(TAG, "WiFi disconnected, retrying...");
        esp_wifi_connect();
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        char ip_str[16];
        esp_ip4addr_ntoa(&event->ip_info.ip, ip_str, sizeof(ip_str));
        ESP_LOGI(TAG, "WiFi connected, got IP: %s", ip_str);
        // start web server here if needed
        ESP_LOGI(TAG, "Starting webserver");
        is_start_webserver = true;
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG, "Device connected to AP: AID=%d, MAC=%02x:%02x:%02x:%02x:%02x:%02x",
                 event->aid,
                 event->mac[0], event->mac[1], event->mac[2],
                 event->mac[3], event->mac[4], event->mac[5]);
        // You can add your custom logic here (e.g., start webserver, notify UI, etc.)
        if (is_ws_init_done() == false)
            ws_server_init();
    }
}

static void wifi_init_sta(void)
{
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    // Register WiFi and IP event handler
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();

    ESP_LOGI(TAG, "Connecting to WiFi SSID:%s", WIFI_SSID);
}

static void wifi_deinit(void)
{
    esp_wifi_stop();
    esp_wifi_deinit();
    esp_event_loop_delete_default();
    esp_netif_deinit();
}

static void wifi_init_ap(void)
{
    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    // Register WiFi event handler
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id);

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "ESP32-Access-Point",
            .ssid_len = strlen("ESP32-Access-Point"),
            .channel = 1,
            .password = "123456789",
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK},
    };
    if (strlen("123456789") == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    esp_wifi_start();

    ESP_LOGI(TAG, "WiFi AP started. SSID:%s password:%s", "ESP32-Access-Point", "123456789");
}