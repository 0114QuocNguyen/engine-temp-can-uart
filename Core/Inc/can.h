/*
 * can.h
 * @file    can.h
 * @brief   Header file for CAN initialization, transmission, and reception on STM32F1.
 *          Provides functions for configuring the CAN peripheral, sending and receiving
 *          standard and extended CAN frames, and handling CAN interrupts.
 * @author  Nguyen
 * @date    May 30, 2025
 */

#ifndef INC_CAN_H_
#define INC_CAN_H_

#include "stm32f1xx.h"

/**
 * @brief Configure GPIO pins used for CAN (e.g., PA11 = RX, PA12 = TX).
 */
void CAN_GPIO_Init(void);

/**
 * @brief Initialize the CAN peripheral with standard settings.
 */
void CAN_Config(void);

/**
 * @brief Send a standard (11-bit ID) CAN frame.
 *
 * @param std_id  Standard 11-bit CAN ID
 * @param data    Pointer to data buffer (up to 8 bytes)
 * @param len     Length of the data (0–8)
 */
void CAN_Send_STD(uint16_t std_id, uint8_t *data, uint8_t len);

/**
 * @brief Send an extended (29-bit ID) CAN frame.
 *
 * @param ext_id  Extended 29-bit CAN ID
 * @param data    Pointer to data buffer (up to 8 bytes)
 * @param len     Length of the data (0–8)
 */
void CAN_Send_EXT(uint32_t ext_id, uint8_t *data, uint8_t len);

/**
 * @brief Receive a CAN frame (either standard or extended).
 *
 * @param data_out      Pointer to buffer for received data
 * @param id_out        Pointer to store received CAN ID
 * @param is_extended   Pointer to flag: 0 = Standard, 1 = Extended
 * @return              1 if a message was received, 0 otherwise
 */
uint8_t CAN_Receive(uint8_t *data_out, uint32_t *id_out, uint8_t *is_extended);

/**
 * @brief Send raw byte array over UART1 (used for debugging or PC communication).
 *
 * @param data    Pointer to data array
 * @param length  Number of bytes to send
 */
void UART1_SendRawBytes(uint8_t *data, uint16_t length);

/**
 * @brief Interrupt handler for CAN RX FIFO 0 (called when a message is received).
 */
void CAN1_RX0_IRQHandler(void);

#endif /* INC_CAN_H_ */

