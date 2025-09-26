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
#define WIFI_SSID "tuan123"
#define WIFI_PASS "123456789"

    void wifi_init_sta(const char *ssid, const char *password);

    void user_wifi_deinit(void);

    void wifi_init_ap(void);

    void wifi_scan_action(void);

    void wifi_user_disconnect(void);

    bool get_is_wifi_init_done(void);

    bool get_is_wifi_connected(void);

    void wifi_user_connect(const char *ssid, const char *password);
#ifdef __cplusplus
}
#endif

#endif // USER_WIFI_H