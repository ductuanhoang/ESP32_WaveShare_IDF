

/***********************************************************************************************************************
 * Pragma directive
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes <System Includes>
 ***********************************************************************************************************************/
#include "pc_com.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "driver/uart.h"
#include "driver/gpio.h"
/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/
#define TAG "PC_COM"

/***********************************************************************************************************************
 * Typedef definitions
 ***********************************************************************************************************************/
#define UART_PORT_NUM      UART_NUM_0
/***********************************************************************************************************************
 * Private global variables and functions
 ***********************************************************************************************************************/
static void pc_com_uart_init(void);
/***********************************************************************************************************************
 * Exported global variables and functions (to be accessed by other files)
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Imported global variables and functions (from other files)
 ***********************************************************************************************************************/
void pc_com_init(void)
{
    ESP_LOGI(TAG, "Initializing PC_COM module...");
    // uart initialization or other setup code can be added here
    pc_com_uart_init();
}

void pc_com_send_data(const char *data, size_t length)
{
    if (length == 0 || data == NULL)
    {
        ESP_LOGW(TAG, "No data to send.");
        return;
    }
    int tx_bytes = uart_write_bytes(UART_PORT_NUM, data, length);
    if (tx_bytes < 0)
    {
        ESP_LOGE(TAG, "Failed to send data over UART.");
    }
    else
    {
        ESP_LOGI(TAG, "Sent %d bytes over UART.", tx_bytes);
    }
}
/***********************************************************************************************************************
 * static functions
 ***********************************************************************************************************************/
static void pc_com_uart_init(void)
{
    // UART initialization code here
    ESP_LOGI(TAG, "PC_COM UART initialized.");
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    // Cấu hình UART
    uart_driver_install(UART_PORT_NUM, 128 * 2, 0, 0, NULL, 0);
    uart_param_config(UART_PORT_NUM, &uart_config);
    uart_set_pin(UART_PORT_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}
/***********************************************************************************************************************
 * End of file
 ***********************************************************************************************************************/