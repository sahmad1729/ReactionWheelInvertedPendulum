/*
 * rcc.c
 *
 *  Created on: Aug 20, 2024
 */

#include "rcc.h"

static __IO uint32_t msTicks = 0;

void RCC_HSE_CONFIG(void)
{
	//HSE ON
	RCC->CR &= ~(RCC_CR_HSEON);
	RCC->CR |= (RCC_CR_HSEON);
	while(!(RCC->CR & RCC_CR_HSERDY)){}

	//Flash pre-fetch and wait state, 2WS -> 72 MHz
	FLASH -> ACR &= ~(FLASH_ACR_LATENCY);
	FLASH -> ACR |= FLASH_ACR_LATENCY_2;
	//Enable pre-fetch buffer
	FLASH -> ACR |= FLASH_ACR_PRFTBE;

	//Select PLL as source
	//RCC->CFGR &= ~(RCC_CFGR_PLLSRC);
	RCC->CFGR |= (RCC_CFGR_PLLSRC);

	//Setting PLLMUL to 9, and USB pre-scalar to 1.5
	//RCC->CFGR &= ~(RCC_CFGR_PLLXTPRE);
	//RCC->CFGR &= ~(RCC_CFGR_PLLMULL);
	RCC->CFGR |= RCC_CFGR_PLLMULL9;
	//RCC->CFGR &= ~(RCC_CFGR_USBPRE);

	//PLL ON
	//RCC->CR &= ~(RCC_CR_PLLON);
	RCC->CR |= (RCC_CR_PLLON);
	while(!(RCC->CR & RCC_CR_PLLRDY)){}

	//Select APB1 clock divisor 2, Max APB1 clock speed is 36MHz
	RCC->CFGR &= ~(RCC_CFGR_PPRE1);
	RCC->CFGR |= (RCC_CFGR_PPRE1_DIV2);

	//Select APB2 clock divisor 1
	RCC->CFGR &= ~(RCC_CFGR_PPRE2);
	RCC->CFGR |= (RCC_CFGR_PPRE2_DIV1);

	//Select AHB clock divisor as 1
	RCC->CFGR &= ~(RCC_CFGR_HPRE);
	RCC->CFGR |= (RCC_CFGR_HPRE_DIV1);

	//Turn Select
	RCC->CFGR &= ~(RCC_CFGR_SW);
	RCC->CFGR |= (RCC_CFGR_SW_PLL);
	while(!(RCC->CFGR & RCC_CFGR_SWS_PLL)){}
}

void SYSTICK_INIT(uint32_t reload_value)
{
	//Load the required value in SysTick LOAD
	SysTick->LOAD = reload_value - 1;
	//Set the SysTick count value to 0
	SysTick->VAL = 0;
	//Select Clock Source
	SysTick->CTRL |= (SysTick_CTRL_CLKSOURCE_Msk);
	//Configure interrupt priority
	NVIC_SetPriority(SysTick_IRQn, 0);
	//Count down to zero asserts SysTick
	SysTick->CTRL |= (SysTick_CTRL_TICKINT_Msk);
	//Enable Counter
	SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk);
}

uint32_t GET_MSTICKS(void)
{
	return msTicks;
}

void SysTick_Handler(void)
{
	NVIC_ClearPendingIRQ(SysTick_IRQn);
	msTicks++;
}







