/*
 * MF4005v2.h
 *
 *  Created on: Oct 27, 2024
 */

#ifndef MF4005V2_INC_MF4005V2_H_
#define MF4005V2_INC_MF4005V2_H_

#include "main.h"

#define MOTOR_ID 0x141

struct MotorCommands {
	uint8_t MotorOff[8];
	uint8_t MotorOn[8];
	uint8_t MotorStop[8];
	uint8_t SpeedControl[8];
	uint8_t EncoderValue[8];
	uint8_t MotorState2[8];
};

extern struct MotorCommands commands;

extern volatile uint8_t MOTOR_STATE;

void MOTOR_OFF(void);
void MOTOR_ON(void);
void MOTOR_STOP(void);
void MOTOR_SPEED_CONTROL(int32_t speed);
void MOTOR_ENCODER_VALUE(void);
void MOTOR_STATE3(void);

#endif /* MF4005V2_INC_MF4005V2_H_ */
