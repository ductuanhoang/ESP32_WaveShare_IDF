#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            // current_time
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.current_time = obj;
            lv_obj_set_pos(obj, 156, 2);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "00:00:00");
        }
        {
            // pressure_main_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.pressure_main_label = obj;
            lv_obj_set_pos(obj, 22, 33);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Pressure: 13254");
        }
        {
            // temperature_main_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.temperature_main_label = obj;
            lv_obj_set_pos(obj, 22, 81);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Temperature :  123");
        }
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            lv_obj_set_pos(obj, 263, 71);
            lv_obj_set_size(obj, 42, 32);
            lv_obj_add_event_cb(obj, action_temperature_monitor, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, -1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "View");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // screen_main_pressure
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.screen_main_pressure = obj;
            lv_obj_set_pos(obj, 171, 23);
            lv_obj_set_size(obj, 80, LV_SIZE_CONTENT);
            lv_dropdown_set_options(obj, "PSI\nmbar");
            lv_obj_add_event_cb(obj, action_pressure_change_unit, LV_EVENT_VALUE_CHANGED, (void *)0);
        }
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            lv_obj_set_pos(obj, 264, 29);
            lv_obj_set_size(obj, 42, 32);
            lv_obj_add_event_cb(obj, action_pressure_minotor, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, -1, -1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "View");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // screen_main_temperature
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.screen_main_temperature = obj;
            lv_obj_set_pos(obj, 171, 71);
            lv_obj_set_size(obj, 80, LV_SIZE_CONTENT);
            lv_dropdown_set_options(obj, "C\nF");
            lv_obj_add_event_cb(obj, action_temperature_change_unit, LV_EVENT_VALUE_CHANGED, (void *)0);
        }
        {
            // main_chart
            lv_obj_t *obj = lv_chart_create(parent_obj);
            objects.main_chart = obj;
            lv_obj_set_pos(obj, 30, 120);
            lv_obj_set_size(obj, 275, 100);
            lv_obj_set_scroll_snap_x(obj, LV_DIR_NONE);
            lv_obj_set_scroll_snap_x(obj, LV_SCROLL_SNAP_START);
            lv_obj_set_scroll_snap_y(obj, LV_SCROLL_SNAP_START);
        }
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            lv_obj_set_pos(obj, 0, 1);
            lv_obj_set_size(obj, 77, 28);
            lv_obj_add_event_cb(obj, action_settings_button, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Settings");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // main_lable_unit
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.main_lable_unit = obj;
            lv_obj_set_pos(obj, 4, 107);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "PSI");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_8, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 302, 220);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "time");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_8, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // wifi_picture
            lv_obj_t *obj = lv_img_create(parent_obj);
            objects.wifi_picture = obj;
            lv_obj_set_pos(obj, 296, 5);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_wifi_connected);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 113, 2);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Time:");
        }
    }
}

void tick_screen_main() {
}

void create_screen_screen_pressure() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.screen_pressure = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            lv_obj_set_pos(obj, 9, 9);
            lv_obj_set_size(obj, 64, 32);
            lv_obj_add_event_cb(obj, action_back_to_main_screen, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Back");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // pressure_chart
            lv_obj_t *obj = lv_chart_create(parent_obj);
            objects.pressure_chart = obj;
            lv_obj_set_pos(obj, 30, 107);
            lv_obj_set_size(obj, 270, 110);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 106, 20);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Pressure Graph");
        }
        {
            // pressure_screen2_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.pressure_screen2_label = obj;
            lv_obj_set_pos(obj, 16, 60);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Pressure: 13456");
        }
        {
            // screen_press_unit
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.screen_press_unit = obj;
            lv_obj_set_pos(obj, 229, 49);
            lv_obj_set_size(obj, 69, LV_SIZE_CONTENT);
            lv_dropdown_set_options(obj, "PSI\nmbar");
            lv_obj_add_event_cb(obj, action_pressure_change_unit, LV_EVENT_VALUE_CHANGED, (void *)0);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 302, 218);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "time");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_8, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // main_lable_unit_2
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.main_lable_unit_2 = obj;
            lv_obj_set_pos(obj, 9, 98);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "PSI");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_8, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
}

