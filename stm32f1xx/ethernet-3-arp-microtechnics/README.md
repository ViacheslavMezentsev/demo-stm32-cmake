# STM32 and Ethernet, part 3. ARP.

<img src="https://microtechnics.ru/wp-content/uploads/2021/10/osi-468x400.jpg" width="250">

Demo project with basic ARP protocol implementation for STM32 and ENC28J60 as ethernet physical layer provider. Connection diagram for BluePill development board:

<img src="https://microtechnics.ru/wp-content/uploads/2021/08/enc28j60-connection-768x419.jpg" width="400">

List of required hardware:
- BluePill (STM32F103C8T6)
- ENC28J60

Peripherals in use:
- SPI1 for communication
- some additional GPIOs in output mode to control ENC28J60

The full description, if necessary, is [here](https://microtechnics.ru/stm32-i-ethernet-chast-3-kanalnyj-uroven-protokol-arp/).
