/*
 * Energia.c
 *
 * Created: 08/04/2013 01:07:16 p.m.
 *  Author: Juan Manuel
 */ 
#include "Energia.h"
#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "ADS1298.h"
#include "to_tcc0_delays.h"

extern volatile ADS_Control ADS_Ctrl;

extern volatile PW_Control PW_Ctrl;

extern volatile to_tcc0_delays_control to_c0_ctrl;

void PW_init(volatile PW_Control *PW_Ctrl,
			char Sleep_mode)
{
	//Interrupciones tipo 0 habilitadas para el puerto B
	PORTB.INTCTRL |= (3<<PORT_INT0LVL_gp);
	PORTB.INT0MASK |= PW_PUSHBUTTON_bm;
	//Sense Rising edge
	PORTB.PIN2CTRL |= 0b00000001;
		
	//Indique estado en que quederá el dispositivo
	PW_Ctrl->modo = PW_MODO_SLEEP;
	//Empiece en modo sleep;
	//Configure el modo de ahorro de energía
	SLEEP.CTRL |= Sleep_mode;	
	//Habilite el sleep;
	sleep_enable();
	//INTERRUPCIONES DEBEN ESTAR ENCENDIDAS
	//vaya a modo sleep;
	sleep_cpu();
}


ISR(PORTB_INT0_vect)
{
	//Anti-rebote:
	//Inhabilite las interrupciones del puerto
	PORTB.INTCTRL &= ~(3<<PORT_INT0LVL_gp);
	To_tcc0_InitTo(&to_c0_ctrl,0x1e,0x84,7);
	
		
	switch(PW_Ctrl.modo)
	{
		//Debe despertar
		case PW_MODO_SLEEP:
			sleep_disable();
			PW_Ctrl.modo = PW_MODO_ACTIVO;
		break;
		
		//Debe detener Adquisiciones y dormir
		case PW_MODO_ACTIVO:
			wdt_enable(WDTO_15MS);
		break;
	}
}