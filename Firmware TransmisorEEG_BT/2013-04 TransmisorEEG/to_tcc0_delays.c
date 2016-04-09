/*
 * to_tcc0_delays.c
 *
 * Created: 06/06/2012 10:05:36 a.m.
 *  Author: Juan Manuel Lopez
 */ 

/*
 * Para utilizar este módulo se deben habilitar las interrupciones
 * Se requiere una estructura de control de tipo to_tcc0_delays_control
 */

#include <avr/io.h>
#include "to_tcc0_delays.h"
#include <avr/interrupt.h>

extern to_tcc0_delays_control to_c0_ctrl;

ISR(TCC0_OVF_vect)
{
	TCC0.INTCTRLA &= ~(3); //deshabilite interrupciones de tcc0
	TCC0.CTRLA = 0; //detenga el timer
	TCC0.CTRLFSET = (3 <<3); //reinicie el contador
	to_c0_ctrl.banderas = TO_TCC0_FIN_bm; //indique que hubo un fin de conteo

	//Limpie bandera del puerto B para interrupciones del pulsador
	PORTB.INTFLAGS |= 0x01;	
	//Habilite interrupciones del puerto B para el pulsador
	PORTB.INTCTRL |= (3<<PORT_INT0LVL_gp);
	
}

void To_tcc0_Init(to_tcc0_delays_control *to_ctrl)
{
	to_ctrl->banderas = 0;
}

void To_tcc0_InitTo(to_tcc0_delays_control *to_ctrl,
					char perh,
					char perl,
					char presc)
{
	if (to_ctrl->banderas & TO_TCC0_ACTIVO_bm)
		to_ctrl->banderas |= TO_TCC0_OVR_bm; //Sobreescritura, sin reinicio
	
	to_ctrl->banderas |= TO_TCC0_ACTIVO_bm;	
	to_ctrl->banderas &= ~(TO_TCC0_FIN_bm);
	TCC0.CTRLFSET = (3 <<3); //reinicie el contador
	TCC0.PERL = perl;
	TCC0.PERH = perh;
	TCC0.INTCTRLA |= 3;
	TCC0.CTRLA |= (presc & 0x0f); 
}


void To_tcc0_ReInitTo(to_tcc0_delays_control *to_ctrl,
					char perh,
					char perl,
					char presc)
{
	TCC0.CTRLA = 0; //detenga el timer
	//en caso de que se haya ejecutado una interrupción antes de detener
	to_ctrl->banderas |= TO_TCC0_ACTIVO_bm;	
	TCC0.CTRLFSET = (3 <<3); //reinicie el contador
	TCC0.PERH = perh;
	TCC0.PERL = perl;
	TCC0.CTRLA |= (presc & 0x0f); 
}

char To_tcc0_HuboFin(to_tcc0_delays_control *to_ctrl)
{
	return(to_ctrl->banderas & TO_TCC0_FIN_bm);
}

void To_tcc0_BajeFin(to_tcc0_delays_control *to_ctrl)
{
	to_ctrl->banderas &= ~(TO_TCC0_FIN_bm);
}

char To_tcc0_Activo(to_tcc0_delays_control *to_ctrl)
{
	return(to_ctrl->banderas & TO_TCC0_ACTIVO_bm);
}

char To_tcc0_Contador(to_tcc0_delays_control *to_ctrl, 
					char *contador)
{
	char *p = contador;
	*p++ = TCC0.CNTL;
	*p = TCC0.CNTH;
	if (to_ctrl->banderas & TO_TCC0_ACTIVO_bm)
		return (1);
	
	return (0);
}

void To_tcc0_DetengaTo(to_tcc0_delays_control *to_ctrl)
{
	TCC0.INTCTRLA &= ~(3); //deshabilite interrupciones de tcc0
	TCC0.CTRLA = 0; //detenga el timer
	TCC0.CTRLFSET = (3 <<3); //reinicie el contador
	to_ctrl->banderas = 0; //Limpie banderas
}