/*
 * uart.c
 *
 *  Created on: May 30, 2025
 *      Author: nguye
 */
/*
 * Include files
 */
#include "uart.h"
#include "can_cyclic.h"

#define UART_BUFFER_SIZE 300                         // Maximum size of UART receive buffer

uint8_t rx_buffer[UART_BUFFER_SIZE];                 // Receive buffer
volatile uint8_t rx_index = 0;                       // Current receive index

volatile uint8_t uart_rx_complete_flag = 0;          // Flag to indicate full message received

// === UART1 Initialization: PA9 (TX), PA10 (RX) ===
void UART1_Init(void) {
    // Enable clock for GPIOA, USART1, and AFIO
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN | RCC_APB2ENR_AFIOEN;

    // Configure PA9 as Alternate Function Push-Pull output at 2 MHz for TX
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= (0b10 << GPIO_CRH_MODE9_Pos) | (0b10 << GPIO_CRH_CNF9_Pos);

    // Configure PA10 as input floating for RX
    GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |= (0b10 << GPIO_CRH_CNF10_Pos);

    // Set baud rate for 115200 bps (assuming 8 MHz clock)
    USART1->BRR = 8000000 / 115200;

    // Enable Transmitter, Receiver, RX interrupt, and USART module
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_UE;

    // Enable USART1 interrupt in NVIC
    NVIC_EnableIRQ(USART1_IRQn);
}

// === Send a single character via UART1 ===
void UART1_SendChar(char c) {
    while (!(USART1->SR & USART_SR_TXE));           // Wait until transmit buffer is empty
    USART1->DR = c;                                  // Send character
}

// === Send a null-terminated string via UART1 ===
void UART1_SendString(const char *s) {
    while (*s) UART1_SendChar(*s++);                // Send characters one by one
}

// === Send raw byte array via UART1 ===
void UART1_SendRawBytes(uint8_t *data, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        while (!(USART1->SR & USART_SR_TXE));       // Wait until ready to transmit
        USART1->DR = data[i];                        // Send byte
    }
}

// === Receive a single character (blocking) ===
char UART1_ReceiveChar(void) {
    while (!(USART1->SR & USART_SR_RXNE));          // Wait until data received
    return USART1->DR;                               // Return received character
}

// === USART1 Interrupt Service Routine ===
void USART1_IRQHandler(void) {
    if (USART1->SR & USART_SR_RXNE) {               // Check if RX register is not empty
        uint8_t byte = USART1->DR;                  // Read received byte

        // Store byte into buffer if within bounds
        if (rx_index < UART_BUFFER_SIZE) {
            rx_buffer[rx_index++] = byte;
        }

        // Check if at least header part is received
        if (rx_index >= 4) {
            uint8_t model = rx_buffer[0];           // Determine model type (0 = STD, 1 = EXT)
            uint8_t len = (model == 0) ? rx_buffer[3] : rx_buffer[5];    // Get data length
            uint8_t header_len = (model == 0) ? 4 : 6;                    // Determine header size
            uint16_t total_len = header_len + len + 2;                   // Total expected length (+2 bytes for cyclic)

            // If full packet received
            if (rx_index >= total_len) {
                // Extract CAN ID
                uint32_t id = (model == 0)
                    ? (rx_buffer[1] << 8 | rx_buffer[2])                        // STD ID: 11-bit
                    : ((uint32_t)rx_buffer[1] << 24 | (uint32_t)rx_buffer[2] << 16 |
                       (uint32_t)rx_buffer[3] << 8 | (uint32_t)rx_buffer[4]);   // EXT ID: 29-bit

                uint8_t* data = &rx_buffer[header_len];                         // Pointer to data field
                uint16_t cyclic = rx_buffer[total_len - 2] << 8 | rx_buffer[total_len - 1]; // CRC/cyclic value

                // Add or update entry in CAN cyclic buffer
                CAN_Cyclic_AddOrUpdate(model, id, data, len, cyclic);

                rx_index = 0;  // Reset buffer index after successful parse
            }
        }
    }
}
/*
 * End of file
 */
