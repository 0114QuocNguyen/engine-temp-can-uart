/**********************************************************************************************
 * Include files                                                                              *
 *********************************************************************************************/
#include "can.h"            // CAN peripheral initialization and handling
#include "uart.h"           // UART1 initialization and data transmission
#include "delay.h"          // Delay function using SysTick
#include "can_cyclic.h"     // CAN cyclic buffer management
#include <can_buffer.h>     // CAN buffer structures
#include <stdio.h>          // For sprintf()

/**********************************************************************************************
 * Main                                                                                       *
 *********************************************************************************************/
int main(void) {

    char buf[512];          // Buffer for formatted UART output

    // Initialize CAN GPIOs and configuration
    CAN_GPIO_Init();
    CAN_Config();

    // Initialize UART1
    UART1_Init();

    // Enable interrupt for CAN1 RX FIFO 0
    NVIC_EnableIRQ(CAN1_RX0_IRQn);

    // Main loop
    while (1) {

        // If a new CAN frame has been received
        if (can_rx_flag) {
            can_rx_flag = 0;    // Reset flag

            // Format and send CAN ID and frame type (Std/Ext) over UART
            sprintf(buf, "ID: 0x%03lX [%s], Data: ", rx_id, is_ext ? "Ext" : "Std");
            UART1_SendString(buf);

            // Send data bytes one by one in hexadecimal
            for (int i = 0; i < rx_len; i++) {
                sprintf(buf, "%02X ", rx_data[i]);
                UART1_SendString(buf);
            }

            // Send newline
            UART1_SendString("\r\n");
        }

        // Update cyclic transmission list
        CAN_Cyclic_Update();

        // Small delay to reduce CPU load
        delay_ms(10);
    }

    return 0;
}
/**********************************************************************************************
 * End of file                                                                                *
 *********************************************************************************************/
