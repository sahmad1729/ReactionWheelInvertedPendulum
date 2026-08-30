/*
 * rcc.h
 *
 *  Created on: Aug 20, 2024
 */

#ifndef PERIPHERALS_INC_RCC_H_
#define PERIPHERALS_INC_RCC_H_

#include "main.h"

void RCC_HSE_CONFIG(void);
void SYSTICK_INIT(uint32_t reload_value);
uint32_t GET_MSTICKS(void);


#endif /* PERIPHERALS_INC_RCC_H_ */
