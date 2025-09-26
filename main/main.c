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
#include "user_wifi.h"
#include "f9_event.h"
#include "pc_com.h"

#define TAG "main"
static void device_data_init(void);
static void show_graphics(void);
static void common_data_init(void);
static void user_wifi_event_handler(void *handler_arg, esp_event_base_t base, int32_t id, void *event_data);
static char *get_device_data_packet(const char *time_stamp, float temperature, float pressure);
bool is_start_webserver = false;
FlowGlobalVariables_t flow_global_variables;
System_t device_system;

void Driver_Loop(void *parameter)
{
    while (1)
    {
        PCF85063_Loop();
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
            else if (flow_global_variables.current_screen_id == SCREEN_ID_MAIN)
            {
                ui_update_wifi_status();
                reset_wifi_settings_screen();
            }
            else
            {
                reset_wifi_settings_screen();
            }
        }
        static uint32_t send_data_interval = 0;
        if (xTaskGetTickCount() - send_data_interval > pdMS_TO_TICKS(2000))
        {
            send_data_interval = xTaskGetTickCount();
            char buffer[128];
            datetime_t datetime;
            datetime = PCF85063_GetDatetime();
            snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d",datetime.hour, datetime.minute, datetime.second);
            pc_com_send_data(get_device_data_packet(buffer, get_temperature_celsius(), get_pressure_mbar()), 128);
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
    device_data_init();
    nvs_flash_init();
    ws_server_init();
    f9_event_init();
    f9_events_register_handler(user_wifi_event_handler, E_EVENT_WIFI_CONNECTED);
    f9_events_register_handler(user_wifi_event_handler, E_EVENT_WIFI_DISCONNECTED);
    f9_events_register_handler(user_wifi_event_handler, E_EVENT_WIFI_GOT_IP);
    common_data_init();
    pc_com_init();
    Driver_Init();
    wifi_init_sta(device_system.wifi_ssid, device_system.wifi_pass);

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

static void device_data_init(void)
{
    device_system.wifi_mode = WIFI_CONFIG_MODE_STATION;
    // get ssid and password from the internal flash if needed
    // for now, use hardcoded values
    snprintf(device_system.wifi_ssid, sizeof(device_system.wifi_ssid), "%s", WIFI_SSID);
    snprintf(device_system.wifi_pass, sizeof(device_system.wifi_pass), "%s", WIFI_PASS);
    snprintf(device_system.wifi_ip, sizeof(device_system.wifi_ip), "%s", "");
    snprintf(device_system.wifi_ap_ssid, sizeof(device_system.wifi_ap_ssid), "ESP32-Force-Five");
    snprintf(device_system.wifi_ap_ip, sizeof(device_system.wifi_ap_ip), "%s", "");
    device_system.auto_sync_time = false;
    device_system.is_wifi_connected = false;
}

static void user_wifi_event_handler(void *handler_arg, esp_event_base_t base, int32_t id, void *event_data)
{
    switch (id)
    {
    case E_EVENT_WIFI_CONNECTED:
        ESP_LOGI(TAG, "WiFi connected event received");
        device_system.is_wifi_connected = true;
        break;
    case E_EVENT_WIFI_DISCONNECTED:
        ESP_LOGI(TAG, "WiFi disconnected event received");
        device_system.is_wifi_connected = false;
        break;
    case E_EVENT_WIFI_GOT_IP:
        ESP_LOGI(TAG, "WiFi got IP event received");
        // start webserver when got IP
        ESP_LOGI(TAG, "Free heap: %ld", esp_get_free_heap_size());
        ws_server_start();
        break;
    default:
        ESP_LOGW(TAG, "Unhandled WiFi event ID: %ld", id);
        break;
    }
}

static char *get_device_data_packet(const char *time_stamp, float temperature, float pressure)
{
    // #,TIMESTAMP,PRESSURE,TEMPERATURE,;CRLF
    // Create a packet with device data
    static char packet[128];
    snprintf(packet, sizeof(packet), "#,%s,%.2f,%.2f;\r\n", time_stamp, pressure, temperature);
    return packet;
}