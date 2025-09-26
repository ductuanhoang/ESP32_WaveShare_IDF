#include "ui_update.h"
#include "ui.h"
#include "screens.h"
#include "actions.h"
#include "lvgl.h"
#include "common.h"
#include "images.h"
#include "PCF85063.h"
#include "user_wifi.h"
// #include "wifi_user.h"
// #include "log.h"

#define TEMPERATURE_CELSIUS_MAX 100.0f
#define TEMPERATURE_CELSIUS_MIN 0
#define LV_CHART_POINT_COUNT 10

static void update_temperature_label(lv_obj_t *label, float temperature);
static void update_pressure_label(lv_obj_t *label, float pressure);
static void update_list_down_value(void);

// Conversion helpers
static inline float c_to_f(float c) { return c * 9.0f / 5.0f + 32.0f; }
static inline float f_to_c(float f) { return (f - 32.0f) * 5.0f / 9.0f; }
static inline float psi_to_mbar(float psi) { return psi * 68.9476f; }
static inline float mbar_to_psi(float mbar) { return mbar / 68.9476f; }

uint8_t screen_main_index_value = 0;
uint8_t screen_pressure_index_value = 0;
uint8_t screen_temperature_index_value = 0;
static float pressure_buffer[LV_CHART_POINT_COUNT] = {0};
static float buffer_temperature[LV_CHART_POINT_COUNT] = {0};

static uint8_t old_chart_unit = 0;
static uint8_t old_screen_id = 0;

void ui_screen_update(int screen_id, float pressure_psi, float pressure_mbar, float temperature_celsius, float temperature_fahrenheit)
{
    float pressure = (flow_global_variables.pressure_unit == PRESSURE_UNIT_mBAR) ? pressure_mbar : pressure_psi;
    float temperature = (flow_global_variables.temperature_unit == TEMPERATURE_UNIT_CELSIUS) ? temperature_celsius : temperature_fahrenheit;
    if (screen_id < 1 || screen_id > SCREEN_ID_SCREEN_SETTINGS)
    {
        return; // Invalid screen ID
    }
    update_list_down_value();
    if (screen_id == SCREEN_ID_MAIN)
    {
        lv_obj_t *label = objects.pressure_main_label;
        if (label)
        {
            update_pressure_label(label, pressure);
        }
        label = objects.temperature_main_label;
        if (label)
        {
            update_temperature_label(label, temperature);
        }
    }
    else if (screen_id == SCREEN_ID_SCREEN_PRESSURE)
    {
        lv_obj_t *label = objects.pressure_screen2_label;
        if (label)
        {
            update_pressure_label(label, pressure);
        }
    }
    else if (screen_id == SCREEN_ID_SCREEN_TEMPERATURE)
    {
        lv_obj_t *label = objects.temperature_screen3_label;
        if (label)
        {
            update_temperature_label(label, temperature);
        }
    }
}

void ui_chart_init_extra(void)
{

    lv_chart_set_axis_tick(objects.main_chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 7, 9, true, 60);
    lv_chart_set_axis_tick(objects.main_chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 5, 9, true, 60);
    lv_chart_set_range(objects.main_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 50); // Set range for pressure chart

    // 3. Create a style for the labels with font size 8
    static lv_style_t label_style;
    lv_style_init(&label_style);
    // Make sure this font is enabled in lv_conf.h
    lv_style_set_text_font(&label_style, &lv_font_montserrat_8);

    // 4. Apply the style to the chart's tick/label part
    lv_obj_add_style(objects.main_chart, &label_style, LV_PART_TICKS | LV_STATE_DEFAULT);
    // lv_chart_set_axis_tick(objects.main_chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 12, 3, true, 40);
    // set chart of temperature
    lv_chart_set_axis_tick(objects.temperature_chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 7, 9, true, 60);
    // lv_chart_set_axis_tick(objects.temperature_chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 5, 9, true, 60);
    // lv_obj_add_style(objects.temperature_chart, &label_style, LV_PART_TICKS | LV_STATE_DEFAULT);
    // lv_chart_set_range(objects.temperature_chart, LV_CHART_AXIS_PRIMARY_Y, -20, 100);
    // Set chart Y-axis range from -20 to 100
    lv_chart_set_range(objects.temperature_chart, LV_CHART_AXIS_PRIMARY_Y, -20, 100);

    // Add ticks and labels
    // lv_chart_set_axis_tick(chart, axis, major_len, minor_len, major_cnt, minor_cnt, label_en, draw_size);
    lv_chart_set_axis_tick(objects.temperature_chart,
                           LV_CHART_AXIS_PRIMARY_Y,
                           10,   // major tick length
                           5,    // minor tick length
                           13,   // number of major ticks (-20 → 100 step 10 → 13 ticks)
                           1,    // number of minor ticks between major ticks
                           true, // enable labels
                           60);  // draw size (space reserved for labels)

    // Optional: style for ticks/labels
    lv_obj_add_style(objects.temperature_chart, &label_style,
                     LV_PART_TICKS | LV_STATE_DEFAULT);
    // set chart of pressure
    lv_chart_set_axis_tick(objects.pressure_chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 7, 9, true, 60);
    lv_chart_set_axis_tick(objects.pressure_chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 5, 9, true, 60);
    lv_obj_add_style(objects.pressure_chart, &label_style, LV_PART_TICKS | LV_STATE_DEFAULT);
}

