#ifndef ESP32_XBEE_CONFIG_H
#define ESP32_XBEE_CONFIG_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stddef.h>

typedef enum {
    CONFIG_ITEM_TYPE_BOOL = 0,
    CONFIG_ITEM_TYPE_INT8,
    CONFIG_ITEM_TYPE_INT16,
    CONFIG_ITEM_TYPE_INT32,
    CONFIG_ITEM_TYPE_INT64,
    CONFIG_ITEM_TYPE_UINT8,
    CONFIG_ITEM_TYPE_UINT16,
    CONFIG_ITEM_TYPE_UINT32,
    CONFIG_ITEM_TYPE_UINT64,
    CONFIG_ITEM_TYPE_STRING,
    CONFIG_ITEM_TYPE_BLOB,
    CONFIG_ITEM_TYPE_COLOR,
    CONFIG_ITEM_TYPE_IP,
    CONFIG_ITEM_TYPE_MAX
} config_item_type_t;

typedef union {
    struct values {
        uint8_t alpha;
        uint8_t blue;
        uint8_t green;
        uint8_t red;
    } values;
    uint32_t rgba;
} config_color_t;

typedef union {
    bool bool1;
    int8_t int8;
    int16_t int16;
    int32_t int32;
    int64_t int64;
    uint8_t uint8;
    uint16_t uint16;
    uint32_t uint32;
    uint64_t uint64;
    config_color_t color;
    char *str;
    struct blob {
        uint8_t *data;
        size_t length;
    } blob;

} config_item_value_t;

typedef struct config_item {
    char *key;
    config_item_type_t type;
    bool secret;
    config_item_value_t def;
} config_item_t;

#define CONFIG_VALUE_UNCHANGED "\x1a\x1a\x1a\x1a\x1a\x1a\x1a\x1a"

// WiFi
#define KEY_CONFIG_WIFI_USERNAME    "wifi_user"
#define KEY_CONFIG_WIFI_PASSWORD    "wifi_pass"
#define KEY_CONFIG_WIFI_MODE       "wifi_mode"
#define KEY_CONFIG_AUTO_SYNC_TIME           "sync_time"


esp_err_t config_init();

const config_item_t *config_items_get(int *count);
const config_item_t * config_get_item(const char *key);

#define CONF_ITEM( key ) config_get_item(key)

bool config_get_bool1(const config_item_t *item);
int8_t config_get_i8(const config_item_t *item);
int16_t config_get_i16(const config_item_t *item);
int32_t config_get_i32(const config_item_t *item);
int64_t config_get_i64(const config_item_t *item);
uint8_t config_get_u8(const config_item_t *item);
uint16_t config_get_u16(const config_item_t *item);
uint32_t config_get_u32(const config_item_t *item);
uint64_t config_get_u64(const config_item_t *item);
config_color_t config_get_color(const config_item_t *item);

esp_err_t config_set(const config_item_t *item, void *value);
esp_err_t config_set_bool1(const char *key, bool value);
esp_err_t config_set_i8(const char *key, int8_t value);
esp_err_t config_set_i16(const char *key, int16_t value);
esp_err_t config_set_i32(const char *key, int32_t value);
esp_err_t config_set_i64(const char *key, int64_t value);
esp_err_t config_set_u8(const char *key, uint8_t value);
esp_err_t config_set_u16(const char *key, uint16_t value);
esp_err_t config_set_u32(const char *key, uint32_t value);
esp_err_t config_set_u64(const char *key, uint64_t value);
esp_err_t config_set_color(const char *key, config_color_t value);
esp_err_t config_set_str(const char *key, char *value);
esp_err_t config_set_blob(const char *key, char *value, size_t length);

esp_err_t config_get_str_blob_alloc(const config_item_t *item, void **out_value);
esp_err_t config_get_str_blob(const config_item_t *item, void *out_value, size_t *length);
esp_err_t config_get_primitive(const config_item_t *item, void *out_value);
esp_err_t config_get_str(const char *key, char *out_value, size_t max_len);

esp_err_t config_commit(void);
#ifdef __cplusplus
}
#endif

#endif //ESP32_XBEE_CONFIG_H
