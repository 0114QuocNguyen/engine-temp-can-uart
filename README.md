# uart-can-bridge
Simulates engine temperature packets sent from PC via UART, forwarded over CAN, and returned to PC.
# UART-CAN Bridge on STM32

This project implements a bidirectional communication system using two STM32F103C8T6 boards. It forwards simulated packets from UART → CAN → UART, supporting both cyclic and manual transmission modes.

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
