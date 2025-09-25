#ifndef USER_KEYBOARD_H
#define USER_KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include "lvgl.h"

    lv_obj_t * my_keyboard_create(lv_obj_t * parent);
#ifdef __cplusplus
}
#endif

#endif /* USER_KEYBOARD_H */