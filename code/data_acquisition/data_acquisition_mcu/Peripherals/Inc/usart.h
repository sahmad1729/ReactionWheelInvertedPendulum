/*
 * usart.h
 *
 *  Created on: Aug 23, 2024
 */

#ifndef PERIPHERALS_INC_USART_H_
#define PERIPHERALS_INC_USART_H_

#include "main.h"

extern uint8_t TxBuffer_Size;
extern uint8_t *TxBuffer;
extern volatile uint8_t TxIndex;
extern uint8_t RxData;
extern volatile uint8_t RxDataPending;

void usart_init(void);

void USART1_INIT(void);
void WRITE_USART_TX_BUFFER(uint8_t *data, uint8_t length);
uint8_t USART1_TRANSMIT_IT();
void USART1_TRANSMIT();

#endif /* PERIPHERALS_INC_USART_H_ */
