/*
 * can.h
 *
 *  Created on: Oct 17, 2024
 */

#ifndef PERIPHERALS_INC_CAN_H_
#define PERIPHERALS_INC_CAN_H_

#include "main.h"

#define	MotorOff_Command		0x80
#define	MotorOn_Command 		0x88
#define	MotorStop_Command		0x81
#define	SpeedControl_Command	0xA2
#define	EncoderValue_Command	0x90
#define	MotorState2_Command 	0x9C

struct MotorState2 {
	uint8_t command_byte;
	uint8_t temperature;
	uint8_t TorqueCurrent[2];
	uint8_t Speed[4];
	uint8_t EncoderPosition[2];
};

extern struct MotorState2 motor_state2;

extern volatile uint8_t can_message_pending;

void CAN_INIT(void);
void CAN_TRANSMIT(uint8_t *data, uint16_t ID);

#endif /* PERIPHERALS_INC_CAN_H_ */
