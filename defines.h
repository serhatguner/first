#ifndef __DEFINES_H
#define __DEFINES_H



#include "stdint.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#define SysTickFreq 100

#define ExeptionReset HardFault_Handler 

#define RF_USART_GPIO_Port GPIOA
#define MeterUSART_GPIO_Port GPIOA

#define RF_RX_Pin GPIO_Pin_2
#define RF_TX_Pin GPIO_Pin_3
#define MeterRX_Pin GPIO_Pin_10
#define MeterTX_Pin GPIO_Pin_9


#define RF_SetPort GPIOA
#define RF_SetBit  GPIO_Pin_1
#define RF_CS_Bit  GPIO_Pin_0

#define RS485_ControlPort GPIOA
#define RS485_ControlPin GPIO_Pin_5
#define Meter_IRQn USART1_IRQn
#define RF_IRQn USART2_IRQn

#define AF_CONFIG do{	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_1); \
											GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1); \
											GPIO_PinAFConfig(GPIOA, GPIO_PinSource2,  GPIO_AF_1); \
											GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);}	while(0)

#define RCC_GPIO do {RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA , ENABLE);}while(0)

#define RCC_USART_Meter RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE)
#define	RCC_USARRT_RF RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE)


#define RF_UART_Channel USART2
#define Meter_UART_Channel USART1
#define Meter_Interrupt USART1_IRQHandler
#define RF_Interrupt USART2_IRQHandler

typedef int bool;
#define true 1
#define false 0
#endif
