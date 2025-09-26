
#include "actions.h"
#include "vars.h"
#include "common.h"
#include <lvgl.h>
#include "screens.h"
#include "ui.h"
#include "ui_update.h"
#include "PCF85063.h"
#include "user_wifi.h"

#define TAG "ACTIONS"

void action_pressure_minotor(lv_event_t *e)
{
    flow_global_variables.current_screen_id = SCREEN_ID_SCREEN_PRESSURE;
    loadScreen(SCREEN_ID_SCREEN_PRESSURE);
}
void action_temperature_monitor(lv_event_t *e)
{
    flow_global_variables.current_screen_id = SCREEN_ID_SCREEN_TEMPERATURE;
    loadScreen(SCREEN_ID_SCREEN_TEMPERATURE);
}

void action_temperature_change_unit(lv_event_t *e)
{
    lv_obj_t *dropdown = lv_event_get_target(e); // Get the object that triggered the event

    // Option 1: Get the selected option as a string
    char buf[32]; // Buffer to store the selected string
    lv_dropdown_get_selected_str(dropdown, buf, sizeof(buf));
    ESP_LOGI(TAG, "Selected unit (string): %s", buf);

    // Option 2: Get the index of the selected option
    uint16_t selected_index = lv_dropdown_get_selected(dropdown);
    // ESP_LOGI(TAG, "Selected unit (index): %ld", selected_index);

    // You can then use an if-else or switch statement to react to the selected value
    if (strcmp(buf, "C") == 0)
    {
        // Do something when Celsius is selected
        ESP_LOGI(TAG, "Celsius selected!");
        set_var_temperature_unit(TEMPERATURE_UNIT_CELSIUS);
    }
    else if (strcmp(buf, "F") == 0)
    {
        // Do something when Fahrenheit is selected
        ESP_LOGI(TAG, "Fahrenheit selected!");
        set_var_temperature_unit(TEMPERATURE_UNIT_FAHRENHEIT);
    }

    // Or using the index:
    if (selected_index == 0)
    {
        // Celsius (first option)
        ESP_LOGI(TAG, "Celsius selected by index!");
        set_var_temperature_unit(TEMPERATURE_UNIT_CELSIUS);
    }
    else if (selected_index == 1)
    {
        // Fahrenheit (second option)
        ESP_LOGI(TAG, "Fahrenheit selected by index!");
        set_var_temperature_unit(TEMPERATURE_UNIT_FAHRENHEIT);
    }
}

void action_pressure_change_unit(lv_event_t *e)
{
    lv_obj_t *dropdown = lv_event_get_target(e); // Get the object that triggered the event

    // Option 1: Get the selected option as a string
    char buf[32]; // Buffer to store the selected string
    lv_dropdown_get_selected_str(dropdown, buf, sizeof(buf));
    ESP_LOGI(TAG, "Selected unit (string): %s", buf);

    // Option 2: Get the index of the selected option
    uint16_t selected_index = lv_dropdown_get_selected(dropdown);
    ESP_LOGI(TAG, "Selected unit (index): %d", selected_index);

    // You can then use an if-else or switch statement to react to the selected value
    if (strcmp(buf, "PSI") == 0)
    {
        // Do something when PSI is selected
        ESP_LOGI(TAG, "PSI selected!");
        set_var_pressure_unit(PRESSURE_UNIT_PSI);
    }
    else if (strcmp(buf, "mbar") == 0)
    {
        // Do something when mbar is selected
        ESP_LOGI(TAG, "mbar selected!");
        set_var_pressure_unit(PRESSURE_UNIT_mBAR);
    }
}

void action_back_to_main_screen(lv_event_t *e)
{
    // This function should navigate back to the main screen
    // Assuming you have a function to switch screens, e.g., switch_to_main_screen();
    ESP_LOGI(TAG, "Navigating back to main screen...");
    if (flow_global_variables.current_screen_id == SCREEN_ID_SCREEN_SETTINGS)
    {
        // If we are in settings, we need to hide the settings screen
        lv_textarea_set_text(objects.input_password_field, "");
        lv_dropdown_clear_options(objects.input_wifi_list);
    }
    else if (flow_global_variables.current_screen_id == SCREEN_ID_SCREEN_PRESSURE)
    {
        action_back_from_pressure2main(flow_global_variables.pressure_unit);
    }
    flow_global_variables.current_screen_id = SCREEN_ID_MAIN;
    loadScreen(SCREEN_ID_MAIN);
}

