/*
 * can_cyclic.c
 *
 *  Created on: Jun 6, 2025
 *      Author: nguye
 */
/***************************************************
 * inlcucde files
 **************************************************/

#include "can_cyclic.h"   // Header for this module
#include "can.h"          // Provides CAN_Send_STD / CAN_Send_EXT
#include <string.h>       // For memcpy
#define MAX_CYCLIC_MSGS 10  // Maximum number of cyclic messages we can store
// Structure to hold each cyclic message entry
typedef struct {
    uint8_t in_use;         // 1 if slot is used, 0 if free
    uint8_t model;          // 0 = Standard ID, 1 = Extended ID
    uint32_t id;            // CAN ID
    uint8_t data[8];        // Data payload (up to 8 bytes)
    uint8_t len;            // Actual data length
    uint16_t interval;      // Repeat interval in milliseconds (cyclic)
    uint16_t counter;       // Counts 10ms ticks to trigger sending
} CyclicMsg;
// Static array to hold all active cyclic messages
static CyclicMsg msgs[MAX_CYCLIC_MSGS];
// Add a new cyclic message or update an existing one by ID
void CAN_Cyclic_AddOrUpdate(uint8_t model, uint32_t id, uint8_t *data, uint8_t len, uint16_t cyclic_ms) {
    if (cyclic_ms == 0) {
        // Gửi một lần
        if (model == 0)
            CAN_Send_STD((uint16_t)id, data, len);
        else
            CAN_Send_EXT(id, data, len);

        // Nếu đã tồn tại message cùng ID -> xóa đi để tránh giữ lại
        for (int i = 0; i < MAX_CYCLIC_MSGS; i++) {
            if (msgs[i].in_use && msgs[i].id == id) {
                msgs[i].in_use = 0;  // Giải phóng slot
                break;
            }
        }
        return;
    }
    // First pass: Check if message with same ID already exists
    for (int i = 0; i < MAX_CYCLIC_MSGS; i++) {
        if (msgs[i].in_use && msgs[i].id == id) {
            // Update existing message
            memcpy(msgs[i].data, data, len);     // Copy new data
            msgs[i].len = len;                   // Update length
            msgs[i].interval = cyclic_ms;        // Update cyclic interval
            msgs[i].counter = 0;                 // Reset timer
            if (model == 0)
                CAN_Send_STD((uint16_t)id, data, len);
            else
                CAN_Send_EXT(id, data, len);
            return;                              // Done
        }
    }
    // Second pass: Add a new message in the first free slot
    for (int i = 0; i < MAX_CYCLIC_MSGS; i++) {
        if (!msgs[i].in_use) {
            msgs[i].in_use = 1;                  // Mark slot as used
            msgs[i].model = model;               // Save ID model
            msgs[i].id = id;                     // Save CAN ID
            memcpy(msgs[i].data, data, len);     // Copy data
            msgs[i].len = len;                   // Set length
            msgs[i].interval = cyclic_ms;        // Set cyclic interval
            msgs[i].counter = 0;                 // Initialize counter
            if (model == 0)
                CAN_Send_STD((uint16_t)id, data, len);
            else
                CAN_Send_EXT(id, data, len);
            return;                              // Done
        }
    }
}
// This function should be called every 10ms to check and send due messages
void CAN_Cyclic_Update(void) {
    for (int i = 0; i < MAX_CYCLIC_MSGS; i++) {
        if (msgs[i].in_use && msgs[i].interval > 0) {
            msgs[i].counter++;  // Count each 10ms tick

            // Check if it's time to send this message
            if (msgs[i].counter * 10 >= msgs[i].interval) {
                if (msgs[i].model == 0)
                    CAN_Send_STD((uint16_t)msgs[i].id, msgs[i].data, msgs[i].len); // Send Standard ID
                else
                    CAN_Send_EXT(msgs[i].id, msgs[i].data, msgs[i].len);           // Send Extended ID
                msgs[i].counter = 0; // Reset the timer after sending
            }
        }
    }
}
/******************************************************
 * End of file
 *****************************************************/
