/**
 * @file WS_SERVER.h
 *
 */

#ifndef WS_SERVER_H
#define WS_SERVER_H

#ifdef __cplusplus
extern "C"
{
#endif
    #include <stdbool.h>
    #include <math.h>
    
    #define MIN(a,b) (((a)<(b))?(a):(b))
    #define MAX(a,b) (((a)>(b))?(a):(b))

    void ws_server_init(void);
    bool is_ws_init_done(void);
    void ws_server_stop(void);
    void ws_server_start(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // WS_SERVER_H