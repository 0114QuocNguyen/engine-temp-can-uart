# uart-can-bridge
Simulates engine temperature packets sent from PC via UART, forwarded over CAN, and returned to PC.
## Features
- Receive UART frames from PC
- Forward to another STM32 over CAN
- Send received CAN frame to PC via UART
- Supports cyclic transmission with adjustable interval

## Hardware
- STM32F103C8T6 x2
- MCP2551 CAN transceivers
- USB-UART modules for PC connection

## Protocol
UART frame: [Model][ID][Length][Data][Cyclic (ms)]
