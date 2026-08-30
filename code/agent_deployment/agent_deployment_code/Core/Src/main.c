
#include "main.h"
#include "rcc.h"
#include "usart.h"
#include "gpio.h"
#include "timer.h"
#include "usart.h"
#include "can.h"
#include "MF4005v2.h"
#include "agent.h"

//#include "LOOKUPTABLE.h"

int16_t 		new_speed = 0;
int8_t 			previous_speed = 0;
float 			angle = 0;
float 			prev_angle = 180;
float 			pSpeed = 0;
int16_t 		wSpeed = 0;

float 			inputs[3] = {0};

#define THRESHOLD_ANGLE 10
#define PPR 2048
#define DEG_TO_RAD		0.0174532925199 // (pi/180)
#define TIME_STEP 		0.010            // 10 ms


uint8_t TX_DATA[7] = {0, 0, 0, 0, 0, 0, 0};

#define IDLE	0
#define BALANCE 1
uint8_t pendulum_state = IDLE;
uint8_t pendulum_state1 = IDLE;

void start_balancing(void)
{
	GPIO_LED_ON();
	MOTOR_ON();
	timer4_start();
	timer2_start();
}

void stop_balancing(void)
{
	timer4_stop();
	timer2_stop();

	for (uint16_t i = 0; i < 5000; i++);
	MOTOR_STOP();
	for (uint16_t i = 0; i < 5000; i++);
	MOTOR_OFF();
	for (uint16_t i = 0; i < 5000; i++);
	GPIO_LED_OFF();

}

int main(void)
{
	RCC_HSE_CONFIG();

	GPIO_LED_CONFIG();
	GPIOA_EXTI_CONFIG();

	USART1_INIT();

	CAN_INIT();

	timer2_Basic_Init();
	timer4_encoderMode_Init();
	GPIO_LED_OFF();
	MOTOR_OFF();

	RxDataPending = 0;
	while (1)
	{
		if (RxDataPending || ExtIntPending)
		{
			if (ExtIntPending == 1)
			{
				ExtIntPending = 0;

				if (pendulum_state1 == BALANCE)
				{
					pendulum_state1 = IDLE;
					stop_balancing();
				}
				else if (pendulum_state1 == IDLE)
				{
					pendulum_state1 = BALANCE;
					start_balancing();
				}
			}
			if (RxDataPending)
			{
				RxDataPending = 0;
				if (RxData == 1)
				{
					pendulum_state = BALANCE;
					start_balancing();
				}

				else if (RxData == 2)
				{
					pendulum_state = IDLE;
					stop_balancing();
				}
			}
		}

		if (timerInterruptPending == 1)
		{
			timerInterruptPending = 0;
			CAN_TRANSMIT(commands.MotorState2, MOTOR_ID);

			TX_DATA[1] = encoder_count & 0xFF;
			TX_DATA[2] = encoder_count >> 8;

			int8_t direction = TX_DATA[0] == 0 ? 1 : -1;
			TX_DATA[0] = direction == 1 ? 0 : 1;
			angle = (encoder_count%PPR)*(360.0/PPR);
			pSpeed = fabs(angle - prev_angle);
			if (pSpeed > 180)
			{
				if ((360.0 - angle) < (360.0 - prev_angle))
					pSpeed = (360.0 - angle) + (prev_angle);
				else
					pSpeed = angle + (360.0 - prev_angle);
			}
			prev_angle = angle;
			pSpeed = direction*(pSpeed / TIME_STEP);
			inputs[1] = pSpeed*(DEG_TO_RAD);

			if (angle > 180.0f)
				angle = angle - 360.0f;

			//can_message_pending = 1;
			while (can_message_pending == 0);
			can_message_pending = 0;

			inputs[2] = wSpeed*(DEG_TO_RAD);

			inputs[0] = angle*(DEG_TO_RAD);
			new_speed = forward_pass(inputs);

			if (new_speed != previous_speed)
			{
				MOTOR_SPEED_CONTROL(new_speed*600);
				previous_speed = new_speed;
				TX_DATA[5] = new_speed & 0xFF;
				TX_DATA[6] = (new_speed >> 8) & 0xFF;
			}

			WRITE_USART_TX_BUFFER(TX_DATA, 7);
			USART1_TRANSMIT_IT();
		}
	}
}