void tick_screen_screen_pressure() {
}

void create_screen_screen_temperature() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.screen_temperature = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            lv_obj_set_pos(obj, 9, 16);
            lv_obj_set_size(obj, 64, 32);
            lv_obj_add_event_cb(obj, action_back_to_main_screen, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Back");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 0, 7);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Back");
            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 0, 7);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Next");
            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // temperature_chart
            lv_obj_t *obj = lv_chart_create(parent_obj);
            objects.temperature_chart = obj;
            lv_obj_set_pos(obj, 30, 106);
            lv_obj_set_size(obj, 270, 110);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 89, 25);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Temperature Graph");
        }
        {
            // temperature_screen3_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.temperature_screen3_label = obj;
            lv_obj_set_pos(obj, 9, 66);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Temperature: 13456");
        }
        {
            // screen_temp_unit
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.screen_temp_unit = obj;
            lv_obj_set_pos(obj, 224, 57);
            lv_obj_set_size(obj, 74, LV_SIZE_CONTENT);
            lv_dropdown_set_options(obj, "C\nF");
            lv_obj_add_event_cb(obj, action_temperature_change_unit, LV_EVENT_VALUE_CHANGED, (void *)0);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 298, 216);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "time");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_8, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // main_lable_unit_1
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.main_lable_unit_1 = obj;
            lv_obj_set_pos(obj, 13, 90);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Celsius");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_8, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
}

void tick_screen_screen_temperature() {
}

void create_screen_screen_settings() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.screen_settings = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_imgbtn_create(parent_obj);
            lv_obj_set_pos(obj, 254, 2);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, 42);
            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, &img_logo, NULL);
            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_PRESSED, NULL, &img_logo, NULL);
            lv_obj_add_event_cb(obj, action_back_to_main_screen, LV_EVENT_PRESSED, (void *)0);
        }
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            lv_obj_set_pos(obj, 1, 1);
            lv_obj_set_size(obj, 64, 32);
            lv_obj_add_event_cb(obj, action_back_to_main_screen, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Back");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            lv_obj_set_pos(obj, 73, 62);
            lv_obj_set_size(obj, 176, 39);
            lv_obj_add_event_cb(obj, action_wifi_settings, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Wi-Fi Settings");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            lv_obj_set_pos(obj, 73, 120);
            lv_obj_set_size(obj, 176, 39);
            lv_obj_add_event_cb(obj, action_rtc_settings, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "RTC Settings");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            lv_obj_set_pos(obj, 73, 176);
            lv_obj_set_size(obj, 176, 39);
            lv_obj_add_event_cb(obj, action_system_settings, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "System Settings");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 124, 1);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Settings");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 272, 223);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Rev 1.0");
        }
    }
}

void tick_screen_screen_settings() {
}

