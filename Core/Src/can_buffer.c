/*
 * can_buffer.c
 *
 *  Created on: Jun 7, 2025
 *      Author: nguye
 */

/*
 * Include file
 */
#include "can_buffer.h"  // Include header file that declares shared CAN buffer variables

// Global flag indicating if a new CAN frame has been received (set in interrupt handler)
volatile uint8_t can_rx_flag = 0;

// Buffer to store received CAN data (up to 8 bytes for both standard and extended frames)
uint8_t rx_data[8];

// Variable to store received CAN identifier (either standard or extended)
uint32_t rx_id;

// Flag indicating if the received ID is an extended ID (1) or a standard ID (0)
uint8_t is_ext;

// Length of the received CAN data (0 to 8 bytes)
uint8_t rx_len;
/*
 * End of file
 */
