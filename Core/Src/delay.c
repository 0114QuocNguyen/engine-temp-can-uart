/*
 * delay.c
 *
 *  Created on: May 30, 2025
 *      Author: nguye
 */
/*
 * Include files
 */
#include "delay.h"

// Function to create a delay in milliseconds
void delay_ms(uint32_t ms) {
    SysTick->LOAD = 8000 - 1;           // Set reload value for 1ms delay (assuming 8 MHz system clock)
    SysTick->VAL = 0;                   // Clear current SysTick counter value
    SysTick->CTRL = 5;                  // Enable SysTick with processor clock, no interrupt (CLKSOURCE = 1, ENABLE = 1)

    for (uint32_t i = 0; i < ms; i++) { // Loop for each millisecond
        while ((SysTick->CTRL & (1 << 16)) == 0); // Wait until COUNTFLAG is set (1ms elapsed)
    }

    SysTick->CTRL = 0;                  // Disable SysTick after delay
}
/*
 * End of file
 */
