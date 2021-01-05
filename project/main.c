/*uart 1 and uart 3 are working
PA9 tx1
pa10 rx1
pb10 tx3
pb11 rx3

pb6 scl
pb7 sda
*/
#include <stdio.h>
#define Freq8
#include "clk.h"
#include "usart.h"
#include "dma.h"
#include <string.h>
#include "common.h"
#include "nvic.h"
#include "i2c.h"
void Clock_72();
  char Buffer[5],send_buffer[20];	
volatile char received=0,i=0;
//function prototyping
 void extentionWrite1(unsigned char adrs, unsigned char reg,unsigned char value);	
 int extentionRead1(unsigned char adrs, unsigned char reg);
 //system init
 void SystemInit (void){
initClk();
}
 /*
 //implementation
 void extentionWrite1(unsigned char adrs, unsigned char reg,unsigned char value){	
	I2C_Start(I2C1);	//see I2C_CR1 for code
	I2C_Addr(I2C1,adrs);// see I2C_DR for code
	I2C_Write(I2C1,reg);
	I2C_Write(I2C1,value);
	I2C_Stop(I2C1);
}
 //read function
 int extentionRead1(unsigned char adrs, unsigned char reg){
	int r;
  // I2C_Start(I2C1);	//see I2C_CR1 for code
  // I2C_Addr (I2C1, adrs);// see I2C_DR for code
	// I2C_Write(I2C1, reg);
	I2C_Start(I2C1);	//see I2C_CR1 for code
	I2C_Addr (I2C1, adrs|1);// see I2C_DR for code
	r= I2C_Read(I2C1, ACKNOWLEDGE);
	I2C_Stop(I2C1);
	return r;
}*/
//********************************************
 
volatile int transfered = 0;

int main(){
	// enable alternate for UART 2
	 CLOCK_BUS_HIGH=DMACLOCK_ENABLE;
			  portInit(GPIOA,0xFFFF); //M0,M1 // LoRa has 4 modes to be selected Normal Mode M0,M1=0 , Low power mode 1,0, send configuration 0,1 , read configuration1,1
	    portInit(GPIOC,0xF100); // LED indicator 
	 	usartInit(USART1,9600,0);		//USART 1 For LoRa Connection

	  usartInit(USART3,9600,0);			//USART 3 for DEsktop App Connection

	//pb8,pb9

 	nvic_set_priority(NVIC_DMA1_CHANNEL3_IRQ, 0);
  nvic_enable_irq(NVIC_DMA1_CHANNEL3_IRQ);

	nvic_set_priority(NVIC_DMA1_CHANNEL3_IRQ, 0);
	nvic_enable_irq(NVIC_DMA1_CHANNEL3_IRQ);
	
	portInit(GPIOE,0xffff);
	transfered = 0;
	received = 0;
	   PORTC&=~(1<<13);

	
	while(1){
	// read BPM from Desktop App at uart 

			//RecvString(USART3,send_buffer,11);
		if(send_buffer[0]!='\0'){
				   PORTC&=~(1<<13);
		}
		else{
						PORTC|=(1<<13);
		}  
		 
	 //  GetString(USART3,Buffer);
	  	//sprintf(send_buffer,"S,%s,10",Buffer);
	 //	sprintf(send_buffer,"%d",strlen(send_buffer));
	// Send_String(USART3,send_buffer);	//send to LoRa
		PORTA&=~(1<<11);
		PORTA&=~(1<<12);
   Send_String(USART1,"Hello");	//send to LoRa
	 
	  	memset(send_buffer,0,sizeof(send_buffer));
				// delayms(500);

	//sleep for an amount of time	
		PORTA|=(1<<11);
		PORTA&=~(1<<12);
	  delayms(2000);

	}
	return 0;
}

void DMA1_Channel6_IRQHandler(void)
{
	if((DMA1_ISR &DMA_ISR_TCIF6) != 0)  {
		DMA1_IFCR |= DMA_IFCR_CTCIF5;
	
		 received = 1;
	}

	dma_disable_transfer_complete_interrupt(DMA1, DMA_CHANNEL6);

	dma_disable_channel(DMA1, DMA_CHANNEL6);
}
	//*******************************
void Clock_72(){
FLASH->ACR	= 0x12;	//See note
RCC->CR |= 1<<16;	//HSE on
while (!(RCC->CR & (1<<17)));	//wait until HSE ready
   //  RCC->CFGR2 = 3<<4;	//Predev2 divide by 8 -->8MHz
		  RCC->CR |= 1<<26;	//PLL2 on
		RCC->CFGR |= (1<<16);	
   //  RCC->CFGR2 |= 3<<8;	//PLL2 multiply by 8->40MHz
  	RCC->CFGR |= 7<<18;	// mult by 9	--->72MHz
  	RCC->CFGR |= 1<<10;	//APB1 = 36MHz. See bus clocks.
  	RCC->CR |= 1<<24;	//enable PLL
while (!(RCC->CR & (1<<25)));	//wait until locked
RCC->CFGR |= 2;	//PLL as clock source
}
//**************************************************88
	//Interrupt DMA for USART2 recv
void DMA1_Channel3_IRQHandler(void)
{
	if((DMA1_ISR &DMA_ISR_TCIF3) != 0)  {
		DMA1_IFCR |= DMA_IFCR_CTCIF3;
	
		received = 1;
	}

	dma_disable_transfer_complete_interrupt(DMA1, DMA_CHANNEL3);

	dma_disable_channel(DMA1, DMA_CHANNEL3);
}