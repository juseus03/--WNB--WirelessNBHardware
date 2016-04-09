/*
 * to_tcd0_delays.h
 *
 * Created: 06/06/2012 10:05:54 a.m.
 *  Author: Juan Manuel Lopez
 */ 


#ifndef TO_TCD0_DELAYS_H_
#define TO_TCD0_DELAYS_H_

#define TO_TCD0_ACTIVO_bm 0x01U
#define TO_TCD0_FIN_bm 0x02U
#define TO_TCD0_OVR_bm 0x04U //Si se escriben los valores de perh y perl

typedef struct
{
	char banderas;
}to_tcd0_delays_control;

void To_tcd0_Init(volatile to_tcd0_delays_control *to_d0_ctrl);

void To_tcd0_InitTo(volatile to_tcd0_delays_control *to_d0_ctrl,
					char perh,
					char perl,
					char presc);

void To_tcd0_ReInitTo(volatile to_tcd0_delays_control *to_d0_ctrl,
					char perh,
					char perl,
					char presc);


//Verifica que se haya cumplido un To
char To_tcd0_HuboFin(volatile to_tcd0_delays_control *to_d0_ctrl);

//Baja la bandera de fin de To
void To_tcd0_BajeFin(volatile to_tcd0_delays_control *to_d0_ctrl);

//Verifica que un To esté activo
char To_tcd0_Activo(volatile to_tcd0_delays_control *to_d0_ctrl);

//Entrega el valor del contador del To y devuelve 1 si está activo y 0 si está inactivo
char To_tcd0_Contador(volatile to_tcd0_delays_control *to_d0_ctrl, 
					char *contador);

void To_tcd0_DetengaTo(volatile to_tcd0_delays_control *to_d0_ctrl);

#endif /* TO_TCC0_DELAYS_H_ */