/* File: startup_ARMCM3.S
 * Purpose: startup file for Cortex-M3 devices. Should use with
 *   GCC for ARM Embedded Processors
 * Version: V1.3
 * Date: 08 Feb 2012
 *
 * Copyright (c) 2012, ARM Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the ARM Limited nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ARM LIMITED BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES//
 * LOSS OF USE, DATA, OR PROFITS// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
    .syntax unified
    .arch armv7-m

    .section .stack
    .align 3
#ifdef __STACK_SIZE
    .equ    Stack_Size, __STACK_SIZE
#else
    .equ    Stack_Size, 0x400
#endif
    .globl    __StackTop
    .globl    __StackLimit
__StackLimit:
    .space    Stack_Size
    .size __StackLimit, . - __StackLimit
__StackTop:
    .size __StackTop, . - __StackTop

    .section .heap
    .align 3
#ifdef __HEAP_SIZE
    .equ    Heap_Size, __HEAP_SIZE
#else
    .equ    Heap_Size, 0xC00
#endif
    .globl    __HeapBase
    .globl    __HeapLimit
__HeapBase:
    .if    Heap_Size
    .space    Heap_Size
    .endif
    .size __HeapBase, . - __HeapBase
__HeapLimit:
    .size __HeapLimit, . - __HeapLimit

    .section .isr_vector
    .align 2
    .globl __isr_vector
__isr_vector:
    .long    __StackTop            /* Top of Stack */
    .long    Reset_Handler         /* Reset Handler */
    .long    NMI_Handler           /* NMI Handler */
    .long    HardFault_Handler     /* Hard Fault Handler */
    .long    MemManage_Handler     /* MPU Fault Handler */
    .long    BusFault_Handler      /* Bus Fault Handler */
    .long    UsageFault_Handler    /* Usage Fault Handler */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    SVC_Handler           /* SVCall Handler */
    .long    DebugMon_Handler      /* Debug Monitor Handler */
    .long    0                     /* Reserved */
    .long    PendSV_Handler        /* PendSV Handler */
    .long    SysTick_Handler       /* SysTick Handler */

                // External Interrupts
    .long    WWDG_IRQHandler            // Window Watchdog
    .long    PVD_IRQHandler             // PVD through EXTI Line detect
    .long    TAMPER_IRQHandler          // Tamper
    .long    RTC_IRQHandler             // RTC
    .long    FLASH_IRQHandler           // Flash
    .long    RCC_IRQHandler             // RCC
    .long    EXTI0_IRQHandler           // EXTI Line 0
    .long    EXTI1_IRQHandler           // EXTI Line 1
    .long    EXTI2_IRQHandler           // EXTI Line 2
    .long    EXTI3_IRQHandler           // EXTI Line 3
    .long    EXTI4_IRQHandler           // EXTI Line 4
    .long    DMA1_Channel1_IRQHandler   // DMA1 Channel 1
    .long    DMA1_Channel2_IRQHandler   // DMA1 Channel 2
    .long    DMA1_Channel3_IRQHandler   // DMA1 Channel 3
    .long    DMA1_Channel4_IRQHandler   // DMA1 Channel 4
    .long    DMA1_Channel5_IRQHandler   // DMA1 Channel 5
    .long    DMA1_Channel6_IRQHandler   // DMA1 Channel 6
    .long    DMA1_Channel7_IRQHandler   // DMA1 Channel 7
    .long    ADC1_2_IRQHandler          // ADC1_2
    .long    USB_HP_CAN1_TX_IRQHandler  // USB High Priority or CAN1 TX
    .long    USB_LP_CAN1_RX0_IRQHandler // USB Low  Priority or CAN1 RX0
    .long    CAN1_RX1_IRQHandler        // CAN1 RX1
    .long    CAN1_SCE_IRQHandler        // CAN1 SCE
    .long    EXTI9_5_IRQHandler         // EXTI Line 9..5
    .long    TIM1_BRK_IRQHandler        // TIM1 Break
    .long    TIM1_UP_IRQHandler         // TIM1 Update
    .long    TIM1_TRG_COM_IRQHandler    // TIM1 Trigger and Commutation
    .long    TIM1_CC_IRQHandler         // TIM1 Capture Compare
    .long    TIM2_IRQHandler            // TIM2
    .long    TIM3_IRQHandler            // TIM3
    .long    0                          // Reserved
    .long    I2C1_EV_IRQHandler         // I2C1 Event
    .long    I2C1_ER_IRQHandler         // I2C1 Error
    .long    0                          // Reserved
    .long    0                          // Reserved
    .long    SPI1_IRQHandler            // SPI1
    .long    0                          // Reserved
    .long    USART1_IRQHandler          // USART1
    .long    USART2_IRQHandler          // USART2
    .long    0                          // Reserved
    .long    EXTI15_10_IRQHandler       // EXTI Line 15..10
    .long    RTCAlarm_IRQHandler        // RTC Alarm through EXTI Line
    .long    USBWakeUp_IRQHandler       // USB Wakeup from suspend

    .size    __isr_vector, . - __isr_vector

    .text
    .thumb
    .thumb_func
    .align 2
    .globl    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
/*     Loop to copy data from read only memory to RAM. The ranges
 *      of copy from/to are specified by following symbols evaluated in
 *      linker script.
 *      __etext: End of code section, i.e., begin of data sections to copy from.
 *      __data_start__/__data_end__: RAM address range that data should be
 *      copied to. Both must be aligned to 4 bytes boundary.  */

    ldr    r1, =__etext
    ldr    r2, =__data_start__
    ldr    r3, =__data_end__

