/*
 * delay.h
 * Provides millisecond delay function using SysTick timer.
 * This is useful for timing control in embedded applications.
 *  Created on: May 30, 2025
 *      Author: nguye
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include "stm32f1xx.h"

/**
 * @brief Delay execution for a specified number of milliseconds.
 *
 * @param ms  Number of milliseconds to delay
 */
void delay_ms(uint32_t ms);


#endif /* INC_DELAY_H_ */
