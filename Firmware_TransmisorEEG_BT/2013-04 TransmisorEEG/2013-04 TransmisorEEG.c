/*
 * _2013_04_TransmisorEEG.c
 *
 * Created: 01/04/2013 03:43:58 p.m.
 *  Author: Juan Manuel
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 32000000UL
#include <util/delay.h>

#include "FuncUtiles.h"

#include "cola.h"
#define COLA_MAX 27
char cola1[COLA_MAX];
char cola2[COLA_MAX];
volatile cola_control cola_ctrl1;
volatile cola_control cola_ctrl2;
volatile char contador;

/************************************************************************/
/* Comunicación por RF  
#include "nRF24L01plus.h"
volatile RF_Control BT_ctrl;                                                                   */
/************************************************************************/
#include "BT_KC21.h"
volatile BT_Control BT_ctrl;

#include "to_tcc0_delays.h"
volatile to_tcc0_delays_control to_c0_ctrl;

#include "to_tcd0_delays.h"
volatile to_tcd0_delays_control to_d0_ctrl;

//#include "LlenadoColas.h"
//volatile Llenado_Control Ll_ctrl;

#include "ADS1298.h"
volatile ADS_Control ADS_Ctrl;

#include "LEDs.h"

#include "Energia.h"
volatile PW_Control PW_Ctrl;

volatile char TEST_TEST;


int main(void)
{
	Init32MHz();
	HabiliteInt(7);
	sei();
	
	PORTB.OUTCLR = (LED_AZUL_bm|LED_VERDE_bm);
	PORTB.DIRSET = (LED_VERDE_bm|LED_AZUL_bm);
	
	PW_init(&PW_Ctrl,PW_PDOWN);
	
	PORTB.OUTSET = LED_VERDE_bm;
	
	To_tcd0_Init(&to_d0_ctrl);
	To_tcc0_Init(&to_c0_ctrl);
	
	cola_init(&cola_ctrl1,COLA_MAX,&cola1);
	cola_init(&cola_ctrl2,COLA_MAX,&cola2);
	
	BT_Init(&BT_ctrl); 
	ADS_Init(&ADS_Ctrl, ADS_ENERGIA_LOWPOWER, ADS_DATARATE_LP_250); //Funciona

	
	ADS_ConfigChannels(&ADS_Ctrl,  ADS_MODO_CONFIG_NORMAL, ADS_GANANCIA_12);
	//contador=1;
	//int j=1;
	
	while(1)
	{	
		
		ADS_Adquiera(&ADS_Ctrl);
		
		//int i; /Pruebas sin ADS----------------------------------------------
		//
		//if(cola_vacia(&cola_ctrl1))
		//{
			//_delay_ms(500);
			//for(i=1;i<=COLA_MAX;i++)
			//{
				//cola_inserte(&cola_ctrl1,0x40+j);
			//}
			//j++;
		//}
				
		BT_Transmitir(&BT_ctrl); //Definicion pines y adapatar a bluetooth
	}
}

