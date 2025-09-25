

/***********************************************************************************************************************
 * Pragma directive
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes <System Includes>
 ***********************************************************************************************************************/
#include "sntp_time.h"
#include <esp_sntp.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"
#include <map>
/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/
#define TAG "SNTP_TIME"

/***********************************************************************************************************************
 * Typedef definitions
 ***********************************************************************************************************************/
#define TIME_SYNC_SERVER 24 * 60 * 60
#define TIME_SYNC_SERVER_SHORT_TIME 10
/***********************************************************************************************************************
 * Private global variables and functions
 ***********************************************************************************************************************/
static void sntp_time_set_handler(struct timeval *tv);
static std::string get_timezone_code(uint16_t timezone_index);
/***********************************************************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 ***********************************************************************************************************************/
bool user_sntp_sync(void);
std::map<std::string, std::string> timezone_table = {
    {"GMT-12:00", "GMT+12"},
    {"GMT-11:00", "SST11"},
    {"GMT-10:00", "HST10"},
    {"GMT-09:00", "AKST9AKDT,M3.2.0,M11.1.0"},
    {"GMT-08:00", "PST8PDT,M3.2.0,M11.1.0"},
    {"GMT-07:00", "MST7MDT,M3.2.0,M11.1.0"},
    {"GMT-06:00", "CST6CDT,M3.2.0,M11.1.0"},
    {"GMT-05:00", "EST5EDT,M3.2.0,M11.1.0"},
    {"GMT-04:00", "AST4ADT,M3.2.0,M11.1.0"},
    {"GMT-03:00", "GMT-3"},
    {"GMT-02:00", "GMT-2"},
    {"GMT-01:00", "AZOT1AZOST,M3.5.0/0,M10.5.0/1"},
    {"GMT+00:00", "GMT0BST,M3.5.0/1,M10.5.0"},
    {"GMT+01:00", "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"GMT+02:00", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
    {"GMT+03:00", "MSK-3"},
    {"GMT+04:00", "GST-4"},
    {"GMT+05:00", "PKT-5"},
    {"GMT+06:00", "ALMT-6"},
    {"GMT+07:00", "WIB-7"},
    {"GMT+08:00", "CST-8"},
    {"GMT+09:00", "JST-9"},
    {"GMT+10:00", "AEST-10AEDT,M10.5.0,M4.1.0/3"},
    {"GMT+11:00", "SBT-11"},
    {"GMT+12:00", "NZST-12NZDT,M9.5.0,M4.1.0/3"}};

std::string timezone_table_int[] = {
    "GMT-12:00",
    "GMT-11:00",
    "GMT-11:00",
    "GMT-11:00",
    "GMT-11:00",
    "GMT-11:00",
    "GMT-10:00",
    "GMT-10:00",
    "GMT-10:00",
    "GMT-10:00",
    "GMT-09:30",
    "GMT-09:00",
    "GMT-09:00",
    "GMT-08:00",
    "GMT-08:00",
    "GMT-08:00",
    "GMT-08:00",
    "GMT-08:00",
    "GMT-08:00",
    "GMT-08:00",
    "GMT-08:00",
    "GMT-07:00",
    "GMT-07:00",
    "GMT-07:00",
    "GMT-07:00",
    "GMT-07:00",
    "GMT-07:00",
    "GMT-07:00",
    "GMT-07:00",
    "GMT-07:00",
    "GMT-07:00",
    "GMT-07:00",
    "GMT-06:00",
    "GMT-06:00",
    "GMT-06:00",
    "GMT-06:00",
    "GMT-06:00",
    "GMT-06:00",
    "GMT-06:00",
    "GMT-06:00",
    "GMT-06:00",
    "GMT-06:00",
    "GMT-06:00",
    "GMT-06:00",
    "GMT-06:00",
    "GMT-06:00",
    "GMT-06:00",
    "GMT-06:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-05:00",
    "GMT-04:30",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-04:00",
    "GMT-03:30",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-03:00",
    "GMT-02:00",
    "GMT-02:00",
    "GMT-02:00",
    "GMT-02:00",
    "GMT-01:00",
    "GMT-01:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+00:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+01:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+02:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:00",
    "GMT+03:30",
    "GMT+04:00",
    "GMT+04:00",
    "GMT+04:00",
    "GMT+04:00",
    "GMT+04:00",
    "GMT+04:00",
    "GMT+04:00",
    "GMT+04:00",
    "GMT+04:00",
    "GMT+04:00",
    "GMT+04:00",
    "GMT+04:00",
    "GMT+04:30",
    "GMT+05:00",
    "GMT+05:00",
    "GMT+05:00",
    "GMT+05:00",
    "GMT+05:00",
    "GMT+05:00",
    "GMT+05:00",
    "GMT+05:00",
    "GMT+05:00",
    "GMT+05:00",
    "GMT+05:00",
    "GMT+05:00",
    "GMT+05:30",
    "GMT+05:30",
    "GMT+05:30",
    "GMT+05:45",
    "GMT+06:00",
    "GMT+06:00",
    "GMT+06:00",
    "GMT+06:00",
    "GMT+06:00",
    "GMT+06:00",
    "GMT+06:00",
    "GMT+06:00",
    "GMT+06:00",
    "GMT+06:00",
    "GMT+06:30",
    "GMT+06:30",
    "GMT+06:30",
    "GMT+07:00",
    "GMT+07:00",
    "GMT+07:00",
    "GMT+07:00",
    "GMT+07:00",
    "GMT+07:00",
    "GMT+07:00",
    "GMT+07:00",
    "GMT+07:00",
    "GMT+07:00",
    "GMT+07:00",
    "GMT+07:00",
    "GMT+07:00",
    "GMT+08:00",
    "GMT+08:00",
    "GMT+08:00",
    "GMT+08:00",
    "GMT+08:00",
    "GMT+08:00",
    "GMT+08:00",
    "GMT+08:00",
    "GMT+08:00",
    "GMT+08:00",
    "GMT+08:00",
    "GMT+08:00",
    "GMT+08:00",
    "GMT+08:00",
    "GMT+08:00",
    "GMT+08:00",
    "GMT+08:00",
    "GMT+08:30",
    "GMT+08:30",
    "GMT+09:00",
    "GMT+09:00",
    "GMT+09:00",
    "GMT+09:00",
    "GMT+09:00",
    "GMT+09:00",
    "GMT+09:00",
    "GMT+09:00",
    "GMT+09:30",
    "GMT+09:30",
    "GMT+09:30",
    "GMT+10:00",
    "GMT+10:00",
    "GMT+10:00",
    "GMT+10:00",
    "GMT+10:00",
    "GMT+10:00",
    "GMT+10:00",
    "GMT+10:30",
    "GMT+10:30",
    "GMT+11:00",
    "GMT+11:00",
    "GMT+11:00",
    "GMT+11:00",
    "GMT+11:00",
    "GMT+11:00",
    "GMT+11:00",
    "GMT+11:00",
    "GMT+11:00",
    "GMT+11:00",
    "GMT+11:00",
    "GMT+11:00",
    "GMT+11:00",
    "GMT+12:00",
    "GMT+12:00",
    "GMT+12:00",
    "GMT+12:00",
    "GMT+12:00",
    "GMT+12:00",
    "GMT+12:00",
    "GMT+12:00",
    "GMT+12:00",
    "GMT+12:00",
    "GMT+12:00",
    "GMT+12:00",
    "GMT+12:45",
    "GMT+13:00",
    "GMT+13:00",
    "GMT+13:00",
    "GMT+14:00",
    "GMT+14:00"};

static std::string timezone_local_code = "EST5EDT,M3.2.0,M11.1.0";

static void sntp_time_tasks(void *pvParameters);
/***********************************************************************************************************************
 * Imported global variables and functions (from other files)
 ***********************************************************************************************************************/

/**
 * @brief Initializes the user SNTP (Simple Network Time Protocol) module.
 *
 * This function sets the operating mode to SNTP_OPMODE_POLL, sets the server name to "pool.ntp.org",
 * sets the sync mode to SNTP_SYNC_MODE_SMOOTH, and registers the time sync notification callback function.
 *
 * @note This function should be called before using any SNTP-related functionality.
 */
void user_sntp_init(void)
{
    xTaskCreate(sntp_time_tasks, "sntp_time_tasks", 4096, NULL, 10, NULL);
}

/**
 * @brief Starts the user SNTP (Simple Network Time Protocol) functionality.
 *
 * This function sets the timezone to China Standard Time (CST-8) and retrieves the current date and time in Shanghai.
 *
 * @note This function requires the ESP_LOGI macro to be defined.
 */
void user_sntp_start(void)
{
    ESP_LOGI(TAG, "*************user_sntp_start");
    time_t now;
    char strftime_buf[64];
    struct tm timeinfo;

    time(&now);
    // Set timezone to China Standard Time
    setenv("TZ", "CST-8", 1);
    tzset();

    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "The current date/time is: %s", strftime_buf);
}

