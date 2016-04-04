/*
 * BlinkingLedTarjeta.c
 *
 * Created: 2/04/2016 10:27:48 a. m.
 * Author : Juan Useche
 */ 

#include <avr/io.h>
#define F_CPU 32000000
#include <util/delay.h>
#include "FuncUtiles.h"

int main(void)
{	Init32MHz();
	PORTB.DIRSET=(PIN1_bm|PIN0_bm);
	PORTB.OUTSET=PIN1_bm;
    /* Replace with your application code */
    while (1) 
    {
		PORTB.OUTTGL=(PIN1_bm|PIN0_bm);
		_delay_ms(500);
    }
}

