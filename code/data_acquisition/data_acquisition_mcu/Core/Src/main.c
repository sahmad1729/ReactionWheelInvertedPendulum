
#include "main.h"
#include "rcc.h"
#include "usart.h"
#include "gpio.h"
#include "timer.h"
#include "usart.h"
#include "can.h"
#include "MF4005v2.h"

#include "input_signal.h"

int16_t lookupTable(void);

volatile uint16_t lookUpIndex = 0;
int16_t new_speed = 0;
int8_t previous_speed = 0;
int16_t wSpeed = 0;
float inputs[3] = {0};

#define THRESHOLD_ANGLE 10
#define PPR 2048
#define DEG_TO_RAD		0.0174532925199 // (pi/180)
#define TIME_STEP 		0.010            // 10 ms

int8_t prev_s = 1;

uint8_t TX_DATA[7] = {0, 0, 0, 0, 0, 0, 0};

int main(void)
{
	RCC_HSE_CONFIG();
	//SYSTICK_INIT(7200);

	GPIO_LED_CONFIG();
	//GPIO_EXTI_CONFIG();

	USART1_INIT();
	//usart_init();

	CAN_INIT();

	timer2_Basic_Init();
	timer3_Basic_Init();
	timer4_encoderMode_Init();
	GPIO_LED_OFF();
	MOTOR_OFF();

	RxDataPending = 0;
	while (1)
	{
		if (RxDataPending)
		{
			RxDataPending = 0;
			if (RxData == 1)
			{
				GPIO_LED_ON();
				MOTOR_ON();
				timer4_start();
				timer2_start();
			}
			else if (RxData == 2)
			{
				timer4_stop();
				timer2_stop();
				GPIO_LED_OFF();
				lookUpIndex = 0;
				for (uint16_t i = 0; i < 5000; i++);
				MOTOR_STOP();
				for (uint16_t i = 0; i < 5000; i++);
				MOTOR_OFF();
			}
		}

		if (timerInterruptPending == 1)
		{
			timerInterruptPending = 0;
			CAN_TRANSMIT(commands.MotorState2, MOTOR_ID);

			TX_DATA[1] = encoder_count & 0xFF;
			TX_DATA[2] = encoder_count >> 8;

			//can_message_pending = 1;
			while (can_message_pending == 0);
			can_message_pending = 0;

			if (lookUpIndex < SIGNAL_LENGTH)
			{
				new_speed = ((int16_t)mlpr_signal[lookUpIndex++])*100;
			}
			else
			{
				RxData = 2;
				RxDataPending = 1;
			}

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

