/*
 * usart.c
 *
 *  Created on: Aug 23, 2024
 */

#include "usart.h"

uint8_t TxBuffer_Size = 0;
uint8_t *TxBuffer = NULL;
volatile uint8_t TxIndex = 0;
uint8_t RxData = {0};
volatile uint8_t RxDataPending = 0;

void usart_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN;

	// Configure PA9 (TX) as alternate function push-pull
	GPIOA->CRH &= ~GPIO_CRH_CNF9;        // Clear CNF9 bits
	GPIOA->CRH |= GPIO_CRH_CNF9_1;       // Set PA9 as AF push-pull
	GPIOA->CRH |= GPIO_CRH_MODE9;        // Set PA9 as output max speed 50 MHz

	// Configure PA10 (RX) as input floating
	GPIOA->CRH &= ~GPIO_CRH_CNF10;       // Clear CNF10 bits
	GPIOA->CRH |= GPIO_CRH_CNF10_0;      // Set PA10 as input floating
	GPIOA->CRH &= ~GPIO_CRH_MODE10;      // Set PA10 as input mode

	USART1->BRR = (39 << 4) | (1 << 0);
	//Enable Transmitter and Receiver
	USART1->CR1 |= (USART_CR1_TE  | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_UE);

	//Enable USART1 interrupt in NVIC
	NVIC_EnableIRQ(USART1_IRQn);

}

void USART1_INIT(void)
{
	//USART1 Remap = 0 => Tx: PA9, Rx: PA10
	//Enable Clock to GPIO Port A
	if (!(RCC->APB2ENR & RCC_APB2ENR_IOPAEN))
	{
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	}
	//Enable USART1 Clock
	if (!(RCC->APB2ENR & RCC_APB2ENR_USART1EN))
	{
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	}

	//PA9
	//USART1 TX: AF PP or AF OD
	GPIOA->CRH &= ~(GPIO_CRH_CNF9);
	GPIOA->CRH |= (2 << GPIO_CRH_CNF9_Pos);
	//Output Mode, Speed: 10 MHz
	GPIOA->CRH &= ~(GPIO_CRH_MODE9);
	GPIOA->CRH |= (1 << GPIO_CRH_MODE9_Pos);


	//PA10
	//USART1 RX: Input Floating or Input Pull Up
	GPIOA->CRH &= ~(GPIO_CRH_CNF10);
	GPIOA->CRH |= (2 << GPIO_CRH_CNF10_Pos);
	GPIOA->ODR |= (GPIO_ODR_ODR10);
	//Input Mode, Speed: 10 MHz
	GPIOA->CRH &= ~(GPIO_CRH_MODE10);

	//Map PA9, PA10 to USART1
	if (!(RCC->APB2ENR & RCC_APB2ENR_AFIOEN))
	{
		RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	}
	AFIO->MAPR &= ~(AFIO_MAPR_USART1_REMAP); //Reset bit => no re-mapping

	//Select 8 word length
	USART1->CR1 &= ~(USART_CR1_M);
	//Select Stop Bits
	USART1->CR2 &= ~(USART_CR2_STOP);
	//DMA Configuration
	//Select Baud rate
	//115200 -> 39, 1; 230400 -> 19, 8; 460800 -> 9, 12; 921600 -> 4, 14
	USART1->BRR = (4 << 4) | (14 << 0);
	//Enable Transmitter and Receiver, Enable USART1
	USART1->CR1 |= (USART_CR1_TE  | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_UE);

	//Set Priority
	NVIC_SetPriority(USART1_IRQn, 1);

	//Enable USART1 interrupt in NVIC
	NVIC_EnableIRQ(USART1_IRQn);
}

void WRITE_USART_TX_BUFFER(uint8_t *data, uint8_t length)
{
	TxBuffer = data;
	TxBuffer_Size = length;
}

uint8_t USART1_TRANSMIT_IT()
{
	if (!(USART1->SR & USART_SR_TC))
		return 0; //Transmission in progress

	//Write to DR, send start byte
	USART1->DR = 0xAA;
	USART1->CR1 |= USART_CR1_TXEIE;
	return 1;
}

void USART1_TRANSMIT()
{
	uint8_t index = 0;
	while(index < TxBuffer_Size)
	{
		//TXE is 1, if DR is empty (we can put data in DR)
		if (USART1->SR & USART_SR_TXE)
		{
			USART1->DR = TxBuffer[index]; //Writing data to DR resets TXE
			index++;
		}
	}
	TxBuffer_Size = 0;
	TxBuffer = NULL;
	//TXE may be one but the data might still be in transmission i.e.
	//shift register is not empty yet
	//When transmission is complete, TC is set
	while(!(USART1->SR & USART_SR_TC));
}

void USART1_IRQHandler(void)
{
	if (USART1->SR & USART_SR_TXE)
	{
		if (TxIndex < TxBuffer_Size)
		{

			if (TxBuffer[TxIndex] == 0xAA)
			{
				USART1->DR = 0xAB;
				TxBuffer[TxIndex] = 0x01;
			}
			else if (TxBuffer[TxIndex] == 0xAB)
			{
				USART1->DR = 0xAB;
				TxBuffer[TxIndex] = 0x00;
			}
			else
			{
				USART1->DR = TxBuffer[TxIndex++];
			}
		}
		else
		{
			TxIndex = 0;
			TxBuffer_Size = 0;
			TxBuffer = NULL;
			//Transmission complete, disable TxE interrupts
			USART1->CR1 &= ~(USART_CR1_TXEIE);
		}
	}

	if (USART1->SR & USART_SR_RXNE)
	{
		RxDataPending = 1;
		RxData = USART1->DR;
	}
}