void chart_update_all_data(lv_obj_t *chart, uint8_t screen_id, uint8_t chart_unit)
{
    lv_chart_series_t *ser = lv_chart_get_series_next(chart, NULL);
    if (ser == NULL)
        return;

    for (uint8_t i = 0; i < LV_CHART_POINT_COUNT; i++)
    {
        float value = 0;
        if (screen_id == SCREEN_ID_SCREEN_TEMPERATURE)
        {
            // always keep buffer_temperature[] in Celsius
            value = buffer_temperature[i];
            if (chart_unit == TEMPERATURE_UNIT_FAHRENHEIT)
                value = c_to_f(value);
        }
        else if (screen_id == SCREEN_ID_SCREEN_PRESSURE || screen_id == SCREEN_ID_MAIN)
        {
            // always keep pressure_buffer[] in PSI
            value = pressure_buffer[i];
            if (chart_unit == PRESSURE_UNIT_mBAR)
                value = psi_to_mbar(value);
        }

        ser->y_points[i] = (int32_t)value;
    }

    lv_chart_refresh(chart);
}

void action_back_from_pressure2main(pressure_unit_t pressure_unit)
{
    // Clear the input field and dropdown after returning to main screen
    // update  chart parameters
    if (pressure_unit == PRESSURE_UNIT_PSI)
    {
        lv_chart_set_range(objects.main_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 50);
        lv_label_set_text(objects.main_lable_unit, "PSI");
    }
    else if (pressure_unit == PRESSURE_UNIT_mBAR)
    {
        lv_chart_set_range(objects.main_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 1500);
        lv_label_set_text(objects.main_lable_unit, "mbar");
    }
}

void chart_add_data(lv_obj_t *chart, int32_t value, uint8_t screen_id, uint8_t chart_unit)
{
    // update chart unit if it has changed
    if (old_screen_id != screen_id)
    {
        old_screen_id = screen_id;
        old_chart_unit = 0;
    }

    if (old_chart_unit != chart_unit)
    {
        old_chart_unit = chart_unit;

        // update label unit
        if (screen_id == SCREEN_ID_MAIN)
        {
            if (chart_unit == PRESSURE_UNIT_PSI)
            {
                lv_chart_set_range(objects.main_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 50);
                lv_label_set_text(objects.main_lable_unit, "PSI");
            }
            else if (chart_unit == PRESSURE_UNIT_mBAR)
            {
                lv_chart_set_range(objects.main_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 1500);
                lv_label_set_text(objects.main_lable_unit, "mbar");
            }
        }
        else if (screen_id == SCREEN_ID_SCREEN_PRESSURE)
        {
            if (chart_unit == PRESSURE_UNIT_PSI)
            {
                lv_chart_set_range(objects.pressure_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 50);
                lv_label_set_text(objects.main_lable_unit_2, "PSI");
            }
            else if (chart_unit == PRESSURE_UNIT_mBAR)
            {
                lv_chart_set_range(objects.pressure_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 1500);
                lv_label_set_text(objects.main_lable_unit_2, "mbar");
            }
        }
        else if (screen_id == SCREEN_ID_SCREEN_TEMPERATURE)
        {
            if (chart_unit == TEMPERATURE_UNIT_CELSIUS)
            {
                lv_chart_set_range(objects.temperature_chart, LV_CHART_AXIS_PRIMARY_Y, -20, 100);
                lv_label_set_text(objects.main_lable_unit_1, "Celsius");
            }
            else if (chart_unit == TEMPERATURE_UNIT_FAHRENHEIT)
            {
                lv_chart_set_range(objects.temperature_chart, LV_CHART_AXIS_PRIMARY_Y, 0, 212);
                lv_label_set_text(objects.main_lable_unit_1, "Fahrenheit");
            }
        }

        // 🔥 rescale all previously stored values
        chart_update_all_data(chart, screen_id, chart_unit);
    }

    // ------------------------------
    // add new value (always store raw in base unit)
    // ------------------------------
    uint8_t *p_index = NULL;
    if (screen_id == SCREEN_ID_MAIN)
        p_index = &screen_main_index_value;
    else if (screen_id == SCREEN_ID_SCREEN_PRESSURE)
        p_index = &screen_pressure_index_value;
    else if (screen_id == SCREEN_ID_SCREEN_TEMPERATURE)
        p_index = &screen_temperature_index_value;

    if (!p_index)
        return;

    *p_index = *p_index % LV_CHART_POINT_COUNT; // wrap index

    // store raw in buffer
    if (screen_id == SCREEN_ID_SCREEN_TEMPERATURE)
        buffer_temperature[*p_index] = value; // always store in Celsius
    else if (screen_id == SCREEN_ID_SCREEN_PRESSURE || screen_id == SCREEN_ID_MAIN)
        pressure_buffer[*p_index] = value; // always store in PSI

    // convert for display
    float display_value = value;
    if (screen_id == SCREEN_ID_SCREEN_TEMPERATURE && chart_unit == TEMPERATURE_UNIT_FAHRENHEIT)
        display_value = c_to_f(value);
    else if ((screen_id == SCREEN_ID_SCREEN_PRESSURE || screen_id == SCREEN_ID_MAIN) && chart_unit == PRESSURE_UNIT_mBAR)
        display_value = psi_to_mbar(value);

    lv_chart_series_t *ser = lv_chart_get_series_next(chart, NULL);
    if (ser == NULL)
        ser = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_X);

    ser->y_points[*p_index] = (int32_t)display_value;

    (*p_index)++;

    lv_chart_refresh(chart);
}

