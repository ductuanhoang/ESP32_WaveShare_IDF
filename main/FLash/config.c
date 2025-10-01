/*
 * This file is part of the ESP32-XBee distribution (https://github.com/nebkat/esp32-xbee).
 * Copyright (c) 2019 Nebojsa Cvetkovic.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <esp_err.h>
#include <nvs_flash.h>
#include <esp_log.h>
#include <string.h>
#include <esp_wifi_types.h>
#include <driver/gpio.h>
#include <esp_netif_ip_addr.h>
#include "config.h"
#include "common.h"

static const char *TAG = "CONFIG";
static const char *STORAGE = "config";

nvs_handle_t config_handle;

const config_item_t CONFIG_ITEMS[] = {
        // Wifi settings
        {
                .key = KEY_CONFIG_WIFI_USERNAME,
                .type = CONFIG_ITEM_TYPE_STRING,
                .def.str = ""
        }, {
                .key = KEY_CONFIG_WIFI_PASSWORD,
                .type = CONFIG_ITEM_TYPE_STRING,
                .secret = true,
                .def.str = ""
        }, {
                .key = KEY_CONFIG_WIFI_MODE,
                .type = CONFIG_ITEM_TYPE_UINT8,
                .def.uint8 = WIFI_CONFIG_MODE_AP
        }, {
                .key = KEY_CONFIG_AUTO_SYNC_TIME,
                .type = CONFIG_ITEM_TYPE_BOOL,
                .def.bool1 = false
        },
};

const config_item_t *config_items_get(int *count) {
    *count = sizeof(CONFIG_ITEMS) / sizeof(config_item_t);
    return &CONFIG_ITEMS[0];
}

esp_err_t config_set(const config_item_t *item, void *value) {
    switch (item->type) {
        case CONFIG_ITEM_TYPE_BOOL:
            return config_set_bool1(item->key, *((bool *) value));
        case CONFIG_ITEM_TYPE_INT8:
            return config_set_i8(item->key, *((int8_t *)value));
        case CONFIG_ITEM_TYPE_INT16:
            return config_set_i16(item->key, *((int16_t *)value));
        case CONFIG_ITEM_TYPE_INT32:
            return config_set_i32(item->key, *((int32_t *)value));
        case CONFIG_ITEM_TYPE_INT64:
            return config_set_i64(item->key, *((int64_t *)value));
        case CONFIG_ITEM_TYPE_UINT8:
            return config_set_u8(item->key, *((uint8_t *)value));
        case CONFIG_ITEM_TYPE_UINT16:
            return config_set_u16(item->key, *((uint16_t *)value));
        case CONFIG_ITEM_TYPE_UINT32:
            return config_set_u32(item->key, *((uint32_t *)value));
        case CONFIG_ITEM_TYPE_UINT64:
            return config_set_u64(item->key, *((uint64_t *)value));
        case CONFIG_ITEM_TYPE_STRING:
            return config_set_str(item->key, (char *) value);
        default:
            return ESP_ERR_INVALID_ARG;
    }
}

esp_err_t config_set_i8(const char *key, int8_t value) {
    return nvs_set_i8(config_handle, key, value);
}

esp_err_t config_set_i16(const char *key, int16_t value) {
    return nvs_set_i16(config_handle, key, value);
}

esp_err_t config_set_i32(const char *key, int32_t value) {
    return nvs_set_i32(config_handle, key, value);
}

esp_err_t config_set_i64(const char *key, int64_t value) {
    return nvs_set_i64(config_handle, key, value);
}

esp_err_t config_set_u8(const char *key, uint8_t value) {
    // return nvs_set_u8(config_handle, key, value);
    esp_err_t err = nvs_set_u8(config_handle, key, value);
    if (err == ESP_OK) {
        err = nvs_commit(config_handle);
    }
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Saved u8 %s=%u", key, value);
    } else {
        ESP_LOGE(TAG, "Failed set u8 %s (0x%x)", key, err);
    }
    return err;
}

esp_err_t config_set_u16(const char *key, uint16_t value) {
    return nvs_set_u16(config_handle, key, value);
}

esp_err_t config_set_u32(const char *key, uint32_t value) {
    return nvs_set_u32(config_handle, key, value);
}

esp_err_t config_set_u64(const char *key, uint64_t value) {
    return nvs_set_u64(config_handle, key, value);
}

esp_err_t config_set_color(const char *key, config_color_t value) {
    return nvs_set_u32(config_handle, key, value.rgba);
}

esp_err_t config_set_bool1(const char *key, bool value) {
    return nvs_set_i8(config_handle, key, value);
}

esp_err_t config_set_str(const char *key, char *value) {
    ESP_LOGI(TAG, "Setting string key '%s' to '%s'", key, value);
    // return nvs_set_str(config_handle, key, value);
    esp_err_t err = nvs_set_str(config_handle, key, value);
    if (err == ESP_OK) {
        err = nvs_commit(config_handle);
    }
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Saved str %s=%s", key, value);
    } else {
        ESP_LOGE(TAG, "Failed set str %s (0x%x)", key, err);
    }
    return err;
}

esp_err_t config_set_blob(const char *key, char *value, size_t length) {
    return nvs_set_blob(config_handle, key, value, length);
}

esp_err_t config_init() {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_LOGI(TAG, "Opening Non-Volatile Storage (NVS) handle '%s'... ", STORAGE);
    return nvs_open(STORAGE, NVS_READWRITE, &config_handle);
}

int8_t config_get_i8(const config_item_t *item) {
    int8_t value = item->def.int8;
    nvs_get_i8(config_handle, item->key, &value);
    return value;
}

int16_t config_get_i16(const config_item_t *item) {
    int16_t value = item->def.int16;
    nvs_get_i16(config_handle, item->key, &value);
    return value;
}

int32_t config_get_i32(const config_item_t *item) {
    int32_t value = item->def.int32;
    nvs_get_i32(config_handle, item->key, &value);
    return value;
}

int64_t config_get_i64(const config_item_t *item) {
    int64_t value = item->def.int64;
    nvs_get_i64(config_handle, item->key, &value);
    return value;
}

uint8_t config_get_u8(const config_item_t *item) {
    uint8_t value = item->def.uint8;
    nvs_get_u8(config_handle, item->key, &value);
    return value;
}

uint16_t config_get_u16(const config_item_t *item) {
    uint16_t value = item->def.uint16;
    nvs_get_u16(config_handle, item->key, &value);
    return value;
}

uint32_t config_get_u32(const config_item_t *item) {
    uint32_t value = item->def.uint32;
    nvs_get_u32(config_handle, item->key, &value);
    return value;
}

uint64_t config_get_u64(const config_item_t *item) {
    uint64_t value = item->def.uint64;
    nvs_get_u64(config_handle, item->key, &value);
    return value;
}

config_color_t config_get_color(const config_item_t *item) {
    config_color_t value = item->def.color;
    nvs_get_u32(config_handle, item->key, &value.rgba);
    return value;
}

bool config_get_bool1(const config_item_t *item) {
    int8_t value = item->def.bool1;
    nvs_get_i8(config_handle, item->key, &value);
    return value > 0;
}

const config_item_t * config_get_item(const char *key) {
    for (unsigned int i = 0; i < sizeof(CONFIG_ITEMS) / sizeof(config_item_t); i++) {
        const config_item_t *item = &CONFIG_ITEMS[i];
        if (strcmp(item->key, key) == 0) {
            return item;
        }
    }

    // Fatal error
    ESP_ERROR_CHECK(ESP_FAIL);

    return NULL;
}

esp_err_t config_get_primitive(const config_item_t *item, void *out_value) {
    esp_err_t ret;
    switch (item->type) {
        case CONFIG_ITEM_TYPE_BOOL:
            *((bool *) out_value) = item->def.bool1;
            ret = nvs_get_i8(config_handle, item->key, out_value);
            break;
        case CONFIG_ITEM_TYPE_INT8:
            *((int8_t *) out_value) = item->def.int8;
            ret = nvs_get_i8(config_handle, item->key, out_value);
            break;
        case CONFIG_ITEM_TYPE_INT16:
            *((int16_t *) out_value) = item->def.int16;
            ret = nvs_get_i16(config_handle, item->key, out_value);
            break;
        case CONFIG_ITEM_TYPE_INT32:
            *((int32_t *) out_value) = item->def.int32;
            ret = nvs_get_i32(config_handle, item->key, out_value);
            break;
        case CONFIG_ITEM_TYPE_INT64:
            *((int64_t *) out_value) = item->def.int64;
            ret = nvs_get_i64(config_handle, item->key, out_value);
            break;
        case CONFIG_ITEM_TYPE_UINT8:
            *((uint8_t *) out_value) = item->def.uint8;
            ret = nvs_get_u8(config_handle, item->key, out_value);
            break;
        case CONFIG_ITEM_TYPE_UINT16:
            *((uint16_t *) out_value) = item->def.uint16;
            ret = nvs_get_u16(config_handle, item->key, out_value);
            break;
        case CONFIG_ITEM_TYPE_UINT32:
        case CONFIG_ITEM_TYPE_IP:
            *((uint32_t *) out_value) = item->def.uint32;
            ret = nvs_get_u32(config_handle, item->key, out_value);
            break;
        case CONFIG_ITEM_TYPE_UINT64:
            *((uint64_t *) out_value) = item->def.uint64;
            ret = nvs_get_u64(config_handle, item->key, out_value);
            break;
        case CONFIG_ITEM_TYPE_COLOR:
            *((config_color_t *) out_value) = item->def.color;
            ret = nvs_get_u32(config_handle, item->key, out_value);
            break;
        default:
            return ESP_ERR_INVALID_ARG;
    }

    return (ret == ESP_OK || ret == ESP_ERR_NVS_NOT_FOUND) ? ESP_OK : ret;
}

esp_err_t config_get_str_blob_alloc(const config_item_t *item, void **out_value) {
    size_t length;
    esp_err_t ret = config_get_str_blob(item, NULL, &length);
    if (ret != ESP_OK) return ret;
    *out_value = malloc(length);
    return config_get_str_blob(item, *out_value, &length);
}

esp_err_t config_get_str_blob(const config_item_t *item, void *out_value, size_t *length) {
    esp_err_t ret;

    switch (item->type) {
        case CONFIG_ITEM_TYPE_STRING:
            ret = nvs_get_str(config_handle, item->key, out_value, length);
            if (ret == ESP_ERR_NVS_NOT_FOUND) {
                if (length != NULL) *length = strlen(item->def.str) + 1;
                if (out_value != NULL) strcpy(out_value, item->def.str);
            }
            break;
        case CONFIG_ITEM_TYPE_BLOB:
            ret = nvs_get_blob(config_handle, item->key, out_value, length);
            if (ret == ESP_ERR_NVS_NOT_FOUND) {
                if (length != NULL) *length = item->def.blob.length;
                if (out_value != NULL) memcpy(out_value, item->def.blob.data, item->def.blob.length);
            }
            break;
        default:
            return ESP_ERR_INVALID_ARG;
    }

    return (ret == ESP_OK || ret == ESP_ERR_NVS_NOT_FOUND) ? ESP_OK : ret;
}

esp_err_t config_get_str(const char *key, char *out_value, size_t max_len)
{
    size_t required_len = max_len;
    esp_err_t err = nvs_get_str(config_handle, key, out_value, &required_len);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Read str %s=%s", key, out_value);
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "Key %s not found", key);
    } else {
        ESP_LOGE(TAG, "Error reading %s (0x%x)", key, err);
    }
    return err;
}


esp_err_t config_commit(void) {
    return nvs_commit(config_handle);
}
