#include <lvgl.h>
#include "vars.h"
#include "common.h"

int32_t get_var_pressure_unit()
{
    return flow_global_variables.pressure_unit;
}

int32_t get_var_temperature_unit()
{
    return flow_global_variables.temperature_unit;
}

void set_var_pressure_unit(int32_t value)
{
    flow_global_variables.pressure_unit = value;
}

void set_var_temperature_unit(int32_t value)
{
    flow_global_variables.temperature_unit = value;
}