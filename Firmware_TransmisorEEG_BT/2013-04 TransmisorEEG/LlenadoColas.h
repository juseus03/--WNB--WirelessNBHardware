/*
 * LlenadoColas.h
 *
 * Created: 18/01/2013 11:25:56 a.m.
 *  Author: Juan Manuel
 */ 


#ifndef LLENADOCOLAS_H_
#define LLENADOCOLAS_H_

#define Ll_Estado_BuscarVacio 1
#define Ll_Estado_Llenando 2

typedef struct
{
	char cola_activa;
	char estado;
}Llenado_Control;

void LLenadoInit(Llenado_Control *Ll_ctrl);

void LlenadoCola(Llenado_Control *Ll_ctrl);

void LlenadoBuscar(Llenado_Control *Ll_ctrl);


#endif /* LLENADOCOLAS_H_ */