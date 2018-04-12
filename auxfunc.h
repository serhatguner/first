#ifndef __AUXFUNC_H
#define __AUXFUNC_H

#include "defines.h"

extern 	uint8_t RF_RxBuffer[100];
extern uint8_t Meter_RxBuffer[100];
extern uint8_t RF_RxCounter ;
extern uint8_t MeterRxCounter;
extern bool newRF_DataFlag ;
extern bool newMeterDataFlag ;
extern int rs485_baud_rate_mapping [];
extern USART_InitTypeDef USART_InitStruct;


void delay(int i);
void RCC_Configuration(void);
void GPIO_Configuration(void);
void USART_Configuration(void);
void NVIC_Configuration(void);
void RS485_ReadEnable(void);
void RS485_WriteEnable(void);
void dataBitConvert(uint8_t  data[30],uint8_t length);
void UART_SendStringb(USART_TypeDef* USARTx, uint8_t * data);
void SysTick_Configuration(void);
void WriteToRF(void);
void UART_SendString(USART_TypeDef* USARTx, uint8_t * data, uint8_t length);
void WriteToMeter(void);
void baud300(void);
void baudChange(void);
static void WWDG_Config(void);

#endif
