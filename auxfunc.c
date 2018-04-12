#include "auxfunc.h"
#include "defines.h"


USART_InitTypeDef USART_InitStruct;

//Usart buffers
uint8_t RF_RxBuffer[100];
uint8_t Meter_RxBuffer[100];

//Usart buffer counters
uint8_t RF_RxCounter = 0;
uint8_t MeterRxCounter =0;

bool newRF_DataFlag = false;
bool newMeterDataFlag = false;
	

int rs485_baud_rate_mapping [] =
{
	300,
	600,
	1200,
	2400,
	4800,
	9600,
	19200,
	0
};

void delay(int i){

	while(i)
		i--;
}
//Write incoming data to meter
void WriteToMeter(void){

	RS485_WriteEnable();
	
	dataBitConvert(RF_RxBuffer,RF_RxCounter);
	
	//lower this
	delay(500000);
	
	UART_SendString(Meter_UART_Channel, RF_RxBuffer,RF_RxCounter);

	//wait until transmission complated
	while(!USART_GetFlagStatus(Meter_UART_Channel, USART_FLAG_TC));

	RS485_ReadEnable();
	
	USART_ClearFlag(Meter_UART_Channel, USART_FLAG_ORE|USART_FLAG_FE|USART_FLAG_PE|USART_FLAG_RXNE);
	
}
//Write response of meter to rf module
void WriteToRF(void){
	
	//Disable interrupt
	USART_ITConfig(RF_UART_Channel, USART_IT_RXNE, DISABLE);
	USART_ITConfig(RF_UART_Channel, USART_IT_ORE, DISABLE);
	USART_ITConfig(RF_UART_Channel, USART_IT_PE, DISABLE);

	delay(650);
	
	
	UART_SendString(RF_UART_Channel, Meter_RxBuffer,MeterRxCounter);
	
  while(!USART_GetFlagStatus(RF_UART_Channel, USART_FLAG_TC));

	USART_ClearFlag(RF_UART_Channel, USART_FLAG_ORE|USART_FLAG_FE|USART_FLAG_PE|USART_FLAG_RXNE);
	
	//Enableinterrupt
	USART_ITConfig(RF_UART_Channel, USART_IT_RXNE, ENABLE);
	USART_ITConfig(RF_UART_Channel, USART_IT_ORE, ENABLE);
	USART_ITConfig(RF_UART_Channel, USART_IT_PE, ENABLE);
}
//set meter side uart to 300 baud
void baud300(void){

	USART_Cmd(Meter_UART_Channel, DISABLE);
	USART_InitStruct.USART_BaudRate = rs485_baud_rate_mapping[0];
	USART_Init(Meter_UART_Channel,&USART_InitStruct);
	USART_Cmd(Meter_UART_Channel, ENABLE);
}
//Change baudrate of meter side usart due to incoming obis command
void baudChange(void){

	USART_Cmd(Meter_UART_Channel, DISABLE);

	USART_InitStruct.USART_BaudRate = rs485_baud_rate_mapping[(RF_RxBuffer[2] & 0x0f)];
	USART_Init(Meter_UART_Channel,&USART_InitStruct);
	
	USART_Cmd(Meter_UART_Channel, ENABLE);
}
////////////////////////////////////////////////////////////////////////////////////////

//Clock required usart and gpio peripherals
void RCC_Configuration(void){

	RCC_GPIO;
	RCC_USART_Meter;
	RCC_USARRT_RF;
}

//////////////////////////////////////////////////////////////////////////////
void GPIO_Configuration(void){

	GPIO_InitTypeDef    GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Mode    = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType   = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin     = MeterRX_Pin | MeterTX_Pin;
	GPIO_InitStruct.GPIO_PuPd    = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed   = GPIO_Speed_2MHz;
	GPIO_Init(MeterUSART_GPIO_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin     =  RF_RX_Pin | RF_TX_Pin;
	GPIO_Init(RF_USART_GPIO_Port, &GPIO_InitStruct);

	AF_CONFIG;

	GPIO_InitStruct.GPIO_Pin = RF_SetBit|RF_CS_Bit;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(RF_SetPort, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = RS485_ControlPin;
	GPIO_Init(RS485_ControlPort, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = RS485_ControlPin ;
	GPIO_Init(RS485_ControlPort, &GPIO_InitStruct);
}

////////////////////////////////////////////////////////////////////////////////
void USART_Configuration(void){

	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

	USART_Init(RF_UART_Channel,&USART_InitStruct);
	USART_InitStruct.USART_BaudRate = 300;
	USART_Init(Meter_UART_Channel,&USART_InitStruct);
}

/////////////////////////////////////////////////////////////////////////////////
//enable usart interrupt at nvic
void NVIC_Configuration(void){
	
	NVIC_InitTypeDef NVIC_InitStruct;

	NVIC_InitStruct.NVIC_IRQChannel          = Meter_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd       = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPriority  = 0;
	NVIC_Init(&NVIC_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannel = RF_IRQn;

	NVIC_Init(&NVIC_InitStruct);
}
//Configure and enable systemtick
void SysTick_Configuration(){

	NVIC_SetPriority(SysTick_IRQn,0x0f);
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq (&RCC_Clocks);
	SysTick_Config (RCC_Clocks.HCLK_Frequency / SysTickFreq);

}

//send string of byte to usart
void UART_SendString(USART_TypeDef* USARTx, uint8_t * data, uint8_t length){
	
	int i = 0;
	while(i < length){

		while(!USART_GetFlagStatus(USARTx, USART_FLAG_TXE));
		USART_SendData(USARTx, data[i]);
		i++;
	}
	
}


///macro seems better
 void RS485_ReadEnable(void){

	GPIO_ResetBits(RS485_ControlPort,RS485_ControlPin);
}
 void RS485_WriteEnable(void){
	GPIO_SetBits(RS485_ControlPort,RS485_ControlPin);
}

//Converts array of 8 bit datas to 7 bit even parity
void dataBitConvert(uint8_t data[30],uint8_t length){

	uint8_t temp = 0;
	uint8_t count=0;
	
	for(int k = 0; k<length; k++){
		temp = data[k];
		

		//count parity
		for(int i = 0; i<7; i++){

			if(temp & 0x01){
				count++;
			}
			temp = temp>>1;
		}

		//add parity to data byte
		if((count%2)!=0 ){
			data[k]|=0x80;
			count = 0;
		}
		else{
			data[k]&=0x7f;
			count = 0;

		}
	}
}
//General exeption handler with system reset
void ExeptionReset(void){

	//wait some
	int i = 500;
	while(i)
		i--;
	//Reset system
	NVIC_SystemReset();
}

