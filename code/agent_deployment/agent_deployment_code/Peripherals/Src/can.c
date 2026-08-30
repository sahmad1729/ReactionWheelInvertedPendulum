/*
 * can.c
 *
 *  Created on: Oct 17, 2024
 */
#include "can.h"

volatile uint8_t can_message_pending = 0;

struct MotorState2 motor_state2;

void CAN_INIT(void)
{
	//CAN Tx: General Purpose Alternate function Push Pull, CAN1 Tx: PA12
	//CAN Rx: Input floating / Input Pull Up, CAN1 Rx: PA11
	if (!(RCC->APB2ENR & RCC_APB2ENR_IOPAEN))
	{
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	}

	//Enable CAN1 Clock
	if(!(RCC->APB1ENR & RCC_APB1ENR_CAN1EN))
	{
		RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;
	}

	GPIOA->CRH &= ~(GPIO_CRH_CNF11 | GPIO_CRH_MODE11 | GPIO_CRH_CNF12 | GPIO_CRH_MODE12);
	GPIOA->CRH |= GPIO_CRH_CNF11_0; //Input Floating
	GPIOA->CRH |= (GPIO_CRH_CNF12_1 | GPIO_CRH_MODE12_0); //AF push pull, Output, 10 MHz

	CAN1->MCR &= ~CAN_MCR_SLEEP;

	//Send Initialization request
	CAN1->MCR |= CAN_MCR_INRQ;
	//Wait till we get into initialization is complete
	while(!(CAN1->MSR & CAN_MSR_INAK));

	//Loop Back Mode
	//CAN1->BTR |= CAN_BTR_LBKM;

	//Set Bit Rate Pre-scalar to 8
	CAN1->BTR &= ~(CAN_BTR_BRP);
	CAN1->BTR |= (8 << CAN_BTR_BRP_Pos);
	//Set TS1 to 5 and TS2 to 0
	CAN1->BTR &= ~((CAN_BTR_TS1) | (CAN_BTR_TS2));
	CAN1->BTR |= ((5 << CAN_BTR_TS1_Pos) | (0 << CAN_BTR_TS2_Pos));
	//Set SJW to 0
	CAN1->BTR &= ~(CAN_BTR_SJW);
	//CAN1->BTR |= CAN_BTR_SJW_0;

	// Configure filters
	CAN1->FMR |= CAN_FMR_FINIT;   // Enter filter initialization mode
	CAN1->FM1R = 0;               // Mask mode for all filters
	CAN1->FS1R = 0x1;             // 32-bit scale for filter 0
	CAN1->FFA1R = 0;              // All filters assigned to FIFO 0
	CAN1->FA1R = 0x1;             // Enable filter 0

	// Configure Filter 0 to accept all messages
	CAN1->sFilterRegister[0].FR1 = 0;
	CAN1->sFilterRegister[0].FR2 = 0;

	CAN1->FMR &= ~CAN_FMR_FINIT;  // Exit filter initialization mode

	//Leave the initialization mode
	CAN1->MCR &= ~(CAN_MCR_INRQ);
	//Wait till we are out of initialization mode
	while(CAN1->MSR & CAN_MSR_INAK);

	//Enable FIFO 0 Pending Interrupt
	CAN1->IER |= (CAN_IER_FMPIE0);

	//Set Priority
	NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 2);

	//Enable Interrupt
	NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
	//NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
}


void CAN_TRANSMIT(uint8_t *data, uint16_t ID)
{
	if (CAN1->TSR & CAN_TSR_TME0)
	{
		//Configure standard id, no extended frame and not a remote frame
		CAN1->sTxMailBox[0].TIR = ID << CAN_TI0R_STID_Pos;
		//Number of bytes we are sending is 8
		CAN1->sTxMailBox[0].TDTR = 8 << CAN_TDT0R_DLC_Pos;
		//CAN_TDLxR
		CAN1->sTxMailBox[0].TDLR  = data[0] << 0; //Command Byte
		CAN1->sTxMailBox[0].TDLR |= data[1] << 8;
		CAN1->sTxMailBox[0].TDLR |= data[2] << 16;
		CAN1->sTxMailBox[0].TDLR |= data[3] << 24;
		//CAN_TDHxR
		CAN1->sTxMailBox[0].TDHR  = data[4] << 0;
		CAN1->sTxMailBox[0].TDHR |= data[5] << 8;
		CAN1->sTxMailBox[0].TDHR |= data[6] << 16;
		CAN1->sTxMailBox[0].TDHR |= data[7] << 24;
		//Request Transmission
		CAN1->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
	}
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
	//Check for pending messages
	//Last two bits in RFxR indicates how many messages are pending (e.g. 0, 1, 2, 3)
	//If pending messages are non zero, we need to handle them
	if (CAN1->RF0R & CAN_RF0R_FMP0)
	{
		if (((CAN1->sFIFOMailBox[0].RDLR >> 0) & 0xFF) == MotorState2_Command)
		{
			//motor_state2.command_byte = (CAN1->sFIFOMailBox[0].RDLR >> 0) & 0xFF;
			//motor_state2.temperature = (CAN1->sFIFOMailBox[0].RDLR >> 8) & 0xFF;
			//motor_state2.TorqueCurrent[0] = (CAN1->sFIFOMailBox[0].RDLR >> 16) & 0xFF;
			//motor_state2.TorqueCurrent[1] = (CAN1->sFIFOMailBox[0].RDLR >> 24) & 0xFF;
			wSpeed = CAN1->sFIFOMailBox[0].RDHR & 0xFFFF;
			//TX_DATA[3] = (CAN1->sFIFOMailBox[0].RDHR >> 0) & 0xFF;
			//TX_DATA[4] = (CAN1->sFIFOMailBox[0].RDHR >> 8) & 0xFF;
			TX_DATA[3] = (wSpeed >> 0) & 0xFF;
			TX_DATA[4] = (wSpeed >> 8) & 0xFF;
			//motor_state2.EncoderPosition[0] = (CAN1->sFIFOMailBox[0].RDHR >> 16) & 0xFF;
			//motor_state2.EncoderPosition[1] = (CAN1->sFIFOMailBox[0].RDHR >> 24) & 0xFF;
			can_message_pending = 1;
		}
		//Release FIFO
		CAN1->RF0R |= CAN_RF0R_RFOM0;
	}
}

void USB_HP_CAN1_TX_IRQHandler(void) {
    // Check if the transmit mailbox is empty (message has been sent)
    if (CAN1->TSR & CAN_TSR_RQCP0) {  // Request Completed for mailbox 0
        CAN1->TSR |= CAN_TSR_RQCP0;   // Clear the request complete flag for mailbox 0

        // Additional handling code (e.g., signal successful transmission)
    } else if (CAN1->TSR & CAN_TSR_RQCP1) {  // Request Completed for mailbox 1
        CAN1->TSR |= CAN_TSR_RQCP1;   // Clear the request complete flag for mailbox 1

        // Additional handling code
    } else if (CAN1->TSR & CAN_TSR_RQCP2) {  // Request Completed for mailbox 2
        CAN1->TSR |= CAN_TSR_RQCP2;   // Clear the request complete flag for mailbox 2

        // Additional handling code
    }
}
