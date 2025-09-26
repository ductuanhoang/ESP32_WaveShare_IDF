

/***********************************************************************************************************************
 * Pragma directive
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes <System Includes>
 ***********************************************************************************************************************/
#include "f9_event.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/
#define TAG "F9_EVENT"

/***********************************************************************************************************************
 * Typedef definitions
 ***********************************************************************************************************************/
ESP_EVENT_DEFINE_BASE(F9_EVENTS);

/***********************************************************************************************************************
 * Private global variables and functions
 ***********************************************************************************************************************/
static esp_event_loop_handle_t event_loop_handle = NULL;
/***********************************************************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Imported global variables and functions (from other files)
 ***********************************************************************************************************************/
void f9_event_init(void)
{
    ESP_LOGI(TAG, "Initializing event module...");
    esp_event_loop_args_t loop_args = {
        .queue_size = 10,
        .task_name = "f9_event_loop_task",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 4096,
        .task_core_id = tskNO_AFFINITY};

    esp_event_loop_create(&loop_args, &event_loop_handle);
}

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
void f9_event_post(uint32_t event_id, void *event_data, uint32_t event_data_size)
{
    esp_event_post_to(event_loop_handle, F9_EVENTS, event_id, event_data, event_data_size, portMAX_DELAY);
}

/**
 * @brief Registers an event handler for the specified event ID.
 *
 * This function registers the provided event handler function to be called when the specified event ID is triggered.
 *
 * @param handler The event handler function to be registered.
 * @param event_id The ID of the event to register the handler for.
 */
void f9_events_register_handler(esp_event_handler_t handler, uint32_t event_id)
{
    esp_event_handler_register_with(event_loop_handle, F9_EVENTS, event_id, handler, &event_loop_handle);
}

/**
 * @brief Unregisters an event handler for the specified event ID.
 *
 * This function unregisters the given event handler from the event loop for the specified event ID.
 *
 * @param handler The event handler function to unregister.
 * @param event_id The ID of the event to unregister the handler from.
 */
void f9_events_unregister_handler(esp_event_handler_t handler, uint32_t event_id)
{
    esp_event_handler_unregister_with(event_loop_handle, F9_EVENTS, event_id, handler);
}
/***********************************************************************************************************************
 * static functions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * End of file
 ***********************************************************************************************************************/