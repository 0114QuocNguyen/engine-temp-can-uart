/*
 * can.c
 *
 *  Created on: May 30, 2025
 *      Author: nguye
 */
/*
 * Include files
 */
#include <can.h>           // Include CAN header for function declarations
#include <can_buffer.h>    // Include CAN buffer to access shared variables

// === Initialize GPIO pins for CAN (PA11 - RX, PA12 - TX) ===
void CAN_GPIO_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;  // Enable clock for GPIOA and AFIO
    AFIO->MAPR &= ~AFIO_MAPR_CAN_REMAP;  // Use default CAN pin mapping (PA11, PA12)

    // Configure PA11 (CAN_RX) as input with pull-up
    GPIOA->CRH &= ~(GPIO_CRH_MODE11 | GPIO_CRH_CNF11);              // Clear mode and config bits
    GPIOA->CRH |= (0b10 << GPIO_CRH_CNF11_Pos);                     // Set input with pull-up/down
    GPIOA->ODR |= (1 << 11);                                        // Enable pull-up resistor

    // Configure PA12 (CAN_TX) as alternate function push-pull output
    GPIOA->CRH &= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12);              // Clear mode and config bits
    GPIOA->CRH |= (0b10 << GPIO_CRH_MODE12_Pos)                     // Output mode, 2 MHz
                | (0b10 << GPIO_CRH_CNF12_Pos);                     // Alternate function push-pull
}

// === Configure CAN in normal mode ===
void CAN_Config(void) {
    RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;  // Enable CAN1 peripheral clock

    CAN1->MCR |= CAN_MCR_INRQ;           // Request initialization mode
    while (!(CAN1->MSR & CAN_MSR_INAK)); // Wait until initialization mode is acknowledged
    CAN1->MCR &= ~CAN_MCR_SLEEP;         // Exit sleep mode

    // Set CAN bit timing (prescaler, segment1, segment2) for 500kbps or as needed
    CAN1->BTR = (0b00 << 24)       // Normal mode (no loopback or silent)
              | (0 << 20)          // SJW = 1
              | (5 << 16)          // TS1 = 6
              | (1);               // TS2 = 2, Prescaler = 1 (divide clock accordingly)

    // === Configure filter to accept all messages ===
    CAN1->FMR |= CAN_FMR_FINIT;                // Enter filter init mode
    CAN1->FA1R |= (1 << 0);                    // Activate filter 0
    CAN1->FS1R |= (1 << 0);                    // Set filter 0 to 32-bit mode
    CAN1->FM1R &= ~(1 << 0);                   // Set filter 0 to mask mode
    CAN1->sFilterRegister[0].FR1 = 0;          // Filter ID = 0
    CAN1->sFilterRegister[0].FR2 = 0;          // Filter mask = 0 (accept all)
    CAN1->FMR &= ~CAN_FMR_FINIT;               // Exit filter init mode

    CAN1->MCR &= ~CAN_MCR_INRQ;                // Leave initialization mode

    CAN1->IER |= CAN_IER_FMPIE0;               // Enable FIFO 0 message pending interrupt

    NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);      // Enable CAN1 RX0 interrupt in NVIC
    NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 1); // Set priority of CAN1 RX0 interrupt

    while (CAN1->MSR & CAN_MSR_INAK);          // Wait until initialization mode is exited
}

// === Send CAN frame with standard ID ===
void CAN_Send_STD(uint16_t std_id, uint8_t *data, uint8_t len) {
    while ((CAN1->TSR & CAN_TSR_TME0) == 0);   // Wait until TX mailbox 0 is empty

    CAN1->sTxMailBox[0].TIR = (std_id << 21);  // Set standard ID in TIR (bits 21–31)
    CAN1->sTxMailBox[0].TDTR = len & 0xF;      // Set data length (0–8 bytes)

    // Write data to lower and higher data registers
    CAN1->sTxMailBox[0].TDLR = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
    CAN1->sTxMailBox[0].TDHR = data[4] | (data[5] << 8) | (data[6] << 16) | (data[7] << 24);

    CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;  // Request transmission
}

// === Send CAN frame with extended ID ===
void CAN_Send_EXT(uint32_t ext_id, uint8_t *data, uint8_t len) {
    while ((CAN1->TSR & CAN_TSR_TME0) == 0);   // Wait until TX mailbox 0 is empty

    // Set extended ID and IDE bit (bit 2 = 1 for extended)
    CAN1->sTxMailBox[0].TIR = (ext_id << 3) | (1 << 2);
    CAN1->sTxMailBox[0].TDTR = len & 0xF;      // Set data length

    // Load data into data registers
    CAN1->sTxMailBox[0].TDLR = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
    CAN1->sTxMailBox[0].TDHR = data[4] | (data[5] << 8) | (data[6] << 16) | (data[7] << 24);

    CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;  // Request transmission
}

// === Receive CAN message (polling method) ===
uint8_t CAN_Receive(uint8_t *data_out, uint32_t *id_out, uint8_t *is_extended) {
    if ((CAN1->RF0R & 0x03) == 0) return 0;  // No message pending in FIFO 0

    uint32_t rir = CAN1->sFIFOMailBox[0].RIR;  // Read identifier register

    // Check IDE bit to determine ID type
    if (rir & (1 << 2)) {
        *id_out = (rir >> 3) & 0x1FFFFFFF;   // Extract extended ID
        *is_extended = 1;
    } else {
        *id_out = (rir >> 21) & 0x7FF;       // Extract standard ID
        *is_extended = 0;
    }

    uint8_t len = CAN1->sFIFOMailBox[0].RDTR & 0xF;  // Get data length
    uint32_t dlr = CAN1->sFIFOMailBox[0].RDLR;       // Read lower data
    uint32_t dhr = CAN1->sFIFOMailBox[0].RDHR;       // Read higher data

    // Copy up to 8 bytes of data to output buffer
    for (int i = 0; i < 4 && i < len; i++) data_out[i] = (dlr >> (8 * i)) & 0xFF;
    for (int i = 4; i < 8 && i < len; i++) data_out[i] = (dhr >> (8 * (i - 4))) & 0xFF;

    CAN1->RF0R |= CAN_RF0R_RFOM0;  // Release FIFO 0 output mailbox
    return len;                    // Return number of received bytes
}

// === CAN1 RX0 interrupt handler ===
void CAN1_RX0_IRQHandler(void) {
    if ((CAN1->RF0R & 0x03) == 0) return;  // No message pending

    uint32_t rir = CAN1->sFIFOMailBox[0].RIR;  // Read identifier register
    is_ext = (rir & (1 << 2)) ? 1 : 0;         // Determine if extended ID
    rx_id = is_ext ? ((rir >> 3) & 0x1FFFFFFF) // Extract extended ID
                   : ((rir >> 21) & 0x7FF);    // Extract standard ID

    rx_len = CAN1->sFIFOMailBox[0].RDTR & 0xF; // Read data length
    uint32_t dlr = CAN1->sFIFOMailBox[0].RDLR; // Lower data
    uint32_t dhr = CAN1->sFIFOMailBox[0].RDHR; // Higher data

    // Copy received data into rx_data buffer
    for (int i = 0; i < 4 && i < rx_len; i++) rx_data[i] = (dlr >> (8 * i)) & 0xFF;
    for (int i = 4; i < 8 && i < rx_len; i++) rx_data[i] = (dhr >> (8 * (i - 4))) & 0xFF;

    CAN1->RF0R |= CAN_RF0R_RFOM0;   // Release FIFO 0 output mailbox

    can_rx_flag = 1;  // Set flag to indicate a message has been received
}

/*
 * End of file
 */
