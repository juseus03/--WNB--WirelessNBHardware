/*
 * FuncUtiles.c
 *
 * Created: 13/11/2012 09:49:34 a.m.
 *  Author: Usuario
 */ 

#include <avr/io.h>
#include "FuncUtiles.h"

//Habilita oscilador interno de 32MHz
void Init32MHz()
{
	OSC.CTRL = OSC_RC32MEN_bm;       // enable 32MHz oscillator
    while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for oscillator to be ready
    CCP = CCP_IOREG_gc;              // disable register security for clock update
    CLK.CTRL = CLK_SCLKSEL_RC32M_gc; // switch to 32MHz clock
	OSC.CTRL &=(~OSC_RC2MEN_bm); 
}

//Habilita interrupciones del nivel seleccionado
void HabiliteInt(char nivel)
{
	PMIC.CTRL |= nivel;
}

//Inhabilita interrupciones del nivel seleccionado
void InhabiliteInt(char nivel)
{
	PMIC.CTRL &= ~(nivel);
}