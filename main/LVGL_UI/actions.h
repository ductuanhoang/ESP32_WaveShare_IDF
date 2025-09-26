#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_pressure_minotor(lv_event_t * e);
extern void action_temperature_monitor(lv_event_t * e);
extern void action_temperature_change_unit(lv_event_t * e);
extern void action_pressure_change_unit(lv_event_t * e);
extern void action_back_to_main_screen(lv_event_t * e);
extern void action_wifi_scan_button(lv_event_t * e);
extern void action_wifi_connect_button(lv_event_t * e);
extern void action_setttings_button(lv_event_t * e);
extern void action_settings_button(lv_event_t * e);
extern void action_keyboard_event(lv_event_t * e);
extern void action_wifi_settings(lv_event_t * e);
extern void action_rtc_settings(lv_event_t * e);
extern void action_system_settings(lv_event_t * e);
extern void action_rtc_set_time_manual(lv_event_t * e);
extern void action_wifi_save(lv_event_t * e);
extern void action_checkbox_wifi_station(lv_event_t * e);
extern void action_checkbox_wifi_ap(lv_event_t * e);
extern void action_checkbox_wifi_station_ap(lv_event_t * e);
extern void action_checkbox_sync_time(lv_event_t * e);
extern void action_rtc_setting_year_button(lv_event_t * e);
extern void action_rtc_setting_hour_button(lv_event_t * e);
extern void action_back_to_rtc_settings(lv_event_t * e);
extern void action_save_rtc_manual_button(lv_event_t * e);
extern void action_back_to_wifi_settings(lv_event_t * e);
extern void action_checkbox_wifi_off(lv_event_t * e);
extern void action_rtc_setting_hour_ten_changed(lv_event_t * e);
extern void action_rtc_setting_hour_unit_changed(lv_event_t * e);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/