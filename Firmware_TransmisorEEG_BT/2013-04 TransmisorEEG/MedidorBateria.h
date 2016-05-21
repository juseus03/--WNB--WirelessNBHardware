/*
 * MedidorBateria.h
 *
 * Created: 01/04/2013 03:45:34 p.m.
 *  Author: Juan Manuel
 */ 


#ifndef MEDIDORBATERIA_H_
#define MEDIDORBATERIA_H_

//Pines
//Puerto B
//Señal activa baja indicando batería baja
#define MB_LB_Ind_bm PIN3_bm

//Banderas
#define MB_ESTADO_LED_bm 0x01
#define MB_BATERIABAJA_bm 0x02

typedef struct
{
	char banderas;
}MB_Control;

void MB_Init(volatile MB_Control *MB_Ctrl);

void MB_MedirBateria(volatile MB_Control *MB_Ctrl);



#endif /* MEDIDORBATERIA_H_ */