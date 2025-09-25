#ifndef USER_UI_UPDATE_H
#define USER_UI_UPDATE_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include "common.h"
    void wifi_scan_event_handler(uint8_t id);

    void action_back_from_pressure2main(pressure_unit_t pressure_unit);

    void ui_chart_init_extra(void);
    void ui_screen_update(int screen_id, float pressure_psi, float pressure_mbar, float temperature_celsius, float temperature_fahrenheit);
    void chart_add_data(lv_obj_t *chart, int32_t value, uint8_t screen_id, uint8_t chart_unit);
    void ui_update_wifi_status(void);
    void show_popup(const char *title, const char *message, uint32_t timeout_ms);
    void close_popup(void);
    void ui_update_time(int screen_id, uint8_t hour, uint8_t minute, uint8_t second);
    void update_wifi_settings_screen(void);
#ifdef __cplusplus
}
#endif

#endif /*USER_UI_UPDATE_H*/