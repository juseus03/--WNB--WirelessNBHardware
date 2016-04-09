/*
 * cola.h
 *
 * Created: 13/11/2012 11:21:58 p.m.
 *  Author: Usuario
 */ 


#ifndef COLA_H_
#define COLA_H_

typedef struct
{
	char cont_in;
	char cont_out;
	char cont_datos;
	char max;
	char *cola;
}cola_control;

void cola_init(	volatile cola_control *cola_ctrl,
				char max,
				char *cola);
				
char cola_inserte(volatile cola_control *cola_ctrl,
				char dato);

char cola_retire(volatile cola_control *cola_ctrl,
				char *dato);
				
char cola_llena(volatile cola_control *cola_ctrl);

char cola_vacia(volatile cola_control *cola_ctrl);

#endif /* COLA_H_ */