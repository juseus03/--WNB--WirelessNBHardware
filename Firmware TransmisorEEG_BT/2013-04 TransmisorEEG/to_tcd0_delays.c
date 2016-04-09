/*
 * to_tcd0_delays.c
 *
 * Created: 06/06/2012 10:05:36 a.m.
 *  Author: Juan Manuel Lopez
 */ 

/*
 * Para utilizar este módulo se deben habilitar las interrupciones
 * Se requiere una estructura de control de tipo to_tcc0_delays_control
 */

#include <avr/io.h>
#include "to_tcd0_delays.h"
#include <avr/interrupt.h>

extern to_tcd0_delays_control to_d0_ctrl;

ISR(TCD0_OVF_vect)
{
	TCD0.INTCTRLA &= ~(3); //deshabilite interrupciones de tcc0
	TCD0.CTRLA = 0; //detenga el timer
	TCD0.CTRLFSET = (3 <<3); //reinicie el contador
	to_d0_ctrl.banderas = TO_TCD0_FIN_bm; //indique que hubo un fin de conteo
}

void To_tcd0_Init(volatile to_tcd0_delays_control *to_d0_ctrl)
{
	to_d0_ctrl->banderas = 0;
}

void To_tcd0_InitTo(volatile to_tcd0_delays_control *to_d0_ctrl,
					char perh,
					char perl,
					char presc)
{
	if (to_d0_ctrl->banderas & TO_TCD0_ACTIVO_bm)
		to_d0_ctrl->banderas |= TO_TCD0_OVR_bm; //Sobreescritura, sin reinicio
	
	to_d0_ctrl->banderas |= TO_TCD0_ACTIVO_bm;	
	to_d0_ctrl->banderas &= ~(TO_TCD0_FIN_bm);
	TCD0.CTRLFSET = (3 <<3); //reinicie el contador
	TCD0.PERL = perl;
	TCD0.PERH = perh;
	TCD0.INTCTRLA |= 3;
	TCD0.CTRLA |= (presc & 0x0f); 
}


void To_tcd0_ReInitTo(volatile to_tcd0_delays_control *to_d0_ctrl,
					char perh,
					char perl,
					char presc)
{
	TCD0.CTRLA = 0; //detenga el timer
	//en caso de que se haya ejecutado una interrupción antes de detener
	to_d0_ctrl->banderas |= TO_TCD0_ACTIVO_bm;	
	TCD0.CTRLFSET = (3 <<3); //reinicie el contador
	TCD0.PERH = perh;
	TCD0.PERL = perl;
	TCD0.CTRLA |= (presc & 0x0f); 
}

char To_tcd0_HuboFin(volatile to_tcd0_delays_control *to_d0_ctrl)
{
	return(to_d0_ctrl->banderas & TO_TCD0_FIN_bm);
}

void To_tcd0_BajeFin(volatile to_tcd0_delays_control *to_d0_ctrl)
{
	to_d0_ctrl->banderas &= ~(TO_TCD0_FIN_bm);
}

char To_tcd0_Activo(volatile to_tcd0_delays_control *to_d0_ctrl)
{
	return(to_d0_ctrl->banderas & TO_TCD0_ACTIVO_bm);
}

char To_tcd0_Contador(volatile to_tcd0_delays_control *to_d0_ctrl, 
					char *contador)
{
	char *p = contador;
	*p++ = TCD0.CNTL;
	*p = TCD0.CNTH;
	if (to_d0_ctrl->banderas & TO_TCD0_ACTIVO_bm)
		return (1);
	
	return (0);
}

void To_tcd0_DetengaTo(volatile to_tcd0_delays_control *to_d0_ctrl)
{
	TCD0.INTCTRLA &= ~(3); //deshabilite interrupciones de tcc0
	TCD0.CTRLA = 0; //detenga el timer
	TCD0.CTRLFSET = (3 <<3); //reinicie el contador
	to_d0_ctrl->banderas = 0; //Limpie banderas
}