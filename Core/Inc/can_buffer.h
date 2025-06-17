/*
 * can_buffer.h
 * @brief   Shared buffer and flags for CAN receive interrupt handling.
 *  Created on: Jun 7, 2025
 *      Author: nguye
 */

#ifndef SRC_CAN_BUFFER_H_
#define SRC_CAN_BUFFER_H_
#include <stdint.h>

/**
 * @brief Flag set to 1 when a new CAN frame is received.
 */
extern volatile uint8_t can_rx_flag;

/**
 * @brief Buffer for received CAN data (max 8 bytes).
 */
extern uint8_t rx_data[8];

/**
 * @brief ID of the received CAN message.
 */
extern uint32_t rx_id;

/**
 * @brief Flag indicating if the received ID is extended (1) or standard (0).
 */
extern uint8_t is_ext;

/**
 * @brief Length of the received CAN data in bytes.
 */
extern uint8_t rx_len;
#endif /* SRC_CAN_BUFFER_H_ */
