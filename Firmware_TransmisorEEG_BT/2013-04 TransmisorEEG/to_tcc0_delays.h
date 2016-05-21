/*
 * to_tcc0_delays.h
 *
 * Created: 06/06/2012 10:05:54 a.m.
 *  Author: Juan Manuel Lopez
 */ 


#ifndef TO_TCC0_DELAYS_H_
#define TO_TCC0_DELAYS_H_

#define TO_TCC0_ACTIVO_bm 0x01U
#define TO_TCC0_FIN_bm 0x02U
#define TO_TCC0_OVR_bm 0x04U
//Si se escriben los valores de perh y perl

typedef struct
{
	char banderas;
}to_tcc0_delays_control;

void To_tcc0_Init(to_tcc0_delays_control *to_ctrl);

void To_tcc0_InitTo(to_tcc0_delays_control *to_ctrl,
					char perh,
					char perl,
					char presc);

void To_tcc0_ReInitTo(to_tcc0_delays_control *to_ctrl,
					char perh,
					char perl,
					char presc);


//Verifica que se haya cumplido un To
char To_tcc0_HuboFin(to_tcc0_delays_control *to_ctrl);

//Baja la bandera de fin de To
void To_tcc0_BajeFin(to_tcc0_delays_control *to_ctrl);

//Verifica que un To esté activo
char To_tcc0_Activo(to_tcc0_delays_control *to_ctrl);

//Entrega el valor del contador del To y devuelve 1 si está activo y 0 si está inactivo
char To_tcc0_Contador(to_tcc0_delays_control *to_ctrl, 
					char *contador);

void To_tcc0_DetengaTo(to_tcc0_delays_control *to_ctrl);

#endif /* TO_TCC0_DELAYS_H_ */