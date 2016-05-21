#include <avr/io.h>
#include <avr/interrupt.h>
#include "BT_KC21.h"
#include "cola.h"

volatile extern BT_Control BT_ctrl;
volatile extern cola_control cola_ctrl1;
volatile extern cola_control cola_ctrl2;

void setupSerial()
{	
	PORTC_DIRSET=PIN3_bm;
	PORTC_DIRCLR=PIN2_bm;
	
	USARTC0_BAUDCTRLA = 123;//123;//53; 
	USARTC0_BAUDCTRLB = 0b00001100<<4; //Just to be sure that BSCALE is -4//0b00000110<<4 //0b11000000
	
	
	//Disable interrupts, just for safety
	USARTC0_CTRLA = USART_INTERRUPT;
	//8 data bits, no parity and 1 stop bit
	USARTC0_CTRLC = USART_CHSIZE_8BIT_gc;
	
	//Enable receive and transmit
	USARTC0_CTRLB = USART_TXEN_bm | USART_RXEN_bm; // And enable high speed mode
	
}

void sendChar(char c)
{
	
	if( (USARTC0_STATUS & USART_DREIF_bm) ){
		USARTC0.DATA = c;
	}
	
	
}

void sendString(char *text)
{
	while(*text)
	{
		sendChar(*text++);
	}
}

void BT_Init(volatile BT_Control *BT_ctrl)
{
	setupSerial();
	BT_ctrl->estado = BT_ESTADO_TX_HAYDATOS;
	
}

void BT_Transmitir(volatile BT_Control *BT_ctrl)
{

	/************************************************************************/
	/* Dos estados
	Enviar datos
	esperando datos
	                                                                     */
	/************************************************************************/

	switch (BT_ctrl->estado)
	{
		case BT_ESTADO_TX_HAYDATOS:
			
			if (cola_llena(&cola_ctrl1))
			{
				BT_ctrl->cola_activa = 1;
				BT_ctrl->banderas |= BT_HAYDATOS_bm;
			}
			else if (cola_llena(&cola_ctrl2))
			{
				BT_ctrl->cola_activa = 1;
				BT_ctrl->banderas |= BT_HAYDATOS_bm;
			}
			
			if (BT_ctrl->banderas & BT_HAYDATOS_bm)
			{
				BT_ctrl->estado = BT_ESTADO_TX_CARGANDOPAYLOAD;
				//Indicar que se empezó la transmisión
				BT_ctrl->banderas |= BT_TRANSMITIENDO_bm;
				//habilite interrupciones de TXC y de DRE
				USARTC0.CTRLA |= ((3<<USART_TXCINTLVL_gp)|(3<<USART_DREINTLVL_gp));
			}

		break;
		
		case BT_ESTADO_TX_CARGANDOPAYLOAD:
			
			if (!(BT_ctrl->banderas & BT_TRANSMITIENDO_bm))
			{
				BT_ctrl->banderas &= (~BT_HAYDATOS_bm);
				BT_ctrl->estado = BT_ESTADO_TX_HAYDATOS;			
			}
		break;	
	}
}


ISR(USARTC0_DRE_vect)
{
	char dato;
	if (BT_ctrl.cola_activa == 1)
	{
		if (cola_retire(&cola_ctrl1,&dato))
		{
			USARTC0.DATA = dato;
			
		}
		else
		{
			//Apago interrupción
			USARTC0.CTRLA &= ~(3<<USART_DREINTLVL_gp);
		}
	}
	else
	{
		if (cola_retire(&cola_ctrl2,&dato))
		{
			USARTC0.DATA = dato;
			
		}
		else
		{
			//Apago interrupción
			USARTC0.CTRLA &= ~(3<<USART_DREINTLVL_gp);
		}
	}
}

ISR(USARTC0_TXC_vect)
{
	//Indicar que finalizó transmisión de una muestra
	BT_ctrl.banderas &= ~(BT_TRANSMITIENDO_bm);
}