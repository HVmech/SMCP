.syntax unified
.cpu cortex-m3
.fpu softvfp
.thumb

.section .isr_vector,"a",%progbits
.global g_pfnVectors

g_pfnVectors:
    .word _stack                    /* Initial stack pointer */
    .word Reset_Handler
    .word NMI_Handler
    .word HardFault_Handler
    .word MemManage_Handler
    .word BusFault_Handler
    .word UsageFault_Handler
    .word 0                         /* Reserved */
    .word 0                         /* Reserved */
    .word 0                         /* Reserved */
    .word 0                         /* Reserved */
    .word SVCall_Handler
    .word DebugMonitor_Handler
    .word 0                         /* Reserved */
    .word PendSV_Handler
    .word SysTick_Handler

    /* External Interrupts */
    .word WWDG_Handler
    .word PVD_Handler
    .word TAMPER_Handler
    .word RTC_Handler
    .word FLASH_Handler
    .word RCC_Handler
    .word EXTI0_Handler
    .word EXTI1_Handler
    .word EXTI2_Handler
    .word EXTI3_Handler
    .word EXTI4_Handler
    .word DMA1_Channel1_Handler
    .word DMA1_Channel2_Handler
    .word DMA1_Channel3_Handler
    .word DMA1_Channel4_Handler
    .word DMA1_Channel5_Handler
    .word DMA1_Channel6_Handler
    .word DMA1_Channel7_Handler
    .word ADC1_2_Handler
    .word USB_HP_CAN_TX_Handler
    .word USB_LP_CAN_RX0_Handler
    .word CAN_RX1_Handler
    .word CAN_SCE_Handler
    .word EXTI9_5_Handler
    .word TIM1_BRK_Handler
    .word TIM1_UP_Handler
    .word TIM1_TRG_COM_Handler
    .word TIM1_CC_Handler
    .word TIM2_Handler
    .word TIM3_Handler
    .word TIM4_Handler
    .word I2C1_EV_Handler
    .word I2C1_ER_Handler
    .word I2C2_EV_Handler
    .word I2C2_ER_Handler
    .word SPI1_Handler
    .word SPI2_Handler
    .word USART1_Handler
    .word USART2_Handler
    .word USART3_Handler
    .word EXTI15_10_Handler
    .word RTCAlarm_Handler
    .word USBWakeup_Handler
    .word TIM8_BRK_Handler
    .word TIM8_UP_Handler
    .word TIM8_TRG_COM_Handler
    .word TIM8_CC_Handler
    .word ADC3_Handler
    .word FSMC_Handler
    .word SDIO_Handler
    .word TIM5_Handler
    .word SPI3_Handler
    .word UART4_Handler
    .word UART5_Handler
    .word TIM6_Handler
    .word TIM7_Handler
    .word DMA2_Channel1_Handler
    .word DMA2_Channel2_Handler
    .word DMA2_Channel3_Handler
    .word DMA2_Channel4_5_Handler

.macro IRQ_HANDLER name
    .weak \name
    .set \name, Default_Handler
.endm

IRQ_HANDLER NMI_Handler
IRQ_HANDLER HardFault_Handler
IRQ_HANDLER MemManage_Handler
IRQ_HANDLER BusFault_Handler
IRQ_HANDLER UsageFault_Handler
IRQ_HANDLER SVCall_Handler
IRQ_HANDLER DebugMonitor_Handler
IRQ_HANDLER PendSV_Handler
IRQ_HANDLER SysTick_Handler
IRQ_HANDLER WWDG_Handler
IRQ_HANDLER PVD_Handler
IRQ_HANDLER TAMPER_Handler
IRQ_HANDLER RTC_Handler
IRQ_HANDLER FLASH_Handler
IRQ_HANDLER RCC_Handler
IRQ_HANDLER EXTI0_Handler
IRQ_HANDLER EXTI1_Handler
IRQ_HANDLER EXTI2_Handler
IRQ_HANDLER EXTI3_Handler
IRQ_HANDLER EXTI4_Handler
IRQ_HANDLER DMA1_Channel1_Handler
IRQ_HANDLER DMA1_Channel2_Handler
IRQ_HANDLER DMA1_Channel3_Handler
IRQ_HANDLER DMA1_Channel4_Handler
IRQ_HANDLER DMA1_Channel5_Handler
IRQ_HANDLER DMA1_Channel6_Handler
IRQ_HANDLER DMA1_Channel7_Handler
IRQ_HANDLER ADC1_2_Handler
IRQ_HANDLER USB_HP_CAN_TX_Handler
IRQ_HANDLER USB_LP_CAN_RX0_Handler
IRQ_HANDLER CAN_RX1_Handler
IRQ_HANDLER CAN_SCE_Handler
IRQ_HANDLER EXTI9_5_Handler
IRQ_HANDLER TIM1_BRK_Handler
IRQ_HANDLER TIM1_UP_Handler
IRQ_HANDLER TIM1_TRG_COM_Handler
IRQ_HANDLER TIM1_CC_Handler
IRQ_HANDLER TIM2_Handler
IRQ_HANDLER TIM3_Handler
IRQ_HANDLER TIM4_Handler
IRQ_HANDLER I2C1_EV_Handler
IRQ_HANDLER I2C1_ER_Handler
IRQ_HANDLER I2C2_EV_Handler
IRQ_HANDLER I2C2_ER_Handler
IRQ_HANDLER SPI1_Handler
IRQ_HANDLER SPI2_Handler
IRQ_HANDLER USART1_Handler
IRQ_HANDLER USART2_Handler
IRQ_HANDLER USART3_Handler
IRQ_HANDLER EXTI15_10_Handler
IRQ_HANDLER RTCAlarm_Handler
IRQ_HANDLER USBWakeup_Handler
IRQ_HANDLER TIM8_BRK_Handler
IRQ_HANDLER TIM8_UP_Handler
IRQ_HANDLER TIM8_TRG_COM_Handler
IRQ_HANDLER TIM8_CC_Handler
IRQ_HANDLER ADC3_Handler
IRQ_HANDLER FSMC_Handler
IRQ_HANDLER SDIO_Handler
IRQ_HANDLER TIM5_Handler
IRQ_HANDLER SPI3_Handler
IRQ_HANDLER UART4_Handler
IRQ_HANDLER UART5_Handler
IRQ_HANDLER TIM6_Handler
IRQ_HANDLER TIM7_Handler
IRQ_HANDLER DMA2_Channel1_Handler
IRQ_HANDLER DMA2_Channel2_Handler
IRQ_HANDLER DMA2_Channel3_Handler
IRQ_HANDLER DMA2_Channel4_5_Handler

.section .text.Default_Handler
.type Default_Handler, %function
Default_Handler:
    b Default_Handler

.section .text.Reset_Handler
.global Reset_Handler
.type Reset_Handler, %function
.thumb_func
Reset_Handler:
    ldr r0, =_stack
    mov sp, r0

    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_etext
    cmp r0, r1
    beq .skip_data
.copy_loop:
    ldr r3, [r2], #4
    str r3, [r0], #4
    cmp r0, r1
    bne .copy_loop
.skip_data:

    ldr r0, =_sbss
    ldr r1, =_ebss
    cmp r0, r1
    beq .skip_bss
.bss_loop:
    mov r2, #0
    str r2, [r0], #4
    cmp r0, r1
    bne .bss_loop
.skip_bss:
    bl main
    b .
