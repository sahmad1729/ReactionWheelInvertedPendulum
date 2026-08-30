/*
 * MF4005v2.c
 *
 *  Created on: Oct 27, 2024
 */

#include "MF4005v2.h"
#include "can.h"

volatile uint8_t MOTOR_STATE = 0;
struct MotorCommands commands = {
		.MotorOff		= { MotorOff_Command, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		.MotorOn 		= { MotorOn_Command, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		.MotorStop 		= { MotorStop_Command, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		.SpeedControl	= { SpeedControl_Command, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		.EncoderValue	= { EncoderValue_Command, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		.MotorState2 	= { MotorState2_Command, 0x00, 0x00, 0x00, 0xAB, 0xAA, 0x00, 0x00}
};

void MOTOR_OFF(void)
{
	CAN_TRANSMIT(commands.MotorOff, MOTOR_ID);
}

void MOTOR_ON(void)
{
	CAN_TRANSMIT(commands.MotorOn, MOTOR_ID);
}

void MOTOR_STOP(void)
{
	CAN_TRANSMIT(commands.MotorStop, MOTOR_ID);
}

void MOTOR_SPEED_CONTROL(int32_t speed)
{
	commands.SpeedControl[4] = (speed >> 0) & 0xFF;
	commands.SpeedControl[5] = (speed >> 8) & 0xFF;
	commands.SpeedControl[6] = (speed >> 16) & 0xFF;
	commands.SpeedControl[7] = (speed >> 24) & 0xFF;
	CAN_TRANSMIT(commands.SpeedControl, MOTOR_ID);
}

void MOTOR_ENCODER_VALUE(void)
{
	CAN_TRANSMIT(commands.EncoderValue, MOTOR_ID);
}

void MOTOR_STATE3(void)
{
	CAN_TRANSMIT(commands.MotorState2, MOTOR_ID);
}
