#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *screen_pressure;
    lv_obj_t *screen_temperature;
    lv_obj_t *screen_settings;
    lv_obj_t *screen_wifi_settings;
    lv_obj_t *screen_rtc_settings;
    lv_obj_t *screen_rtc_change_clock;
    lv_obj_t *screen_wifi_scan;
    lv_obj_t *current_time;
    lv_obj_t *pressure_main_label;
    lv_obj_t *temperature_main_label;
    lv_obj_t *screen_main_pressure;
    lv_obj_t *screen_main_temperature;
    lv_obj_t *main_chart;
    lv_obj_t *main_lable_unit;
    lv_obj_t *wifi_picture;
    lv_obj_t *pressure_chart;
    lv_obj_t *pressure_screen2_label;
    lv_obj_t *screen_press_unit;
    lv_obj_t *main_lable_unit_2;
    lv_obj_t *temperature_chart;
    lv_obj_t *temperature_screen3_label;
    lv_obj_t *screen_temp_unit;
    lv_obj_t *main_lable_unit_1;
    lv_obj_t *wifi_option_station;
    lv_obj_t *wifi_option_ap;
    lv_obj_t *wifi_setting_ssid;
    lv_obj_t *wifi_setting_ip;
    lv_obj_t *rtc_setting_hour;
    lv_obj_t *rtc_setting_hour_label;
    lv_obj_t *rtc_roller_hour_ten;
    lv_obj_t *rtc_roller_hour_unit;
    lv_obj_t *rtc_roller_minute_ten;
    lv_obj_t *rtc_roller_minute_unit;
    lv_obj_t *checkbox_sync_time;
    lv_obj_t *input_password_field;
    lv_obj_t *input_wifi_list;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_SCREEN_PRESSURE = 2,
    SCREEN_ID_SCREEN_TEMPERATURE = 3,
    SCREEN_ID_SCREEN_SETTINGS = 4,
    SCREEN_ID_SCREEN_WIFI_SETTINGS = 5,
    SCREEN_ID_SCREEN_RTC_SETTINGS = 6,
    SCREEN_ID_SCREEN_RTC_CHANGE_CLOCK = 7,
    SCREEN_ID_SCREEN_WIFI_SCAN = 8,
};

void create_screen_main();
void tick_screen_main();

void create_screen_screen_pressure();
void tick_screen_screen_pressure();

void create_screen_screen_temperature();
void tick_screen_screen_temperature();

void create_screen_screen_settings();
void tick_screen_screen_settings();

void create_screen_screen_wifi_settings();
void tick_screen_screen_wifi_settings();

void create_screen_screen_rtc_settings();
void tick_screen_screen_rtc_settings();

void create_screen_screen_rtc_change_clock();
void tick_screen_screen_rtc_change_clock();

void create_screen_screen_wifi_scan();
void tick_screen_screen_wifi_scan();

void create_screens();
void tick_screen(int screen_index);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/