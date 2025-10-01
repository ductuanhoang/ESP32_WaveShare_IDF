// File: user_wifi.h

#ifndef USER_WIFI_H
#define USER_WIFI_H

#include <stdint.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C"
{
#endif

    // Cấu hình WiFi
#define WIFI_SSID "1610 B2"
#define WIFI_PASS "123456789"

    void wifi_net_init(void);
    void wifi_init_sta(const char *ssid, const char *pass);
    void wifi_init_ap(const char *ssid, const char *pass);
    void wifi_off(void);
    void wifi_scan_action(void);
    void wifi_user_connect(const char *ssid, const char *pass);

    bool get_is_wifi_connected(void);
#ifdef __cplusplus
}
#endif

#endif // USER_WIFI_H