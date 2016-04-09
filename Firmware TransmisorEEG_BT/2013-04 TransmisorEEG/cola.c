/*
 * cola.c
 *
 * Created: 13/11/2012 11:22:18 p.m.
 *  Author: Usuario
 */ 

#include "cola.h"

void cola_init(volatile cola_control *cola_ctrl, 
				char max, 
				char *cola)
{
	cola_ctrl->cont_in = 0;
	cola_ctrl->cont_out = 0;
	cola_ctrl->cont_datos = 0;
	cola_ctrl->max = max;
	cola_ctrl->cola = cola;
}

char cola_inserte(volatile cola_control *cola_ctrl, 
				char dato)
{
	if (cola_ctrl->cont_datos == cola_ctrl->max)
		return(0);
	
	cola_ctrl->cola[cola_ctrl->cont_in] = dato;
	(cola_ctrl->cont_in)++;
	
	if (cola_ctrl->cont_in == cola_ctrl->max)
		cola_ctrl->cont_in = 0;
		
	(cola_ctrl->cont_datos)++;
	return(1);
}

char cola_retire(volatile cola_control *cola_ctrl, 
				char *dato)
{
	if(!(cola_ctrl->cont_datos))
		return (0);
		
	*dato = cola_ctrl->cola[cola_ctrl->cont_out];
	(cola_ctrl->cont_out)++;
	
	if(cola_ctrl->cont_out == cola_ctrl->max)
		cola_ctrl->cont_out = 0;
	
	(cola_ctrl->cont_datos)--;
	return(1);
}

char cola_llena(volatile cola_control *cola_ctrl)
{
	if (cola_ctrl->cont_datos == cola_ctrl->max)
	{
		return(1);
	}		
	else
	{
		return(0);	
	}	
}

char cola_vacia(volatile cola_control *cola_ctrl)
{
	if(!(cola_ctrl->cont_datos))
	{
		return(1);
	}		
	else
	{
		return(0);	
	}
	
}