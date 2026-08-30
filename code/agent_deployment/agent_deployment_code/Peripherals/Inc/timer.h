/*
 * timer.h
 *
 *  Created on: Aug 25, 2024
 */

#ifndef PERIPHERALS_INC_TIMER_H_
#define PERIPHERALS_INC_TIMER_H_

#include "main.h"

extern volatile uint8_t timerInterruptPending;
extern uint16_t encoder_count;
extern uint8_t encoder_data[3];

void timer2_Basic_Init(void);
void timer2_start(void);
void timer2_stop(void);

void timer3_Basic_Init(void);

void timer4_encoderMode_Init(void);
void timer4_start(void);
void timer4_stop(void);

#endif /* PERIPHERALS_INC_TIMER_H_ */
