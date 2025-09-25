// File: sntp_time.h

#ifndef SNTP_TIME_H
#define SNTP_TIME_H

#include <stdint.h>
#include <string>
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

/**
 * @brief Initializes the user SNTP (Simple Network Time Protocol) module.
 * 
 * This function sets the operating mode to SNTP_OPMODE_POLL, sets the server name to "pool.ntp.org",
 * sets the sync mode to SNTP_SYNC_MODE_SMOOTH, and registers the time sync notification callback function.
 * 
 * @note This function should be called before using any SNTP-related functionality.
 */
void user_sntp_init(void);

#endif // SNTP_TIME_H