void ui_update_wifi_status(void)
{
    if (get_is_wifi_connected())
    {
        lv_img_set_src(objects.wifi_picture, &img_wifi_connected);
    }
    else
    {
        lv_img_set_src(objects.wifi_picture, &img_wifi_disconnected);
    }
}

static lv_obj_t *g_popup = NULL; // biến toàn cục hoặc static để lưu popup

// Callback để auto-close popup
static void popup_timeout_cb(lv_timer_t *timer)
{
    lv_obj_t *popup = (lv_obj_t *)timer->user_data;
    if (popup && lv_obj_is_valid(popup))
    {
        lv_msgbox_close(popup);
    }
    lv_timer_del(timer); // Xóa timer sau khi dùng
    g_popup = NULL;
}

void show_popup(const char *title, const char *message, uint32_t timeout_ms)
{
    // Nếu đã có popup cũ thì đóng trước
    if (g_popup && lv_obj_is_valid(g_popup))
    {
        lv_msgbox_close(g_popup);
        g_popup = NULL;
    }

    // Tạo popup mới
    g_popup = lv_msgbox_create(NULL, title, message, NULL, true);
    lv_obj_center(g_popup);

    // Đổi màu tiêu đề sang đỏ
    lv_obj_t *child = lv_obj_get_child(g_popup, 0); // Title container
    if (child)
    {
        lv_obj_t *title_label = lv_obj_get_child(child, 0); // Label
        if (title_label && lv_obj_check_type(title_label, &lv_label_class))
        {
            lv_obj_set_style_text_color(title_label, lv_color_hex(0xFF0000), LV_PART_MAIN);
        }
    }

    // Tự đóng sau timeout_ms
    if (timeout_ms != 0)
        lv_timer_create(popup_timeout_cb, timeout_ms, g_popup);
}

void close_popup(void)
{
    if (g_popup && lv_obj_is_valid(g_popup))
    {
        lv_msgbox_close(g_popup);
        g_popup = NULL;
    }
}

static bool is_set_check_box = false;
void ui_update_time(int screen_id, uint8_t hour, uint8_t minute, uint8_t second)
{
    char time_str[12]; // HH:MM:SS format
    switch (screen_id)
    {
    case SCREEN_ID_MAIN:
        snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", hour, minute, second);
        lv_label_set_text(objects.current_time, time_str);
        is_set_check_box = false;
        break;
    case SCREEN_ID_SCREEN_RTC_SETTINGS:
        snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", hour, minute, second);
        lv_label_set_text(objects.rtc_setting_hour_label, time_str);
        // set check box state
        if (is_set_check_box == false)
        {
            is_set_check_box = true;
            if (device_system.auto_sync_time)
                lv_obj_add_state(objects.checkbox_sync_time, LV_STATE_CHECKED);
            else
                lv_obj_clear_state(objects.checkbox_sync_time, LV_STATE_CHECKED);
        }
        break;
    default:
        break;
    }
}