static void popup_anim_cb(void *var, int32_t v)
{
    lv_obj_set_style_transform_zoom(var, v, LV_PART_MAIN);
}

void action_wifi_scan_button(lv_event_t *e)
{
    ESP_LOGI(TAG, "Scanning for WiFi networks...");
    flow_global_variables.current_screen_id = SCREEN_ID_SCREEN_WIFI_SCAN;
    loadScreen(SCREEN_ID_SCREEN_WIFI_SCAN);
    // clear the dropdown options
    lv_dropdown_clear_options(objects.input_wifi_list);
    wifi_scan_action();
}

void action_wifi_connect_button(lv_event_t *e)
{
    // go to the main screen
    ESP_LOGI(TAG, "Connecting to WiFi...");
    // get the password from the input field
    const char *password = lv_textarea_get_text(objects.input_password_field);
    ESP_LOGI(TAG, "Password entered: %s", password);
    char ssid[32];
    lv_dropdown_get_selected_str(objects.input_wifi_list, ssid, sizeof(ssid));
    ESP_LOGI(TAG, "SSID selected: %s", ssid);
    if (strlen(ssid) == 0)
    {
        ESP_LOGE(TAG, "SSID is empty. Cannot connect.");
        // lv_obj_clear_flag(lv_obj_get_parent(objects.container_wifi), LV_OBJ_FLAG_HIDDEN);
        show_popup("Error", "Please select a WiFi network.", 5000);
        return;
    }

    // wifi_user_save_credentials(ssid, password);
    // wifi_scan_event_handler(2);
    // wifi_user_connect(ssid, password);
    // Clear the input field and dropdown after connection
    wifi_user_disconnect();
    wifi_user_connect(ssid, password);
    // set to device system
    snprintf(device_system.wifi_ssid, sizeof(device_system.wifi_ssid), "%s", ssid);
    snprintf(device_system.wifi_pass, sizeof(device_system.wifi_pass), "%s", password);
    ESP_LOGI(TAG, "WiFi credentials set in device system.");
    lv_textarea_set_text(objects.input_password_field, "");
    lv_dropdown_clear_options(objects.input_wifi_list);
    flow_global_variables.current_screen_id = SCREEN_ID_MAIN;
    loadScreen(SCREEN_ID_MAIN);
}

void action_settings_button(lv_event_t *e)
{
    // This function should navigate to the settings screen
    // Assuming you have a function to switch screens, e.g., switch_to_settings_screen();
    ESP_LOGI(TAG, "Navigating to settings screen...");
    flow_global_variables.current_screen_id = SCREEN_ID_SCREEN_SETTINGS;
    loadScreen(SCREEN_ID_SCREEN_SETTINGS);
}

void action_keyboard_event(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    uint32_t btn_id = lv_btnmatrix_get_selected_btn(obj);
    // get special button like space
    // Get the text of the button that was pressed
    const char *txt = lv_btnmatrix_get_btn_text(obj, btn_id);

    if (code == LV_EVENT_VALUE_CHANGED)
    {
        // check if the button is character in range charector from 32 to126
        if (*txt > 32 && *txt < 126)
        {
            ESP_LOGI(TAG, "Button pressedtest: %s", txt);
            if (strcmp(txt, "abc") == 0 || strcmp(txt, "ABC") == 0 || strcmp(txt, "123") == 0)
                return;
            if (strcmp(txt, LV_SYMBOL_KEYBOARD) == 0)
                return;
            if (strcmp(txt, "space") == 0)
            {
                // add space character
                lv_textarea_add_char(objects.input_password_field, ' ');
                return;
            }
            lv_textarea_add_char(objects.input_password_field, txt[0]);
        }
        else
        {
            ESP_LOGI(TAG, "button id: %ld", btn_id);
            //
            if (strcmp(txt, LV_SYMBOL_BACKSPACE) == 0)
            {
                // delete the last character
                lv_textarea_del_char(objects.input_password_field);
            }
        }
    }
}

// draft
void action_setttings_button(lv_event_t *e)
{
}

