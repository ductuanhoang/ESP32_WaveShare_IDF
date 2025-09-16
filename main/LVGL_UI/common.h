

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
        TEMPERATURE_UNIT_CELSIUS,
        TEMPERATURE_UNIT_FAHRENHEIT
    } temperature_unit_t;

    typedef struct
    {
        pressure_unit_t pressure_unit;
        temperature_unit_t temperature_unit;
        uint8_t current_screen_id; // Current screen ID
    } FlowGlobalVariables_t;

    extern FlowGlobalVariables_t flow_global_variables;

    #define GMT_OFFSET_SEC (7 * 3600) // GMT+7 for Vietnam
#ifdef __cplusplus
}
#endif

#endif /*USER_COMMON_H*/