/*
 * Energia.h
 *
 * Created: 08/04/2013 01:07:28 p.m.
 *  Author: Juan Manuel
 */ 


#ifndef ENERGIA_H_
#define ENERGIA_H_

//Puerto B - pin 2 para el pulsador
#define PW_PUSHBUTTON_bm PIN2_bm

//Modos de energía
#define PW_MODO_SLEEP 1
#define PW_MODO_ACTIVO 2

//Modos de sueño
#define PW_IDLE (0 << 1)
#define PW_PDOWN (2 << 1)
#define PW_PSAVE (3 << 1)
#define PW_STDBY (6 << 1)
#define PW_ESTDBY (7 << 1)

typedef struct
{
	char modo;	
	char banderas;
}PW_Control;

void PW_init(volatile PW_Control *PW_Ctrl,
			char Sleep_mode);

void PW_Sensar(volatile PW_Control *PW_Ctrl);




#endif /* ENERGIA_H_ */