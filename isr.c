#include "defines.h"
#include "stdint.h" 
#include "stm32f0xx_gpio.h"
#include "auxfunc.h"




/*****************************************************************************************/

void RF_Interrupt(){

	MeterRxCounter = 0;

	/*Read incoming byte*/
	if(USART_GetITStatus(RF_UART_Channel, USART_IT_RXNE) != RESET)
	{
		/* Read one byte from the receive data register */
		//detect2f;
		uint8_t temp;
		temp = (uint8_t)USART_ReceiveData(RF_UART_Channel) & 0x7f;
		if(temp == 0x2f){
			
			RF_RxCounter = 0;
			MeterRxCounter = 0;
			newRF_DataFlag = false;
			newMeterDataFlag = false;
			
		}
		RF_RxBuffer[RF_RxCounter] = (uint8_t)USART_ReceiveData(RF_UART_Channel) & 0x7f;
		RF_RxCounter++;
		RF_RxBuffer[RF_RxCounter] = 0x00;
		newRF_DataFlag = true;

	}
	//if nothing to read then clear buffer
	else{
		RF_RxCounter = 0;
		RF_RxBuffer[RF_RxCounter] = 0x00;
		
		if(USART_GetFlagStatus(RF_UART_Channel, USART_FLAG_PE|USART_FLAG_FE|USART_FLAG_ORE) != RESET)
			USART_ClearFlag(RF_UART_Channel, USART_FLAG_ORE|USART_FLAG_FE|USART_FLAG_PE);
		if(USART_GetFlagStatus(RF_UART_Channel,USART_FLAG_NE ) != RESET)
			USART_ClearFlag(RF_UART_Channel,USART_FLAG_NE );
	}
	newMeterDataFlag = false;
}

void Meter_Interrupt(){
	
	RF_RxCounter = 0;
	RF_RxBuffer[RF_RxCounter] = 0;
	
	if(USART_GetFlagStatus(Meter_UART_Channel, USART_FLAG_PE|USART_FLAG_ORE|USART_FLAG_FE) != RESET)
		USART_ClearFlag(Meter_UART_Channel,USART_FLAG_ORE|USART_FLAG_FE|USART_FLAG_PE);
	
	if(USART_GetFlagStatus(Meter_UART_Channel,USART_FLAG_NE) != RESET)
		USART_ClearFlag(Meter_UART_Channel, USART_FLAG_NE);
	
  
	if(USART_GetITStatus(Meter_UART_Channel, USART_IT_RXNE) != RESET)
	{
		/* Read one byte from the receive data register */
		Meter_RxBuffer[MeterRxCounter] =(uint8_t)( USART_ReceiveData(Meter_UART_Channel) & 0x7f);
		MeterRxCounter++;
		Meter_RxBuffer[MeterRxCounter] = 0x00;
		newMeterDataFlag = true;

	}
	newRF_DataFlag = false;
}

////////////////////////////////////////////////////////////////////////////////////////


void SysTick_Handler(void){


	bool tempFlagMeter = newMeterDataFlag;
	bool tempFlagRF = newRF_DataFlag;

	static bool nextRF_ByteIsEnd = false;
	static bool nextMeterByteIsEnd = false;
	static bool baudFlag = false;

	uint8_t * tempRF_Buffer  = RF_RxBuffer;
	uint8_t * tempMeterBuffer = Meter_RxBuffer;

	RS485_ReadEnable();

	if(nextMeterByteIsEnd && tempFlagMeter){

		//send
		WriteToRF();
		nextMeterByteIsEnd = false;
		tempFlagMeter = false;
		MeterRxCounter = 0;
		*Meter_RxBuffer = 0x00;
		return;
	}


	if(nextRF_ByteIsEnd && tempFlagRF){

		//send
		WriteToMeter();
		nextRF_ByteIsEnd = false;
		tempFlagRF = false;
		RF_RxCounter = 0;
		*RF_RxBuffer = 0x00;
		return;
	}

	while(*tempRF_Buffer){



		/*Baud change 300*/
		if(*tempRF_Buffer == 0x2F){
			baud300();

		}
		/*Baud change*/
		if(*tempRF_Buffer == 0x06){
			baudFlag = true;
		}

		/*ETX*/
		if(*tempRF_Buffer == 0x03){
			nextRF_ByteIsEnd = true;
			tempFlagRF = false;

		}

		/*new line*/
		if(*tempRF_Buffer == 0x0A){

			WriteToMeter();
			if(baudFlag){
				baudChange();
				baudFlag = false;
				
			}
			*RF_RxBuffer = 0x00;
			RF_RxCounter = 0;
			return;

			//send
		}

		tempRF_Buffer++;
	}




	while(*tempMeterBuffer){

		/*ETX*/
		if(*tempMeterBuffer == 0x03){
			nextMeterByteIsEnd = true;
			tempFlagMeter = false;

		}
		/*new line*/
		if(*tempMeterBuffer == 0x0A){

			WriteToRF();
			*Meter_RxBuffer = 0x00;
			MeterRxCounter = 0;
			return;

			//send
		}
		tempMeterBuffer++;
	}



}
