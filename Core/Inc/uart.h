/*
 * uart.h
 *UART1 driver header for initialization and communication functions.
 *Supports basic UART transmission and reception using polling and interrupt.
 *  Created on: May 30, 2025
 *      Author: nguye
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include "stm32f1xx.h"

// UART receive buffer and status flags
extern uint8_t rx_buffer[];                    ///< Buffer to store received UART data
extern volatile uint8_t rx_index;              ///< Index for buffer tracking
extern volatile uint8_t uart_rx_complete_flag; ///< Flag to indicate full message received

/**
 * @brief Initialize UART1 on PA9 (TX) and PA10 (RX) with interrupt-based reception.
 */
void UART1_Init(void);

/**
 * @brief Send a single character over UART1.
 * @param c Character to be sent
 */
void UART1_SendChar(char c);

/**
 * @brief Send a null-terminated string over UART1.
 * @param s Pointer to the string
 */
void UART1_SendString(const char *s);

/**
 * @brief UART1 interrupt handler for receiving data.
 */
void UART_IRQHandler(void);

/**
 * @brief Receive a single character (blocking mode).
 * @return Received character
 */
char UART1_ReceiveChar(void);

#endif /* INC_UART_H_ */