void create_screen_screen_wifi_settings() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.screen_wifi_settings = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            // wifi_option_station
            lv_obj_t *obj = lv_checkbox_create(parent_obj);
            objects.wifi_option_station = obj;
            lv_obj_set_pos(obj, 18, 48);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_checkbox_set_text(obj, "Wi-Fi Station");
            lv_obj_add_event_cb(obj, action_checkbox_wifi_station, LV_EVENT_VALUE_CHANGED, (void *)0);
        }
        {
            // wifi_option_ap
            lv_obj_t *obj = lv_checkbox_create(parent_obj);
            objects.wifi_option_ap = obj;
            lv_obj_set_pos(obj, 18, 80);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_checkbox_set_text(obj, "Wi-Fi AP");
            lv_obj_add_event_cb(obj, action_checkbox_wifi_ap, LV_EVENT_VALUE_CHANGED, (void *)0);
        }
        {
            // button_wifi_settings
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.button_wifi_settings = obj;
            lv_obj_set_pos(obj, 94, 159);
            lv_obj_set_size(obj, 149, 31);
            lv_obj_add_event_cb(obj, action_wifi_scan_button, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_button_wifi_settings
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_button_wifi_settings = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Scan Networks");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 105, 5);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Wi-Fi Settings");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // wifi_setting_ssid
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.wifi_setting_ssid = obj;
            lv_obj_set_pos(obj, 18, 191);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "SSID: ");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // wifi_setting_ip
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.wifi_setting_ip = obj;
            lv_obj_set_pos(obj, 18, 213);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "IP: ");
        }
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 64, 32);
            lv_obj_add_event_cb(obj, action_back_to_main_screen, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Back");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            lv_obj_set_pos(obj, 254, 80);
            lv_obj_set_size(obj, 65, 33);
            lv_obj_add_event_cb(obj, action_wifi_save, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Save");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_imgbtn_create(parent_obj);
            lv_obj_set_pos(obj, 254, 0);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, 42);
            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, &img_logo, NULL);
            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_PRESSED, NULL, &img_logo, NULL);
        }
        {
            // wifi_option_off
            lv_obj_t *obj = lv_checkbox_create(parent_obj);
            objects.wifi_option_off = obj;
            lv_obj_set_pos(obj, 18, 111);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_checkbox_set_text(obj, "Wi-Fi Off");
            lv_obj_add_event_cb(obj, action_checkbox_wifi_off, LV_EVENT_VALUE_CHANGED, (void *)0);
        }
    }
}

void tick_screen_screen_wifi_settings() {
}

void create_screen_screen_rtc_settings() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.screen_rtc_settings = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff5f5f5), LV_PART_MAIN | LV_STATE_CHECKED);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 105, 1);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "RTC Settings");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 64, 32);
            lv_obj_add_event_cb(obj, action_back_to_main_screen, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Back");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_checkbox_create(parent_obj);
            lv_obj_set_pos(obj, 59, 153);
            lv_obj_set_size(obj, 203, LV_SIZE_CONTENT);
            lv_checkbox_set_text(obj, "Auto Sync with server");
            lv_obj_add_event_cb(obj, action_checkbox_sync_time, LV_EVENT_VALUE_CHANGED, (void *)0);
        }
        {
            lv_obj_t *obj = lv_imgbtn_create(parent_obj);
            lv_obj_set_pos(obj, 253, 1);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, 42);
            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, &img_logo, NULL);
            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_PRESSED, NULL, &img_logo, NULL);
        }
        {
            // rtc_setting_hour
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.rtc_setting_hour = obj;
            lv_obj_set_pos(obj, 83, 75);
            lv_obj_set_size(obj, 150, 48);
            lv_obj_add_event_cb(obj, action_rtc_setting_hour_button, LV_EVENT_PRESSED, (void *)0);
            lv_obj_set_style_bg_grad_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff5f5f5), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // rtc_setting_hour_label
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.rtc_setting_hour_label = obj;
                    lv_obj_set_pos(obj, -1, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "12:00:00");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }
}

void tick_screen_screen_rtc_settings() {
}

