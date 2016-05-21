/*
 * LlenadoColas.c
 *
 * Created: 18/01/2013 11:25:37 a.m.
 *  Author: Juan Manuel
 */ 

#include "LlenadoColas.h"
#include "cola.h"
#include "nRF24L01plus.h"

extern volatile cola_control cola_ctrl1;
extern volatile cola_control cola_ctrl2;
extern volatile RF_Control RF_ctrl;

void LLenadoInit(Llenado_Control *Ll_ctrl)
{
	Ll_ctrl->estado = Ll_Estado_BuscarVacio;
	Ll_ctrl->cola_activa = 0;
}

void LlenadoCola(Llenado_Control *Ll_ctrl)
{
	switch(Ll_ctrl->estado)
	{
		case Ll_Estado_BuscarVacio:
			LlenadoBuscar(Ll_ctrl);
			if (Ll_ctrl->cola_activa)
			{
				Ll_ctrl->estado = Ll_Estado_Llenando;
			}
		break;
		
		case Ll_Estado_Llenando:
			if (Ll_ctrl->cola_activa == 1)
			{
				if (!cola_inserte(&cola_ctrl1,0x42))
				{
					Ll_ctrl->estado = Ll_Estado_BuscarVacio;
				}
			}
			else
			{
				if (!cola_inserte(&cola_ctrl2,0x41))
				{
					Ll_ctrl->estado = Ll_Estado_BuscarVacio;
				}
				
			}
		break;
	}
}

void LlenadoBuscar(Llenado_Control *Ll_ctrl)
{
	if (cola_vacia(&cola_ctrl1) && ((RF_ctrl.cola_activa == 2)))
	{
		Ll_ctrl->cola_activa = 1;
	}
	else if (cola_vacia(&cola_ctrl2) && ((RF_ctrl.cola_activa == 1)||(RF_ctrl.cola_activa == 0)))
	{
		Ll_ctrl->cola_activa = 2;
	}
	else
	{
		Ll_ctrl->cola_activa = 0;
	}
	
}