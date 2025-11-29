.syntax unified
.cpu cortex-m3
.fpu softvfp
.thumb

.section .isr_vector,"a",%progbits
.global g_pfnVectors

g_pfnVectors:
    .word _estack
    .word Reset_Handler
    .word NMI_Handler
    .word HardFault_Handler
    .word MemManage_Handler
    .word BusFault_Handler
    .word UsageFault_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVCall_Handler
    .word DebugMonitor_Handler
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler
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

.equ _estack, 0x20005000

.weak NMI_Handler
.weak HardFault_Handler
.weak MemManage_Handler
.weak BusFault_Handler
.weak UsageFault_Handler
.weak SVCall_Handler
.weak DebugMonitor_Handler
.weak PendSV_Handler
.weak SysTick_Handler
.weak WWDG_Handler
.weak PVD_Handler
.weak TAMPER_Handler
.weak RTC_Handler
.weak FLASH_Handler
.weak RCC_Handler
.weak EXTI0_Handler
.weak EXTI1_Handler
.weak EXTI2_Handler
.weak EXTI3_Handler
.weak EXTI4_Handler
.weak DMA1_Channel1_Handler
.weak DMA1_Channel2_Handler
.weak DMA1_Channel3_Handler
.weak DMA1_Channel4_Handler
.weak DMA1_Channel5_Handler
.weak DMA1_Channel6_Handler
.weak DMA1_Channel7_Handler
.weak ADC1_2_Handler
.weak USB_HP_CAN_TX_Handler
.weak USB_LP_CAN_RX0_Handler
.weak CAN_RX1_Handler
.weak CAN_SCE_Handler
.weak EXTI9_5_Handler
.weak TIM1_BRK_Handler
.weak TIM1_UP_Handler
.weak TIM1_TRG_COM_Handler
.weak TIM1_CC_Handler
.weak TIM2_Handler
.weak TIM3_Handler
.weak TIM4_Handler
.weak I2C1_EV_Handler
.weak I2C1_ER_Handler
.weak I2C2_EV_Handler
.weak I2C2_ER_Handler
.weak SPI1_Handler
.weak SPI2_Handler
.weak USART1_Handler
.weak USART2_Handler
.weak USART3_Handler
.weak EXTI15_10_Handler
.weak RTCAlarm_Handler
.weak USBWakeup_Handler
.weak TIM8_BRK_Handler
.weak TIM8_UP_Handler
.weak TIM8_TRG_COM_Handler
.weak TIM8_CC_Handler
.weak ADC3_Handler
.weak FSMC_Handler
.weak SDIO_Handler
.weak TIM5_Handler
.weak SPI3_Handler
.weak UART4_Handler
.weak UART5_Handler
.weak TIM6_Handler
.weak TIM7_Handler
.weak DMA2_Channel1_Handler
.weak DMA2_Channel2_Handler
.weak DMA2_Channel3_Handler
.weak DMA2_Channel4_5_Handler

