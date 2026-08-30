/*
 * gpio.h
 *
 *  Created on: Aug 23, 2024
 */

#ifndef PERIPHERALS_INC_GPIO_H_
#define PERIPHERALS_INC_GPIO_H_

#include "main.h"

void GPIO_LED_CONFIG(void);
void GPIO_LED_ON(void);
void GPIO_LED_OFF(void);
void GPIO_LED_TOGGLE(void);

void GPIO_EXTI_CONFIG(void);

#endif /* PERIPHERALS_INC_GPIO_H_ */