void action_wifi_settings(lv_event_t *e)
{
    flow_global_variables.current_screen_id = SCREEN_ID_SCREEN_WIFI_SETTINGS;
    loadScreen(SCREEN_ID_SCREEN_WIFI_SETTINGS);
}
void action_rtc_settings(lv_event_t *e)
{
    flow_global_variables.current_screen_id = SCREEN_ID_SCREEN_RTC_SETTINGS;
    loadScreen(SCREEN_ID_SCREEN_RTC_SETTINGS);
}
void action_system_settings(lv_event_t *e)
{
    flow_global_variables.current_screen_id = SCREEN_ID_SCREEN_SETTINGS;
    loadScreen(SCREEN_ID_SCREEN_SETTINGS);
}
void action_rtc_set_time_manual(lv_event_t *e)
{
    flow_global_variables.current_screen_id = SCREEN_ID_SCREEN_RTC_SETTINGS;
    loadScreen(SCREEN_ID_SCREEN_RTC_SETTINGS);
}
void action_wifi_save(lv_event_t *e)
{
    // save wifi settings
    ESP_LOGI(TAG, "Saving WiFi settings...");
    flow_global_variables.current_screen_id = SCREEN_ID_MAIN;
    loadScreen(SCREEN_ID_MAIN);
}

void action_checkbox_wifi_station(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        const char *txt = lv_checkbox_get_text(obj);
        const char *state = lv_obj_get_state(obj) & LV_STATE_CHECKED ? "Checked" : "Unchecked";
        bool is_checked = (lv_obj_get_state(obj) & LV_STATE_CHECKED) ? true : false;
        if (is_checked)
        {
            // set uncheck other checkbox
            lv_obj_clear_state(objects.wifi_option_ap, LV_STATE_CHECKED);
            lv_obj_clear_state(objects.wifi_option_off, LV_STATE_CHECKED);
            device_system.wifi_mode = WIFI_CONFIG_MODE_STATION;
        }
        else
        {
            // set state to on
            lv_obj_add_state(objects.wifi_option_station, LV_STATE_CHECKED);
            device_system.wifi_mode = WIFI_CONFIG_MODE_STATION;
        }
        // LV_LOG_USER("%s: %s", txt, state);
        ESP_LOGI(TAG, "%s: %s", txt, state);
    }
}

void action_checkbox_wifi_ap(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        const char *txt = lv_checkbox_get_text(obj);
        const char *state = lv_obj_get_state(obj) & LV_STATE_CHECKED ? "Checked" : "Unchecked";
        // LV_LOG_USER("%s: %s", txt, state);
        bool is_checked = (lv_obj_get_state(obj) & LV_STATE_CHECKED) ? true : false;
        if (is_checked)
        {
            // set uncheck other checkbox
            lv_obj_clear_state(objects.wifi_option_station, LV_STATE_CHECKED);
            lv_obj_clear_state(objects.wifi_option_off, LV_STATE_CHECKED);
            device_system.wifi_mode = WIFI_CONFIG_MODE_AP;
        }
        else
        {
            // set state to on
            lv_obj_add_state(objects.wifi_option_ap, LV_STATE_CHECKED);
            device_system.wifi_mode = WIFI_CONFIG_MODE_AP;
        }
        ESP_LOGI(TAG, "%s: %s", txt, state);
    }
}

void action_checkbox_wifi_off(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        const char *txt = lv_checkbox_get_text(obj);
        const char *state = lv_obj_get_state(obj) & LV_STATE_CHECKED ? "Checked" : "Unchecked";
        // LV_LOG_USER("%s: %s", txt, state);
        bool is_checked = (lv_obj_get_state(obj) & LV_STATE_CHECKED) ? true : false;
        if (is_checked)
        {
            // set uncheck other checkbox
            lv_obj_clear_state(objects.wifi_option_station, LV_STATE_CHECKED);
            lv_obj_clear_state(objects.wifi_option_ap, LV_STATE_CHECKED);
            device_system.wifi_mode = WIFI_CONFIG_OFF;
        }
        else
        {
            // set state to on
            lv_obj_add_state(objects.wifi_option_off, LV_STATE_CHECKED);
            device_system.wifi_mode = WIFI_CONFIG_OFF;
        }
        ESP_LOGI(TAG, "%s: %s", txt, state);
    }
}

