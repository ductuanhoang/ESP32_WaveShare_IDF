#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations



// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_PRESSURE_UNIT = 0,
    FLOW_GLOBAL_VARIABLE_TEMPERATURE_UNIT = 1,
    FLOW_GLOBAL_VARIABLE_WIFI_SCAN_VISIBLE = 2
};

// Native global variables

extern int32_t get_var_pressure_unit();
extern void set_var_pressure_unit(int32_t value);
extern int32_t get_var_temperature_unit();
extern void set_var_temperature_unit(int32_t value);
extern bool get_var_wifi_scan_visible();
extern void set_var_wifi_scan_visible(bool value);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/