void user_sntp_stop(void)
{
}

/**
 * @brief Synchronizes the system time using SNTP protocol.
 *
 * This function initializes the SNTP configuration with the default NTP server "pool.ntp.org",
 * sets the sync mode to smooth, sets the sync interval to TIME_SYNC_SERVER,
 * and registers a callback function for time sync notifications.
 * It then waits for the system time to be updated within a 10-second timeout.
 * If the time update fails, it logs an error message.
 */
bool user_sntp_sync(void)
{
    bool ret = false;
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
    sntp_set_sync_interval(TIME_SYNC_SERVER);
    sntp_set_time_sync_notification_cb(sntp_time_set_handler);
    if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(30000)) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to update system time within 10s timeout");
        ret = false;
    }
    else
        ret = true;
    return ret;
}
/***********************************************************************************************************************
 * static functions
 ***********************************************************************************************************************/
static void sntp_time_tasks(void *pvParameters)
{
    bool ret = false;
    ESP_LOGI(TAG, "*************user_sntp_init");
    // wait_for_ip();
    uint16_t timezone_index = 1;
    // get timezone index from interflash
    if (get_timezone_code(timezone_index) != "")
    {
        timezone_local_code = get_timezone_code(timezone_index);
    }
    else
    {
        timezone_local_code = "EST5EDT,M3.2.0,M11.1.0";
    }
    ret = user_sntp_sync();
    while (1)
    {
        if (ret == false)
        {
            ESP_LOGI(TAG, "SNTP sync failed, retrying in 60s");
            sntp_set_sync_interval(TIME_SYNC_SERVER_SHORT_TIME);
        }
        else
        {
            ESP_LOGI(TAG, "SNTP sync success, retrying in 24h");
            sntp_set_sync_interval(TIME_SYNC_SERVER);
            vTaskDelete(NULL);
        }
        vTaskDelay(60000 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Retrieves the timezone code based on the given timezone index.
 *
 * This function takes a timezone index as input and returns the corresponding timezone code.
 * The timezone code is retrieved from the timezone_table using the timezone index.
 *
 * @param timezone_index The index of the timezone in the timezone_table.
 * @return The timezone code as a std::string. If the timezone index is out of range, an empty string is returned.
 */
static std::string get_timezone_code(uint16_t timezone_index)
{
    int array_size = sizeof(timezone_table_int);
    int element_size = sizeof(std::string);
    int num_elements = sizeof(timezone_table_int) / sizeof(std::string);

    if (timezone_index > num_elements)
        return "";

    std::string timezone = timezone_table_int[timezone_index];
    ESP_LOGI(TAG, "timezone = %s\r\n", timezone.c_str());

    std::string timezone_code = timezone_table.at(timezone.c_str()); // using at() function
    ESP_LOGI(TAG, "timezone_code = %s\r\n", timezone_code.c_str());
    return timezone_code;
}

/**
 * @brief Sets the system time based on the SNTP response.
 *
 * This function is called when the SNTP client receives a response and successfully synchronizes the system time.
 * It sets the system time based on the received timeval structure and adjusts it according to the local timezone.
 * Finally, it logs the current date and time in the local timezone.
 *
 * @param tv Pointer to the timeval structure containing the synchronized time.
 */
static void sntp_time_set_handler(struct timeval *tv)
{
    ESP_LOGI(TAG, "Synced time from SNTP");
    time_t now;
    char strftime_buf[64];
    struct tm timeinfo;

    time(&now);
    // now = now - GMT+7
    // Set timezone to China Standard Time
    setenv("TZ", timezone_local_code.c_str(), 1);
    tzset();

    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "The current date/time is: %s", strftime_buf);
}

/***********************************************************************************************************************
 * End of file
 ***********************************************************************************************************************/