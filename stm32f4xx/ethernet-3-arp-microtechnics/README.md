# STM32 and Ethernet, part 1. ENC28J60 driver.

Bunch of projects for STM32 Ethernet connection based on ENC28J60 IC physical layer. The first part is devoted to creating ENC28J60 SPI driver and its configuration and setting up. Here is a basic connection diagram:

<img src="https://microtechnics.ru/wp-content/uploads/2021/08/enc28j60-connection.jpg" width="400">

Demo project uses SPI1 peripheral of STM32F103C8 and well-known development board BluePill. List of used peripherals includes SPI lines and some GPIOs in output mode:

<img src="https://microtechnics.ru/wp-content/uploads/2021/08/stm32cubemx.jpg" width="400">

If you need a full description in addition to the sources, it can be found on [my site](https://microtechnics.ru/stm32-i-ethernet-chast-1-podklyuchenie-i-nastrojka-enc28j60/).
