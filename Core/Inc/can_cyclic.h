/*
 * can_cyclic.h
 * @brief   Interface for cyclic CAN message transmission.
 *          Allows adding or updating CAN messages to be sent at regular intervals.
 *  Created on: Jun 6, 2025
 *      Author: nguye
 */

#ifndef INC_CAN_CYCLIC_H_
#define INC_CAN_CYCLIC_H_

#include <stdint.h>

/**
 * @brief Add a new cyclic CAN message or update an existing one.
 *
 * This function stores a CAN message that should be transmitted periodically
 * with a specified time interval.
 *
 * @param model       0 for Standard ID, 1 for Extended ID
 * @param id          CAN identifier (11-bit or 29-bit depending on model)
 * @param data        Pointer to data buffer (up to 8 bytes)
 * @param len         Number of data bytes (0–8)
 * @param cyclic_ms   Transmission interval in milliseconds
 */
void CAN_Cyclic_AddOrUpdate(uint8_t model, uint32_t id, uint8_t *data, uint8_t len, uint16_t cyclic_ms);

/**
 * @brief Check and transmit all pending cyclic CAN messages.
 *
 * This function should be called periodically (e.g., every 10 ms)
 * to ensure that all registered cyclic messages are sent at the correct time.
 */
void CAN_Cyclic_Update(void);


#endif /* INC_CAN_CYCLIC_H_ */
