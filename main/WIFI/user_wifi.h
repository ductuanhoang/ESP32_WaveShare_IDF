// File: user_wifi.h

#ifndef USER_WIFI_H
#define USER_WIFI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void wifi_init_sta(void);

    void user_wifi_deinit(void);

    void wifi_init_ap(void);

    void wifi_scan_action(void);
#ifdef __cplusplus
}
#endif

#endif // USER_WIFI_H