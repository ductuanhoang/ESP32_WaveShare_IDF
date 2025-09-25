

#ifndef USER_COMMON_H
#define USER_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_system.h"
#include "esp_log.h"
#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        PRESSURE_UNIT_PSI,
        PRESSURE_UNIT_mBAR,
    } pressure_unit_t;

    typedef enum
    {
        WIFI_CONFIG_MODE_STATION = 0,
        WIFI_CONFIG_MODE_AP = 1,
        WIFI_CONFIG_OFF = 2
    } wifi_config_mode_t;
    typedef enum
    {
        TEMPERATURE_UNIT_CELSIUS,
        TEMPERATURE_UNIT_FAHRENHEIT
    } temperature_unit_t;

    typedef struct
    {
        pressure_unit_t pressure_unit;
        temperature_unit_t temperature_unit;
        uint8_t current_screen_id; // Current screen ID
    } FlowGlobalVariables_t;

    typedef struct 
    {
        wifi_config_mode_t wifi_mode;
        bool auto_sync_time;
        char wifi_ssid[32];
        char wifi_ip[16];
        bool is_wifi_connected;
        char wifi_ap_ssid[32];
        char wifi_ap_ip[16];
    } System_t;
    

    extern FlowGlobalVariables_t flow_global_variables;
    extern System_t device_system;

    #define GMT_OFFSET_SEC (7 * 3600) // GMT+7 for Vietnam
#ifdef __cplusplus
}
#endif

#endif /*USER_COMMON_H*/