static void update_temperature_label(lv_obj_t *label, float temperature)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "Temperature: %.2f", temperature);
    lv_label_set_text(label, buf);
}

static void update_pressure_label(lv_obj_t *label, float pressure)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "Pressure: %.2f", pressure);
    lv_label_set_text(label, buf);
}

static void update_list_down_value(void)
{
    // Update the list down value based on the current screen ID
    switch (flow_global_variables.current_screen_id)
    {
    case SCREEN_ID_MAIN:
        lv_dropdown_set_selected(objects.screen_main_pressure, flow_global_variables.pressure_unit);
        lv_dropdown_set_selected(objects.screen_main_temperature, flow_global_variables.temperature_unit);
        break;
    case SCREEN_ID_SCREEN_PRESSURE:
        lv_dropdown_set_selected(objects.screen_press_unit, flow_global_variables.pressure_unit);
        break;
    case SCREEN_ID_SCREEN_TEMPERATURE:
        lv_dropdown_set_selected(objects.screen_temp_unit, flow_global_variables.temperature_unit);
        break;
    default:
        break;
    }
}

void wifi_scan_event_handler(uint8_t id)
{
    if (id == 0)
    {
        show_popup("WiFi Scan", "Scan done. Please select a network.", 1000);
    }
    else if (id == 1)
    {
        show_popup("WiFi Scanning", "Please wait...", 3000);
    }
    else if (id == 2)
    {
        show_popup("WiFi Connection", "Waiting for connection...", 1000);
    }
}

bool is_first_load_wifi_screen = true;

void update_wifi_settings_screen(void)
{
    if (is_first_load_wifi_screen)
    {
        is_first_load_wifi_screen = false;
        // Clear the input field and dropdown after returning to main screen
        if (device_system.wifi_mode == WIFI_CONFIG_MODE_STATION)
        {
            lv_obj_add_state(objects.wifi_option_station, LV_STATE_CHECKED);
            lv_obj_clear_state(objects.wifi_option_ap, LV_STATE_CHECKED);
            lv_obj_clear_state(objects.wifi_option_off, LV_STATE_CHECKED);
            // set label of button is "Scan Networks"
            lv_label_set_text(objects.label_button_wifi_settings, "Scan Networks");
            // update ssid connect to and ip
            char buf[64];
            snprintf(buf, sizeof(buf), "SSID: %s", device_system.wifi_ssid);
            lv_label_set_text(objects.wifi_setting_ssid, buf);
            if (get_is_wifi_connected() == false)
                snprintf(buf, sizeof(buf), "IP: Not connected");
            else
                snprintf(buf, sizeof(buf), "IP: %s", device_system.wifi_ip);
            lv_label_set_text(objects.wifi_setting_ip, buf);
        }
        // disable other options for now
        lv_obj_add_state(objects.wifi_option_ap, LV_STATE_DISABLED);
        lv_obj_add_state(objects.wifi_option_off, LV_STATE_DISABLED);
        // else if (device_system.wifi_mode == WIFI_CONFIG_MODE_AP)
        // {
        //     // update ssid connect to and ip
        //     // lv_textarea_set_text(objects.wifi_setting_ssid, device_system.wifi_ap_ssid);
        //     // lv_textarea_set_text(objects.wifi_setting_ip, device_system.wifi_ap_ip);
        //     lv_label_set_text(objects.label_button_wifi_settings, "Setup Networks");

        //     lv_obj_add_state(objects.wifi_option_ap, LV_STATE_CHECKED);
        //     lv_obj_clear_state(objects.wifi_option_station, LV_STATE_CHECKED);
        //     lv_obj_clear_state(objects.wifi_option_off, LV_STATE_CHECKED);
        // }
        // else if (device_system.wifi_mode == WIFI_CONFIG_OFF)
        // {
        //     lv_obj_add_state(objects.wifi_option_off, LV_STATE_CHECKED);
        //     lv_obj_clear_state(objects.wifi_option_ap, LV_STATE_CHECKED);
        //     lv_obj_clear_state(objects.wifi_option_station, LV_STATE_CHECKED);

        // }
    }
}

void reset_wifi_settings_screen(void)
{
    is_first_load_wifi_screen = true;
}
