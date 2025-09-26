/**
 * @brief Initializes the user event subsystem
 */
#ifndef PC_COM_H
#define PC_COM_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C"
{
#endif
#include "esp_event_base.h"
  
    void pc_com_init(void);
    void pc_com_send_data(const char *data, size_t length);
#ifdef __cplusplus
}
#endif

#endif // PC_COM_H