#if 1
/* Here are two copies of loop implemenations. First one favors code size
 * and the second one favors performance. Default uses the first one.
 * Change to "#if 0" to use the second one */
.flash_to_ram_loop:
    cmp     r2, r3
    ittt    lt
    ldrlt   r0, [r1], #4
    strlt   r0, [r2], #4
    blt    .flash_to_ram_loop
#else
    subs    r3, r2
    ble    .flash_to_ram_loop_end
.flash_to_ram_loop:
    subs    r3, #4
    ldr    r0, [r1, r3]
    str    r0, [r2, r3]
    bgt    .flash_to_ram_loop
.flash_to_ram_loop_end:
#endif

#ifndef __NO_SYSTEM_INIT
    ldr    r0, =SystemInit
    blx    r0
#endif

    ldr    r0, =_start
    bx    r0
    .pool
    .size Reset_Handler, . - Reset_Handler

/* Our weak _start alternative if we don't use the library _start
 * The zero init section must be cleared, otherwise the librtary is
 * doing that */
    .align 1
    .thumb_func
    .weak _start
    .type _start, %function
_start:

    /* Zero fill the bss segment. */
    ldr   r1, = __bss_start__
    ldr   r2, = __bss_end__
    movs  r3, #0
    b  .fill_zero_bss
.loop_zero_bss:
    str  r3, [r1], #4

.fill_zero_bss:
    cmp  r1, r2
    bcc  .loop_zero_bss

    /* Jump to our main */
    bl main
    b .
    .size    _start, . - _start

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro    def_irq_handler    handler_name
    .align 1
    .thumb_func
    .weak    \handler_name
    .type    \handler_name, %function
\handler_name :
    b    .
    .size    \handler_name, . - \handler_name
    .endm

    def_irq_handler    NMI_Handler
    def_irq_handler    HardFault_Handler
    def_irq_handler    MemManage_Handler
    def_irq_handler    BusFault_Handler
    def_irq_handler    UsageFault_Handler
    def_irq_handler    SVC_Handler
    def_irq_handler    DebugMon_Handler
    def_irq_handler    PendSV_Handler
    def_irq_handler    SysTick_Handler
    def_irq_handler    Default_Handler

                // External Interrupts
    def_irq_handler    WWDG_IRQHandler            // Window Watchdog
    def_irq_handler    PVD_IRQHandler             // PVD through EXTI Line detect
    def_irq_handler    TAMPER_IRQHandler          // Tamper
    def_irq_handler    RTC_IRQHandler             // RTC
    def_irq_handler    FLASH_IRQHandler           // Flash
    def_irq_handler    RCC_IRQHandler             // RCC
    def_irq_handler    EXTI0_IRQHandler           // EXTI Line 0
    def_irq_handler    EXTI1_IRQHandler           // EXTI Line 1
    def_irq_handler    EXTI2_IRQHandler           // EXTI Line 2
    def_irq_handler    EXTI3_IRQHandler           // EXTI Line 3
    def_irq_handler    EXTI4_IRQHandler           // EXTI Line 4
    def_irq_handler    DMA1_Channel1_IRQHandler   // DMA1 Channel 1
    def_irq_handler    DMA1_Channel2_IRQHandler   // DMA1 Channel 2
    def_irq_handler    DMA1_Channel3_IRQHandler   // DMA1 Channel 3
    def_irq_handler    DMA1_Channel4_IRQHandler   // DMA1 Channel 4
    def_irq_handler    DMA1_Channel5_IRQHandler   // DMA1 Channel 5
    def_irq_handler    DMA1_Channel6_IRQHandler   // DMA1 Channel 6
    def_irq_handler    DMA1_Channel7_IRQHandler   // DMA1 Channel 7
    def_irq_handler    ADC1_2_IRQHandler          // ADC1_2
    def_irq_handler    USB_HP_CAN1_TX_IRQHandler  // USB High Priority or CAN1 TX
    def_irq_handler    USB_LP_CAN1_RX0_IRQHandler // USB Low  Priority or CAN1 RX0
    def_irq_handler    CAN1_RX1_IRQHandler        // CAN1 RX1
    def_irq_handler    CAN1_SCE_IRQHandler        // CAN1 SCE
    def_irq_handler    EXTI9_5_IRQHandler         // EXTI Line 9..5
    def_irq_handler    TIM1_BRK_IRQHandler        // TIM1 Break
    def_irq_handler    TIM1_UP_IRQHandler         // TIM1 Update
    def_irq_handler    TIM1_TRG_COM_IRQHandler    // TIM1 Trigger and Commutation
    def_irq_handler    TIM1_CC_IRQHandler         // TIM1 Capture Compare
    def_irq_handler    TIM2_IRQHandler            // TIM2
    def_irq_handler    TIM3_IRQHandler            // TIM3
    def_irq_handler    I2C1_EV_IRQHandler         // I2C1 Event
    def_irq_handler    I2C1_ER_IRQHandler         // I2C1 Error
    def_irq_handler    SPI1_IRQHandler            // SPI1
    def_irq_handler    USART1_IRQHandler          // USART1
    def_irq_handler    USART2_IRQHandler          // USART2
    def_irq_handler    EXTI15_10_IRQHandler       // EXTI Line 15..10
    def_irq_handler    RTCAlarm_IRQHandler        // RTC Alarm through EXTI Line
    def_irq_handler    USBWakeUp_IRQHandler       // USB Wakeup from suspend

    .end
