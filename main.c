
#include "stm32f0xx_usart.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_dma.h"
#include <stdio.h>
#include "defines.h"
#include "auxfunc.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{

	//Enable High Speed External Oscillator
	RCC_HSEConfig (RCC_HSE_ON);
	do{
		RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
	}
	while(RCC_WaitForHSEStartUp() == ERROR);
	
	
	//Reset device settings
	RCC_DeInit();
	USART_DeInit(RF_UART_Channel);
	USART_DeInit(Meter_UART_Channel);
	GPIO_DeInit(GPIOA);	
	RF_RxCounter = 0;
	MeterRxCounter = 0;
	RF_RxBuffer[RF_RxCounter] = 0;
	Meter_RxBuffer[MeterRxCounter] = 0;

	
	//Enable periphheral clocks
	RCC_Configuration();
	
	GPIO_Configuration();

	//set sv611 to configuration mode
  GPIO_SetBits(RS485_ControlPort,RF_CS_Bit);
	GPIO_SetBits(RS485_ControlPort,RF_SetBit);
	//write config code here maybe
	
	
	USART_Configuration();

	//Clear Flags
	if(USART_GetFlagStatus(RF_UART_Channel, USART_FLAG_PE|USART_FLAG_FE|USART_FLAG_ORE) != RESET)
		USART_ClearFlag(RF_UART_Channel, USART_FLAG_ORE|USART_FLAG_FE|USART_FLAG_PE);
	if(USART_GetFlagStatus(RF_UART_Channel,USART_FLAG_NE ) != RESET)
		USART_ClearFlag(RF_UART_Channel,USART_FLAG_NE );

	//Enable Nested vector interrupt controller
	NVIC_Configuration();

	//start systick timer
	SysTick_Configuration();

	//set sv611 to tranceiver mode
  GPIO_SetBits(RS485_ControlPort,RF_CS_Bit);
	GPIO_SetBits(RS485_ControlPort,RF_SetBit);

	//Set Usart' interrupts
	USART_ITConfig(RF_UART_Channel, USART_IT_RXNE , ENABLE);
	USART_ITConfig(RF_UART_Channel, USART_IT_ORE, ENABLE);
	USART_ITConfig(RF_UART_Channel, USART_IT_PE, ENABLE);
	USART_ITConfig(RF_UART_Channel, USART_IT_NE , ENABLE);
	USART_ITConfig(Meter_UART_Channel, USART_IT_ORE, ENABLE);
	USART_ITConfig(Meter_UART_Channel, USART_IT_PE, ENABLE);
	USART_ITConfig(Meter_UART_Channel, USART_IT_RXNE, ENABLE);
	
	//set 485 ic to read 
	RS485_ReadEnable();

	//Enable Usart peripherals
	USART_Cmd(RF_UART_Channel,ENABLE);
	USART_Cmd(Meter_UART_Channel,ENABLE);

	
	while(1){
	}
}

