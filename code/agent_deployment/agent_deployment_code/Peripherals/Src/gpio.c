/*
 * gpio.c
 *
 *  Created on: Aug 23, 2024
 */

#include "gpio.h"

volatile uint8_t ExtIntPending = 0;

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

void GPIOA_EXTI_CONFIG(void)
{
    // Enable GPIOA clock and AFIO clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

    // Map PA0 to EXTI0
    AFIO->EXTICR[0] &= ~(AFIO_EXTICR1_EXTI0);
    AFIO->EXTICR[0] |= (AFIO_EXTICR1_EXTI0_PA);

    // Configure PA0 as input with internal pull-up
    GPIOA->CRL &= ~(GPIO_CRL_MODE0); // MODE0 = 00 : input mode
    GPIOA->CRL &= ~(GPIO_CRL_CNF0); // clear CNF0 bits
    GPIOA->CRL |= (GPIO_CRL_CNF0_1); // CNF0 = 10 : input with PU/PD
    GPIOA->ODR |= (GPIO_ODR_ODR0); // ODR5 = 1 : pull-up active

    // Configure EXTI line 0
    EXTI->IMR |= EXTI_IMR_MR0;
    EXTI->RTSR &= ~(EXTI_RTSR_TR0); // disable rising edge
    EXTI->FTSR |= EXTI_FTSR_TR0; // enable falling edge trigger

    NVIC_SetPriority(EXTI0_IRQn, 15);

    // Enable EXTI0 interrupt in NVIC
    NVIC_EnableIRQ(EXTI0_IRQn);
}

void EXTI0_IRQHandler(void)
{
    if (EXTI->PR & EXTI_PR_PR0)
    {
        EXTI->PR |= EXTI_PR_PR0; // clear pending bit

        // Wait for bouncing to finish
        for (volatile uint32_t i = 0; i < 5000; i++);

        // Confirm the button is still pressed (line low)
        if (!(GPIOA->IDR & GPIO_IDR_IDR0))
        {
            ExtIntPending = 1;
        }
    }
}