void create_screen_screen_rtc_change_clock() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.screen_rtc_change_clock = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 105, 1);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "RTC Settings");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 64, 32);
            lv_obj_add_event_cb(obj, action_back_to_rtc_settings, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Back");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_imgbtn_create(parent_obj);
            lv_obj_set_pos(obj, 253, 1);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, 42);
            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, &img_logo, NULL);
            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_PRESSED, NULL, &img_logo, NULL);
        }
        {
            // rtc_roller_hour_ten
            lv_obj_t *obj = lv_roller_create(parent_obj);
            objects.rtc_roller_hour_ten = obj;
            lv_obj_set_pos(obj, 46, 77);
            lv_obj_set_size(obj, 42, 87);
            lv_roller_set_options(obj, "0\n1\n2", LV_ROLLER_MODE_NORMAL);
        }
        {
            // rtc_roller_hour_unit
            lv_obj_t *obj = lv_roller_create(parent_obj);
            objects.rtc_roller_hour_unit = obj;
            lv_obj_set_pos(obj, 105, 77);
            lv_obj_set_size(obj, 42, 87);
            lv_roller_set_options(obj, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9", LV_ROLLER_MODE_NORMAL);
        }
        {
            // rtc_roller_minute_ten
            lv_obj_t *obj = lv_roller_create(parent_obj);
            objects.rtc_roller_minute_ten = obj;
            lv_obj_set_pos(obj, 179, 77);
            lv_obj_set_size(obj, 42, 87);
            lv_roller_set_options(obj, "0\n1\n2\n3\n4\n5", LV_ROLLER_MODE_NORMAL);
        }
        {
            // rtc_roller_minute_unit
            lv_obj_t *obj = lv_roller_create(parent_obj);
            objects.rtc_roller_minute_unit = obj;
            lv_obj_set_pos(obj, 238, 77);
            lv_obj_set_size(obj, 42, 87);
            lv_roller_set_options(obj, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9", LV_ROLLER_MODE_NORMAL);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 159, 113);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, ":");
        }
        {
            // checkbox_sync_time
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.checkbox_sync_time = obj;
            lv_obj_set_pos(obj, 125, 185);
            lv_obj_set_size(obj, 82, 35);
            lv_obj_add_event_cb(obj, action_save_rtc_manual_button, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "save");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }
}

void tick_screen_screen_rtc_change_clock() {
}

void create_screen_screen_wifi_scan() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.screen_wifi_scan = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_keyboard_create(parent_obj);
            lv_obj_set_pos(obj, 10, 115);
            lv_obj_set_size(obj, 300, 120);
            lv_obj_add_event_cb(obj, action_keyboard_event, LV_EVENT_VALUE_CHANGED, (void *)0);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            lv_obj_set_pos(obj, 252, 49);
            lv_obj_set_size(obj, 67, 36);
            lv_obj_add_event_cb(obj, action_wifi_connect_button, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "connect");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // input_password_field
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.input_password_field = obj;
            lv_obj_set_pos(obj, 77, 49);
            lv_obj_set_size(obj, 164, 36);
            lv_textarea_set_max_length(obj, 128);
            lv_textarea_set_one_line(obj, false);
            lv_textarea_set_password_mode(obj, false);
        }
        {
            // input_wifi_list
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.input_wifi_list = obj;
            lv_obj_set_pos(obj, 77, 2);
            lv_obj_set_size(obj, 164, LV_SIZE_CONTENT);
            lv_dropdown_set_options(obj, "");
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 0, 62);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Password");
        }
        {
            lv_obj_t *obj = lv_imgbtn_create(parent_obj);
            lv_obj_set_pos(obj, 254, 2);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, 42);
            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, &img_logo, NULL);
            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_PRESSED, NULL, &img_logo, NULL);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 64, 32);
            lv_obj_add_event_cb(obj, action_back_to_wifi_settings, LV_EVENT_PRESSED, (void *)0);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Back");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }
}

void tick_screen_screen_wifi_scan() {
}


void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
    create_screen_screen_pressure();
    create_screen_screen_temperature();
    create_screen_screen_settings();
    create_screen_screen_wifi_settings();
    create_screen_screen_rtc_settings();
    create_screen_screen_rtc_change_clock();
    create_screen_screen_wifi_scan();
}

typedef void (*tick_screen_func_t)();

tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_screen_pressure,
    tick_screen_screen_temperature,
    tick_screen_screen_settings,
    tick_screen_screen_wifi_settings,
    tick_screen_screen_rtc_settings,
    tick_screen_screen_rtc_change_clock,
    tick_screen_screen_wifi_scan,
};

void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
