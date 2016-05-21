/*
 * MedidorBateria.c
 *
 * Created: 01/04/2013 03:45:23 p.m.
 *  Author: Juan Manuel
 */ 

#include <avr/io.h>
#include "MedidorBateria.h"
#include "LEDs.h"

void MB_Init(volatile MB_Control *MB_Ctrl)
{
	MB_Ctrl->banderas = 0;
}

void MB_MedirBateria(volatile MB_Control *MB_Ctrl)
{
	if (MB_Ctrl->banderas & MB_BATERIABAJA_bm)
	{
		//if ()
		//{
		//}
	}
	
	//Verifique indicador de batería baja
	if (~(PORTB.IN & MB_LB_Ind_bm))
	{
		
	}
}