void action_checkbox_sync_time(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        const char *txt = lv_checkbox_get_text(obj);
        const char *state = lv_obj_get_state(obj) & LV_STATE_CHECKED ? "Checked" : "Unchecked";
        device_system.auto_sync_time = (lv_obj_get_state(obj) & LV_STATE_CHECKED) ? true : false;
        // LV_LOG_USER("%s: %s", txt, state);
        ESP_LOGI(TAG, "%s: %s", txt, state);
    }
}

void action_rtc_setting_year_button(lv_event_t *e)
{
}

void action_rtc_setting_hour_button(lv_event_t *e)
{
    // change to screen to set hour
    flow_global_variables.current_screen_id = SCREEN_ID_SCREEN_RTC_CHANGE_CLOCK;
    loadScreen(SCREEN_ID_SCREEN_RTC_CHANGE_CLOCK);
    // set the roller to current time
    datetime_t current_time = PCF85063_GetDatetime();
    int hour_ten = current_time.hour / 10;
    int hour_unit = current_time.hour % 10;
    int minute_ten = current_time.minute / 10;
    int minute_unit = current_time.minute % 10;
    lv_roller_set_selected(objects.rtc_roller_hour_ten, hour_ten, LV_ANIM_OFF);
    lv_roller_set_selected(objects.rtc_roller_hour_unit, hour_unit, LV_ANIM_OFF);
    lv_roller_set_selected(objects.rtc_roller_minute_ten, minute_ten, LV_ANIM_OFF);
    lv_roller_set_selected(objects.rtc_roller_minute_unit, minute_unit, LV_ANIM_OFF);
}

void action_back_to_rtc_settings(lv_event_t *e)
{
    // change to screen to set hour
    flow_global_variables.current_screen_id = SCREEN_ID_SCREEN_RTC_SETTINGS;
    loadScreen(SCREEN_ID_SCREEN_RTC_SETTINGS);
}
void action_save_rtc_manual_button(lv_event_t *e)
{
    // get the time from the rollers
    int hour_ten = lv_roller_get_selected(objects.rtc_roller_hour_ten);
    int hour_unit = lv_roller_get_selected(objects.rtc_roller_hour_unit);
    int minute_ten = lv_roller_get_selected(objects.rtc_roller_minute_ten);
    int minute_unit = lv_roller_get_selected(objects.rtc_roller_minute_unit);
    ESP_LOGI(TAG, "Time selected: %d%d:%d%d", hour_ten, hour_unit, minute_ten, minute_unit);
    // back to rtc settings screen
    datetime_t time;
    time.hour = hour_ten * 10 + hour_unit;
    time.minute = minute_ten * 10 + minute_unit;
    time.second = 0;
    PCF85063_Set_Time(time);
    flow_global_variables.current_screen_id = SCREEN_ID_SCREEN_RTC_SETTINGS;
    loadScreen(SCREEN_ID_SCREEN_RTC_SETTINGS);
}

void action_back_to_wifi_settings(lv_event_t *e)
{
    flow_global_variables.current_screen_id = SCREEN_ID_SCREEN_WIFI_SETTINGS;
    loadScreen(SCREEN_ID_SCREEN_WIFI_SETTINGS);
}

void action_rtc_setting_hour_ten_changed(lv_event_t *e)
{
    // get the value from the roller
    int hour_ten = lv_roller_get_selected(objects.rtc_roller_hour_ten);
    ESP_LOGI(TAG, "Hour ten changed: %d", hour_ten);
    if (hour_ten == 2)
    {
        // set the max value of hour unit to 3
        lv_roller_set_options(objects.rtc_roller_hour_unit, "0\n1\n2\n3", LV_ROLLER_MODE_NORMAL);
        // if the current value is greater than 3, set it to 3
        int current_hour_unit = lv_roller_get_selected(objects.rtc_roller_hour_unit);
        if (current_hour_unit > 3)
            lv_roller_set_selected(objects.rtc_roller_hour_unit, 3, LV_ANIM_OFF);
    }
    else
    {
        // set the max value of hour unit to 9
        lv_roller_set_options(objects.rtc_roller_hour_unit, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9", LV_ROLLER_MODE_NORMAL);
    }
}

void action_rtc_setting_hour_unit_changed(lv_event_t *e)
{
    // get the value from the roller
    int hour_unit = lv_roller_get_selected(objects.rtc_roller_hour_unit);
    ESP_LOGI(TAG, "Hour unit changed: %d", hour_unit);
}