.thumb_set NMI_Handler,Default_Handler
.thumb_set HardFault_Handler,Default_Handler
.thumb_set MemManage_Handler,Default_Handler
.thumb_set BusFault_Handler,Default_Handler
.thumb_set UsageFault_Handler,Default_Handler
.thumb_set SVCall_Handler,Default_Handler
.thumb_set DebugMonitor_Handler,Default_Handler
.thumb_set PendSV_Handler,Default_Handler
.thumb_set SysTick_Handler,Default_Handler
.thumb_set WWDG_Handler,Default_Handler
.thumb_set PVD_Handler,Default_Handler
.thumb_set TAMPER_Handler,Default_Handler
.thumb_set RTC_Handler,Default_Handler
.thumb_set FLASH_Handler,Default_Handler
.thumb_set RCC_Handler,Default_Handler
.thumb_set EXTI0_Handler,Default_Handler
.thumb_set EXTI1_Handler,Default_Handler
.thumb_set EXTI2_Handler,Default_Handler
.thumb_set EXTI3_Handler,Default_Handler
.thumb_set EXTI4_Handler,Default_Handler
.thumb_set DMA1_Channel1_Handler,Default_Handler
.thumb_set DMA1_Channel2_Handler,Default_Handler
.thumb_set DMA1_Channel3_Handler,Default_Handler
.thumb_set DMA1_Channel4_Handler,Default_Handler
.thumb_set DMA1_Channel5_Handler,Default_Handler
.thumb_set DMA1_Channel6_Handler,Default_Handler
.thumb_set DMA1_Channel7_Handler,Default_Handler
.thumb_set ADC1_2_Handler,Default_Handler
.thumb_set USB_HP_CAN_TX_Handler,Default_Handler
.thumb_set USB_LP_CAN_RX0_Handler,Default_Handler
.thumb_set CAN_RX1_Handler,Default_Handler
.thumb_set CAN_SCE_Handler,Default_Handler
.thumb_set EXTI9_5_Handler,Default_Handler
.thumb_set TIM1_BRK_Handler,Default_Handler
.thumb_set TIM1_UP_Handler,Default_Handler
.thumb_set TIM1_TRG_COM_Handler,Default_Handler
.thumb_set TIM1_CC_Handler,Default_Handler
.thumb_set TIM2_Handler,Default_Handler
.thumb_set TIM3_Handler,Default_Handler
.thumb_set TIM4_Handler,Default_Handler
.thumb_set I2C1_EV_Handler,Default_Handler
.thumb_set I2C1_ER_Handler,Default_Handler
.thumb_set I2C2_EV_Handler,Default_Handler
.thumb_set I2C2_ER_Handler,Default_Handler
.thumb_set SPI1_Handler,Default_Handler
.thumb_set SPI2_Handler,Default_Handler
.thumb_set USART1_Handler,Default_Handler
.thumb_set USART2_Handler,Default_Handler
.thumb_set USART3_Handler,Default_Handler
.thumb_set EXTI15_10_Handler,Default_Handler
.thumb_set RTCAlarm_Handler,Default_Handler
.thumb_set USBWakeup_Handler,Default_Handler
.thumb_set TIM8_BRK_Handler,Default_Handler
.thumb_set TIM8_UP_Handler,Default_Handler
.thumb_set TIM8_TRG_COM_Handler,Default_Handler
.thumb_set TIM8_CC_Handler,Default_Handler
.thumb_set ADC3_Handler,Default_Handler
.thumb_set FSMC_Handler,Default_Handler
.thumb_set SDIO_Handler,Default_Handler
.thumb_set TIM5_Handler,Default_Handler
.thumb_set SPI3_Handler,Default_Handler
.thumb_set UART4_Handler,Default_Handler
.thumb_set UART5_Handler,Default_Handler
.thumb_set TIM6_Handler,Default_Handler
.thumb_set TIM7_Handler,Default_Handler
.thumb_set DMA2_Channel1_Handler,Default_Handler
.thumb_set DMA2_Channel2_Handler,Default_Handler
.thumb_set DMA2_Channel3_Handler,Default_Handler
.thumb_set DMA2_Channel4_5_Handler,Default_Handler

.section .text.Reset_Handler
.weak Reset_Handler
.weak SystemInit
.type Reset_Handler, %function
Reset_Handler:
    ldr r0, =_estack
    mov sp, r0

    ldr r0, =_sdata
    ldr r1, =_edata 
    ldr r2, =_sidata
    bl data_copy

    ldr r0, =_sbss
    ldr r1, =_ebss
    bl bss_zero

    bl SystemInit
    bl main
    b .  /* Зависание после main */

.section .text.data_copy
.type data_copy, %function
data_copy:
    cmp r0, r1
    itt lt
    ldrlt r3, [r2], #4
    strlt r3, [r0], #4
    blt data_copy
    bx lr

.section .text.bss_zero  
.type bss_zero, %function
bss_zero:
    mov r2, #0
    cmp r0, r1
    it lt
    strlt r2, [r0], #4
    blt bss_zero
    bx lr

.section .text.Default_Handler
.type Default_Handler, %function
Default_Handler:
    b Default_Handler

.section .text.SystemInit
.thumb_func
SystemInit:
    bx lr
