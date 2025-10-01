/**
 * @brief Initializes the user event subsystem
 */
#ifndef F9_EVENT_H
#define F9_EVENT_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C"
{
#endif
#include "esp_event_base.h"
    typedef enum
    {
        E_EVENT_WIFI_CONNECTED = 0,
        E_EVENT_WIFI_DISCONNECTED,
        E_EVENT_WIFI_GOT_IP,

        E_EVENT_WIFI_AP_STARTED,
        E_EVENT_WIFI_AP_STOPPED,
        E_EVENT_MAX
    } e_f9_event;

    void f9_event_init(void);

    /**
     * @brief Posts an event to the OlympiaLED event loop.
     *
     * This function is used to post an event to the OlympiaLED event loop. The event is identified by the provided event ID,
     * and can optionally include event data. The event data is passed as a void pointer, along with the size of the event data.
     * The event is posted with a maximum delay of portMAX_DELAY, which ensures that the event is immediately added to the event queue.
     *
     * @param event_id The ID of the event to be posted.
     * @param event_data A pointer to the event data.
     * @param event_data_size The size of the event data.
     */
    void f9_event_post(uint32_t event_id, void *event_data, uint32_t event_data_size);

    /**
     * @brief Registers an event handler for the specified event ID.
     *
     * This function registers the provided event handler function to be called when the specified event ID is triggered.
     *
     * @param handler The event handler function to be registered.
     * @param event_id The ID of the event to register the handler for.
     */
    void f9_events_register_handler(esp_event_handler_t handler, uint32_t event_id);

    /**
     * @brief Unregisters an event handler for the specified event ID.
     *
     * This function unregisters the given event handler from the event loop for the specified event ID.
     *
     * @param handler The event handler function to unregister.
     * @param event_id The ID of the event to unregister the handler from.
     */
    void f9_events_unregister_handler(esp_event_handler_t handler, uint32_t event_id);
#ifdef __cplusplus
}
#endif

#endif // F9_EVENT_H