/*
 * timer.c
 *
 *  Created on: Aug 25, 2024
 */

#include "timer.h"

volatile uint8_t timerInterruptPending = 0;

volatile uint32_t msTicks = 0;
volatile uint32_t captured_value = 0;

uint16_t encoder_count = 0;
uint8_t encoder_data[3] = {0, 0, 0};
#define encoder_resolution 2048

void timer2_Basic_Init(void)
{
	//Enable Clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	//Set pre-scalar to 7200 - 1, tim_clk frequency = 10kHz
	TIM2->PSC = 7200 - 1;
	//Auto Reload value, 1000 => 10Hz frequency => 100 ms period
	TIM2->ARR = 100 - 1; //Don't forget to change the sampling rate in python script
	//Count Up
	TIM2->CR1 &= ~(TIM_CR1_DIR);
	//Periodic
	TIM2->CR1 &= ~(TIM_CR1_OPM);
	//TRGO is generated on an UEV when the counter reaches ARR
	TIM2->CR2 &= ~(TIM_CR2_MMS);
	TIM2->CR2 |= TIM_CR2_MMS_1;
	//Update registers on event
	TIM2->EGR = 1;
	// Configure Timer 2 to generate TRGO on update event
	TIM2->CR2 |= TIM_CR2_MMS_1; // MMS = 010 (Update Event)
	//Start timer
	//TIM2->CR1 |= TIM_CR1_CEN;
	//Enable timer2 interrupts
	//TIM2->DIER |= TIM_DIER_UIE;
	//Set Priority
	//NVIC_SetPriority(TIM2_IRQn, 3);
	//Enable Interrupt
	//NVIC_EnableIRQ(TIM2_IRQn);
}

void timer2_start(void)
{
	//Start timer
	TIM2->CR1 |= TIM_CR1_CEN;
	//Enable timer2 interrupts
	//TIM2->DIER |= TIM_DIER_UIE;
}

void timer2_stop(void)
{
	//Start timer
	TIM2->CR1 &= ~TIM_CR1_CEN;
	//Enable timer2 interrupts
	//TIM2->DIER &= ~TIM_DIER_UIE;
}

void timer3_Basic_Init(void)
{
	//Enable Clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	//Set pre-scalar to 720 - 1, tim_clk frequency = 100kHz
	TIM3->PSC = 720 - 1;
	//Auto Reload value, 1000 => 100Hz frequency => 10 ms period
	TIM3->ARR = 35 - 1;
	//Count Up
	TIM3->CR1 &= ~(TIM_CR1_DIR);
	//Periodic
	TIM3->CR1 &= ~(TIM_CR1_OPM);
	//TRGO is generated on an UEV when the counter reaches ARR
	TIM3->CR2 &= ~(TIM_CR2_MMS);
	TIM3->CR2 |= (2 << TIM_CR2_MMS_Pos);
	//Update registers on event
	TIM3->EGR = 1;
	//Start timer
	//TIM3->CR1 |= TIM_CR1_CEN;
	//Enable timer3 interrupts
	TIM3->DIER |= TIM_DIER_UIE;
	//Set Priority
	NVIC_SetPriority(TIM3_IRQn, 3);
	//Enable Interrupt
	NVIC_EnableIRQ(TIM3_IRQn);
}

void timer4_encoderMode_Init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	//CH1 -> PB6, CH2 -> PB7
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	//Configure PB6 and PB7 as input pull up
	GPIOB->CRL &= ~(GPIO_CRL_MODE7 | GPIO_CRL_MODE6); //Input
	GPIOB->CRL &= ~(GPIO_CRL_CNF7 | GPIO_CRL_CNF6);
	GPIOB->CRL |= (GPIO_CRL_CNF7_1 | GPIO_CRL_CNF6_1);
	GPIOB->ODR |= (GPIO_ODR_ODR7 | GPIO_ODR_ODR6);
	//Set ARR to Max
	TIM4->ARR = 0xFFFF;
	//TI1 can go to either TI1FP1 or TI1FP2 AND TI2 -> TI2FP1 or TI2FP2
	//We need to map TI1FP1 on TI1 AND TI2FP2 to TI2
	TIM4->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;
	TIM4->CCMR1 |= (TIM_CCMR1_IC1F_1 | TIM_CCMR1_IC2F_1); //Set input filter to 4 samples
	TIM4->CCER &= ~TIM_CCER_CC1P;//TI1FP1, P1: Rising Edge
	TIM4->CCER &= ~TIM_CCER_CC2P; //TI2FP2, P2: Falling Edge
	//Set timer 4 in encoder mode
	TIM4->SMCR &= ~(TIM_SMCR_SMS);
	TIM4->SMCR |= TIM_SMCR_SMS_1;
	//TIM4->SMCR |= (TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1);
	//Select ITR1 (timer 2 TRGO) as TRC
	TIM4->SMCR &= ~(TIM_SMCR_TS);
	TIM4->SMCR |= TIM_SMCR_TS_0;
	//Map IC3 to TRC
	TIM4->CCMR2 &= ~(TIM_CCMR2_CC3S);
	TIM4->CCMR2 |= (TIM_CCMR2_CC3S_0 | TIM_CCMR2_CC3S_1);
	//Set prescaler to zero
	TIM4->CCMR2 &= ~(TIM_CCMR2_IC3PSC);
	//Set filer to 4 samples
	TIM4->CCMR2 |= TIM_CCMR2_IC3F_1;
	//Enable Capture
	TIM4->CCER |= TIM_CCER_CC3E;
	//Enable CC3 interrupt
	TIM4->DIER |= TIM_DIER_CC3IE;
	//Initialize counter to half of the encoder resolution
	TIM4->CNT = encoder_resolution/2;
	//Enable timer 4
	//TIM4->CR1 |= TIM_CR1_CEN;
	//Enable interrupt
	NVIC_EnableIRQ(TIM4_IRQn);
}

void timer4_start(void)
{
	//Start timer
	TIM4->CR1 |= TIM_CR1_CEN;
}

void timer4_stop(void)
{
	//Stop timer
	TIM4->CR1 &= ~TIM_CR1_CEN;;
}
/*
void TIM2_IRQHandler(void)
{
	//Not applicable here
	if (TIM2->SR & TIM_SR_UIF)
	{
		timerInterruptPending = 1;
		encoder_count = 0xff;
		//Clear the interrupts flag
		TIM2->SR &= ~(TIM_SR_UIF);
		//TIM3->CR1 |= TIM_CR1_CEN;
	}
	if (TIM2->SR & TIM_SR_CC1IF)
	{
		captured_value = TIM2->CCR1;
		//Clear interrupt flag
		TIM2->SR &= ~(TIM_SR_CC1IF);
	}
}
*/
void TIM3_IRQHandler(void)
{
	if (TIM3->SR & TIM_SR_UIF)
	{
		timerInterruptPending = 2;
		//Stop timer3
		TIM3->CR1 &= ~(TIM_CR1_CEN);
		//Clear the interrupts flag
		TIM3->SR &= ~(TIM_SR_UIF);
	}

}

void TIM4_IRQHandler(void)
{
    // Check if CC3 interrupt occurred
	if (TIM4->SR & TIM_SR_CC3IF)
	{
		timerInterruptPending = 1;
		encoder_count = TIM4->CCR3; // Read the captured Timer 4 counter value
		TX_DATA[0] = (TIM4->CR1)&TIM_CR1_DIR;
		TIM4->SR &= ~TIM_SR_CC3IF; // Clear the CC3 interrupt flag
		//TIM3->CR1 |= TIM_CR1_CEN;
	}
}
