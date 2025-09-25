#include "user_keyboard.h"
#include "common.h"
#include "lvgl.h"
#define TAG "KEYBOARD"
static void kb_event_cb(lv_event_t *e);
static void ta_event_cb(lv_event_t *e);
static int id_key_board_number = 0; // 0 for num, 1 for special
/* ALPHA lowercase */
static const char *kb_map_alpha[] = {
    "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "\n",
    "a", "s", "d", "f", "g", "h", "j", "k", "l", "\n",
    "ABC", "z", "x", "c", "v", "b", "n", "m", LV_SYMBOL_BACKSPACE, "\n",
    "123", "space", NULL};

/* ALPHA UPPERCASE layout */
static const char *kb_map_alpha_upper[] = {
    "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "\n",
    "A", "S", "D", "F", "G", "H", "J", "K", "L", "\n",
    "abc", "Z", "X", "C", "V", "B", "N", "M", LV_SYMBOL_BACKSPACE, "\n",
    "123", "space", NULL};

static const lv_btnmatrix_ctrl_t kb_ctrl_alpha[] = {
    /* row1 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* row2 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* row3 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* row4 */ 0, 0, 0};

static const lv_btnmatrix_ctrl_t kb_ctrl_alpha_upper[] = {
    /* row1 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* row2 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* row3 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* row4 */ 0, 0, 0};

/* NUMERIC */
static const char *kb_map_num[] = {
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "\n",
    "-", "/", ";", ":", "(", ")", "$", "&", "@", "\"", "\n",
    ".", ",", "?", "!", "'", "+", "_", "=", "#", "%", "\n",
    "abc", LV_SYMBOL_EJECT, "space", LV_SYMBOL_BACKSPACE, NULL};

static const lv_btnmatrix_ctrl_t kb_ctrl_num[] = {
    /* row1 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* row2 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* row3 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* row4 */ 0, 0, 0, 0};

static const char *kb_map_special[] = {
    "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "\n",
    "-", "_", "=", "+", "{", "}", "[", "]", "|", "\\", "\n",
    ";", ":", "'", "\"", "<", ">", ",", ".", "/", "?", "\n",
    "abc", LV_SYMBOL_EJECT, "space", LV_SYMBOL_BACKSPACE, NULL};

static const lv_btnmatrix_ctrl_t kb_ctrl_special[] = {
    /* row1 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* row2 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* row3 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* row4 */ 0, 0, 0, 0};

typedef struct
{
    lv_obj_t *ta;
    lv_obj_t *kb;
    bool alpha_mode;
} kb_ctx_t;

lv_obj_t *my_keyboard_create(lv_obj_t *parent)
{
    // Create the default LVGL keyboard
    lv_obj_t *kb = lv_keyboard_create(parent);
    // Apply your custom alpha layout as a "user" mode
    lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_USER_1, kb_map_alpha, kb_ctrl_alpha);
    lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_USER_2, kb_map_alpha_upper, kb_ctrl_alpha_upper);
    lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_USER_3, kb_map_num, kb_ctrl_num);
    lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_USER_4, kb_map_special, kb_ctrl_special);
    // Set the initial mode to user 1 (lowercase alpha)
    // Register event callback
    // Switch keyboard to use your custom layout
    lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_USER_1);
    // remove the default event callback
    lv_obj_remove_event_cb(kb, lv_keyboard_def_event_cb);
    lv_obj_add_event_cb(kb, kb_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(kb, kb_event_cb, LV_EVENT_CLICKED, NULL);

    // Style for each button
    lv_obj_set_style_border_width(kb, 2, LV_PART_ITEMS);                      // Border thickness
    lv_obj_set_style_border_color(kb, lv_color_hex(0x9bb7fb), LV_PART_ITEMS); // Light blue border
    lv_obj_set_style_border_opa(kb, LV_OPA_COVER, LV_PART_ITEMS);

    lv_obj_set_style_bg_color(kb, lv_color_hex(0x2d3987), LV_PART_ITEMS); // Dark blue background
    lv_obj_set_style_bg_opa(kb, LV_OPA_COVER, LV_PART_ITEMS);

    // Rounded corners for keys
    lv_obj_set_style_radius(kb, 5, LV_PART_ITEMS);

    // Text style
    lv_obj_set_style_text_color(kb, lv_color_hex(0xFFFFFF), LV_PART_ITEMS); // White text
    lv_obj_set_style_text_font(kb, &lv_font_montserrat_14, LV_PART_ITEMS);
    // Optional: styling
    lv_obj_set_style_bg_opa(kb, LV_OPA_90, 0);
    lv_obj_set_style_pad_all(kb, 6, 0);

    return kb;
}

static void ta_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    kb_ctx_t *ctx = lv_event_get_user_data(e);

    if (code == LV_EVENT_FOCUSED)
    {
        // lv_obj_clear_flag(ctx->kb, LV_OBJ_FLAG_HIDDEN);
        // lv_keyboard_set_textarea(ctx->kb, ctx->ta);
    }
}

static void kb_event_cb(lv_event_t *e)
{
    lv_obj_t *kb = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        // Get pressed key
        ESP_LOGI("KEYBOARD", "Keyboard event changed");
        const char *txt = lv_btnmatrix_get_btn_text(kb, lv_btnmatrix_get_selected_btn(kb));
        if (!txt)
            return;

        // Only handle normal character input here
        if (strcmp(txt, "ABC") != 0 &&
            strcmp(txt, "abc") != 0 &&
            strcmp(txt, "123") != 0)
        {
            ESP_LOGI(TAG, "Normal key pressed: %s", txt);
        }
    }
    else if (code == LV_EVENT_CLICKED)
    {
        ESP_LOGI(TAG, "Keyboard event clicked");
        // Layout change buttons handled here
        const char *txt = lv_btnmatrix_get_btn_text(kb, lv_btnmatrix_get_selected_btn(kb));
        if (!txt)
            return;

        if (strcmp(txt, "ABC") == 0)
        {
            lv_btnmatrix_set_map(kb, kb_map_alpha_upper);
            lv_btnmatrix_set_ctrl_map(kb, kb_ctrl_alpha_upper);
        }
        else if (strcmp(txt, "abc") == 0)
        {
            lv_btnmatrix_set_map(kb, kb_map_alpha);
            lv_btnmatrix_set_ctrl_map(kb, kb_ctrl_alpha);
        }
        else if (strcmp(txt, "123") == 0)
        {
            lv_btnmatrix_set_map(kb, kb_map_num);
            lv_btnmatrix_set_ctrl_map(kb, kb_ctrl_num);
        }
        else if (strcmp(txt, LV_SYMBOL_EJECT) == 0)
        {
            if( id_key_board_number == 0)
            {
                id_key_board_number = 1;
                lv_btnmatrix_set_map(kb, kb_map_special);
                lv_btnmatrix_set_ctrl_map(kb, kb_ctrl_special);
            }
            else
            {
                id_key_board_number = 0;
                lv_btnmatrix_set_map(kb, kb_map_num);
                lv_btnmatrix_set_ctrl_map(kb, kb_ctrl_num);
            }
        }
    }
}
