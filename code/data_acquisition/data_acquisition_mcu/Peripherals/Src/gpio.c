/*
 * gpio.c
 *
 *  Created on: Aug 23, 2024
 */

#include "gpio.h"

void GPIO_LED_CONFIG(void)
{
	//If the clock is not enabled, Enable Clock
	if (!(RCC->APB2ENR & RCC_APB2ENR_IOPCEN))
	{
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	}
	//Set Mode to Output
	GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
	//Writing 1 to ODR turns off the led (active low)
	GPIOC->ODR |= (GPIO_ODR_ODR13);
	GPIOC->CRH |= GPIO_CRH_MODE13_1;
	//General Purpose Push Pull
	GPIOC->CRH |= 0 << GPIO_CRH_CNF13_Pos;
}

void GPIO_LED_ON(void)
{
	//Reset PC13, LED is active Low
	GPIOC->BSRR |= GPIO_BSRR_BR13;
}

void GPIO_LED_OFF(void)
{
	//Set PC13, LED is active Low
	GPIOC->BSRR |= GPIO_BSRR_BS13;
}

void GPIO_LED_TOGGLE(void)
{
	GPIOC->ODR ^= GPIO_ODR_ODR13;
}

void GPIO_EXTI_CONFIG(void)
{
	//If the clock is not enabled, Enable Clock
	if (!(RCC->APB2ENR & RCC_APB2ENR_IOPCEN))
	{
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	}
	//Enable clock for AFIO
	if (!(RCC->APB2ENR & RCC_APB2ENR_AFIOEN))
	{
		RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	}

	//Map PC5 to EXTI5
	AFIO->EXTICR[1] &= ~(AFIO_EXTICR2_EXTI5_PC);
	AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI5_PC;

	//Configure PC5 as input, pull down
	GPIOC->CRL &= ~(GPIO_CRL_MODE5); //Input
	GPIOC->CRL &= ~(GPIO_CRL_CNF5);
	GPIOC->CRL |= (GPIO_CRL_CNF5_1); //Input Floating
	GPIOC->ODR &= ~(GPIO_ODR_ODR5); //Input Pull Down

	//Configure mask bits
	EXTI->IMR |= EXTI_IMR_IM5;
	//Enable Rising Edge Trigger
	EXTI->RTSR |= EXTI_RTSR_RT5;
	//Disable Falling Edge Trigger
	EXTI->FTSR &= ~(EXTI_FTSR_FT5);

	NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void EXTI9_5_IRQHandler(void)
{
	if (EXTI->PR & EXTI_PR_PR5)
	{
		//Button press detected
		//Clear Pending interrupt
		EXTI->PR |= EXTI_PR_PR5;
		GPIO_LED_TOGGLE();
	}
}
