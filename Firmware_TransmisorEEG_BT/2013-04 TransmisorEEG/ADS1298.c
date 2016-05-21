/*
 * ADS1298.c
 *
 * Created: 31/01/2013 07:33:49 p.m.
 *  Author: Juan Manuel
 */ 


/* PINES UTILIZADOS Y EQUIVALENCIA
PUERTO C:
ADS1298		PIN			DIRECCIÓN
-------------------------------------
CS			PC4			OUT
SCK			PC5			OUT
DOUT		PC6/MISO	IN
DIN			PC7/MOSI	OUT

PUERTO D:
ADS1298		PIN			DIRECCIÓN
-------------------------------------
START		PD2			OUT
PWD			PD3			OUT
RESET		PD4			OUT
DRDY		PD5			IN

PUERTO E:
ADS1298		PIN			DIRECCIÓN
-------------------------------------
CLK			PE0			IN

*/

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 32000000UL
#include <util/delay.h>
#include "ADS1298.h"
#include "nRF24L01plus.h"
#include "cola.h"
#include "LEDs.h"

volatile extern cola_control cola_ctrl1;
volatile extern cola_control cola_ctrl2;
volatile extern ADS_Control ADS_Ctrl;
volatile extern char TEST_TEST;
volatile extern char contador;

void ADS_Init(volatile ADS_Control *ADS_Ctrl,
		char powermode,
		char datarate)
{
	//Todas la banderas en 0
	ADS_Ctrl->banderas = 0;
	
	//Estado inicial 1
	ADS_Ctrl->estado = 1;
	
	//Configuración de los pines
	//puerto c:
	PORTC.OUTSET = PIN_ADS_CS_bm;
	PORTC.OUTCLR = (PIN_ADS_SCK_bm|PIN_ADS_DIN_bm);
	PORTC.DIRSET = (PIN_ADS_CS_bm|PIN_ADS_SCK_bm|PIN_ADS_DIN_bm);
	
	//CS es activo bajo
	PORTC.OUTSET = PIN_ADS_CS_bm;
	
	//puerto D:
	PORTD.OUTCLR = (PIN_ADS_START_bm|PIN_ADS_PWD_bm|PIN_ADS_RESET_bm);
	PORTD.DIRSET = (PIN_ADS_START_bm|PIN_ADS_PWD_bm|PIN_ADS_RESET_bm);
	
	//Configuración de USARTC1
	//MSB primero
	//Control de velocidad de transferencia
	//BSEL= fPER/(2fBAUD) - 1
	//1Mbps = 15
	//2Mbps = 7
	//16Mbps = 1
	USARTC1.BAUDCTRLA = 15;
	USARTC1.BAUDCTRLB = 0x00;
	
	//Modo Master SPI
	USARTC1.CTRLC |= 0b11000010;
	
	//Habilite transmisor
	USARTC1.CTRLB |= (USART_TXEN_bm);
	
	//Secuencia de encendido:
	
	//esperar 20us para oscilador estabable
	_delay_us(20);
	//En alto PWD y RESET
	PORTD.OUTSET = (PIN_ADS_PWD_bm|PIN_ADS_RESET_bm);
	//Esperar 32ms o 1 s
	_delay_ms(1000);
	//Pulso de reset
	PORTD.OUTCLR = PIN_ADS_RESET_bm;
	//Espere 2us
	_delay_us(2);
	//Fin pulso de reset
	PORTD.OUTSET = PIN_ADS_RESET_bm;
	//Espere 8.78 us antes de empezar las comunicaciones
	_delay_us(9);
	
	//Empiece comunicación
	//Interfaz serial activa:
	//PORTC.OUTCLR = PIN_ADS_CS_bm;
	
	//Detenga conversión Continua
	ADS_EnviarByte(ADS_Ctrl,ADS_SDATAC);
	while(!(ADS_Ctrl->banderas & ADS_BYTE_ENVIADO_bm));
	
	//Configuración del buffer de referencia - referencia de voltaje interna a 2.4V
	ADS_Ctrl->arreglo[0] = (ADS_WREG|ADS_REG_CONFIG3);
	ADS_Ctrl->arreglo[1] = 0x00;
	ADS_Ctrl->arreglo[2] = 0xc0;
	ADS_EnviarArreglo(ADS_Ctrl,3);
	//Espere a que se haya enviado el arreglo
	while (!(ADS_Ctrl->banderas & ADS_ARREGLO_ENVIADO_bm));
	
	//Configuración de frecuencia de muestreo y Modo de energía
	ADS_Ctrl->arreglo[0] = (ADS_WREG|ADS_REG_CONFIG1);
	ADS_Ctrl->arreglo[1] = 0x00;
	ADS_Ctrl->arreglo[2] = (powermode|datarate);
	//ADS_Ctrl->arreglo[2] = 0b00000110;
	ADS_EnviarArreglo(ADS_Ctrl,3);
	//Espere a que se haya enviado el arreglo
	while (!(ADS_Ctrl->banderas & ADS_ARREGLO_ENVIADO_bm));
}

void ADS_EnviarByte(volatile ADS_Control *ADS_Ctrl, 
					char comando)
{
	//Interface serial activa
	PORTC.OUTCLR = PIN_ADS_CS_bm;
	
	//baje bandera de byte enviado
	ADS_Ctrl->banderas &= ~(ADS_BYTE_ENVIADO_bm);
	
	//Modo de transmisión
	ADS_Ctrl->modo_com = ADS_MODO_COM_ENVIANDO_BYTE;
	
	//Cargar dato en registro
	USARTC1.DATA = comando;
	
	//Habilite interrupciones de txc
	USARTC1.CTRLA |= (3<<USART_TXCINTLVL_gp);
}

void ADS_EnviarArreglo(volatile ADS_Control *ADS_Ctrl,
						char datos_max )
{
	//Interface serial activa
	PORTC.OUTCLR = PIN_ADS_CS_bm;
	
	//Limpie bandera de envío de arreglo
	ADS_Ctrl->banderas &= ~(ADS_ARREGLO_ENVIADO_bm);
	
	//Ajuste modo de comunicación
	ADS_Ctrl->modo_com = ADS_MODO_COM_ENVIANDO_ARREGLO;
	
	//Fije el número de datos
	ADS_Ctrl->datos_max = datos_max;
	ADS_Ctrl->n_datos = 0;
	
	//envíe el primer dato
	USARTC1.DATA = ADS_Ctrl->arreglo[0];
	
	//Habilite interrupciones de TXC y de DRE
	USARTC1.CTRLA |= ((3<<USART_TXCINTLVL_gp)|(3<<USART_DREINTLVL_gp));
}

void ADS_ConfigChannels(volatile ADS_Control *ADS_Ctrl,
						char Config_modo,
						char Granancia)
{
	switch (Config_modo)
	{
		//Configurar canales en corto
		case ADS_MODO_CONFIG_CORTO:
			ADS_Ctrl->arreglo[0] = (ADS_WREG|ADS_REG_CONFIG2);
			ADS_Ctrl->arreglo[1] = 0x00;
			ADS_Ctrl->arreglo[2] = 0x00;
			ADS_EnviarArreglo(ADS_Ctrl,3);
			//Espere a que se haya enviado el arreglo
			while (!(ADS_Ctrl->banderas & ADS_ARREGLO_ENVIADO_bm));
			
			ADS_Ctrl->arreglo[0] = (ADS_WREG|ADS_REG_CH1SET);
			ADS_Ctrl->arreglo[1] = 0x07;
			ADS_Ctrl->arreglo[2] = (Granancia|ADS_MUX_CORTO);
			ADS_Ctrl->arreglo[3] = (Granancia|ADS_MUX_CORTO);
			ADS_Ctrl->arreglo[4] = (Granancia|ADS_MUX_CORTO);
			ADS_Ctrl->arreglo[5] = (Granancia|ADS_MUX_CORTO);
			ADS_Ctrl->arreglo[6] = (Granancia|ADS_MUX_CORTO);
			ADS_Ctrl->arreglo[7] = (Granancia|ADS_MUX_CORTO);
			ADS_Ctrl->arreglo[8] = (Granancia|ADS_MUX_CORTO);
			ADS_Ctrl->arreglo[9] = (Granancia|ADS_MUX_CORTO);
			ADS_EnviarArreglo(ADS_Ctrl,10);
			//Espere a que se haya enviado el arreglo
			while (!(ADS_Ctrl->banderas & ADS_ARREGLO_ENVIADO_bm));
			
		break;
		
		case ADS_MODO_CONFIG_TEST:
			ADS_Ctrl->arreglo[0] = (ADS_WREG|ADS_REG_CONFIG2);
			ADS_Ctrl->arreglo[1] = 0x00;
			//ADS_Ctrl->arreglo[2] = 0x14; //DobleAmplitud 2mv
			ADS_Ctrl->arreglo[2] = 0x10;
			ADS_EnviarArreglo(ADS_Ctrl,3);
			//Espere a que se haya enviado el arreglo
			while (!(ADS_Ctrl->banderas & ADS_ARREGLO_ENVIADO_bm));
			
			ADS_Ctrl->arreglo[0] = (ADS_WREG|ADS_REG_CH1SET);
			ADS_Ctrl->arreglo[1] = 0x07;
			ADS_Ctrl->arreglo[2] = (Granancia|ADS_MUX_TEST);
			ADS_Ctrl->arreglo[3] = (Granancia|ADS_MUX_TEST);
			ADS_Ctrl->arreglo[4] = (Granancia|ADS_MUX_TEST);
			ADS_Ctrl->arreglo[5] = (Granancia|ADS_MUX_TEST);
			ADS_Ctrl->arreglo[6] = (Granancia|ADS_MUX_TEST);
			ADS_Ctrl->arreglo[7] = (Granancia|ADS_MUX_TEST);
			ADS_Ctrl->arreglo[8] = (Granancia|ADS_MUX_TEST);
			ADS_Ctrl->arreglo[9] = (Granancia|ADS_MUX_TEST);	
			ADS_EnviarArreglo(ADS_Ctrl,10);
			//Espere a que se haya enviado el arreglo
			while (!(ADS_Ctrl->banderas & ADS_ARREGLO_ENVIADO_bm));
		break;
		
		case ADS_MODO_CONFIG_NORMAL:
			ADS_Ctrl->arreglo[0] = (ADS_WREG|ADS_REG_CONFIG2);
			ADS_Ctrl->arreglo[1] = 0x00;
			ADS_Ctrl->arreglo[2] = 0x10;
			//ADS_Ctrl->arreglo[2] = 0x00;
			ADS_EnviarArreglo(ADS_Ctrl,3);
			//Espere a que se haya enviado el arreglo
			while (!(ADS_Ctrl->banderas & ADS_ARREGLO_ENVIADO_bm));
			
			ADS_Ctrl->arreglo[0] = (ADS_WREG|ADS_REG_CH1SET);
			ADS_Ctrl->arreglo[1] = 0x07;
			ADS_Ctrl->arreglo[2] = (Granancia|ADS_MUX_TEST);
			ADS_Ctrl->arreglo[3] = (Granancia|ADS_MUX_TEST); 
			ADS_Ctrl->arreglo[4] = (Granancia|ADS_MUX_NORMAL);
			ADS_Ctrl->arreglo[5] = (Granancia|ADS_MUX_NORMAL);//lECTURA SCRIPT ANDRES
			ADS_Ctrl->arreglo[6] = (Granancia|ADS_MUX_NORMAL);
			ADS_Ctrl->arreglo[7] = (Granancia|ADS_MUX_NORMAL);
			ADS_Ctrl->arreglo[8] = (Granancia|ADS_MUX_CORTO);
			ADS_Ctrl->arreglo[9] = (Granancia|ADS_MUX_CORTO);

			ADS_EnviarArreglo(ADS_Ctrl,10);
			//Espere a que se haya enviado el arreglo
			while (!(ADS_Ctrl->banderas & ADS_ARREGLO_ENVIADO_bm));
		
		break;
		
		case ADS_MODO_CONFIG_OFF:
			ADS_Ctrl->arreglo[0] = (ADS_WREG|ADS_REG_CONFIG2);
			ADS_Ctrl->arreglo[1] = 0x00;
			ADS_Ctrl->arreglo[2] = 0x00;
			ADS_EnviarArreglo(ADS_Ctrl,3);
			//Espere a que se haya enviado el arreglo
			while (!(ADS_Ctrl->banderas & ADS_ARREGLO_ENVIADO_bm));
		
			ADS_Ctrl->arreglo[0] = (ADS_WREG|ADS_REG_CH1SET);
			ADS_Ctrl->arreglo[1] = 0x07;
			ADS_Ctrl->arreglo[2] = (Granancia|ADS_CHN_OFF);
			ADS_Ctrl->arreglo[3] = (Granancia|ADS_CHN_OFF);
			ADS_Ctrl->arreglo[4] = (Granancia|ADS_CHN_OFF);
			ADS_Ctrl->arreglo[5] = (Granancia|ADS_CHN_OFF);
			ADS_Ctrl->arreglo[6] = (Granancia|ADS_CHN_OFF);
			ADS_Ctrl->arreglo[7] = (Granancia|ADS_CHN_OFF);
			ADS_Ctrl->arreglo[8] = (Granancia|ADS_CHN_OFF);
			ADS_Ctrl->arreglo[9] = (Granancia|ADS_CHN_OFF);
			ADS_EnviarArreglo(ADS_Ctrl,10);
			//Espere a que se haya enviado el arreglo
			while (!(ADS_Ctrl->banderas & ADS_ARREGLO_ENVIADO_bm));
		
		break;
	}
	
	
	//Limpiar banderas de adquisición detenida
	ADS_Ctrl->banderas &= ~(ADS_ADQUISICION_DETENIDA_bm);
	
	//Empezar adquisición
	PORTD.OUTSET = PIN_ADS_START_bm;
	
	ADS_EnviarByte(ADS_Ctrl,ADS_RDATAC);
	//Espere a que se haya enviado el comando
	while (!(ADS_Ctrl->banderas & ADS_BYTE_ENVIADO_bm));
	
}

void ADS_Adquiera(volatile ADS_Control *ADS_Ctrl)
{
	if (ADS_Ctrl->banderas & ADS_ADQUISICION_DETENIDA_bm)
		return;
	
	switch(ADS_Ctrl->estado)
	{
		case ADS_ESTADO_ESPEREDRDY:
			//Verifique que hayan datos listos
			if(!(PORTD.IN & PIN_ADS_DRDY_bm))
			{
				//Busque una cola libre para la adquisición
				ADS_Ctrl->estado = ADS_ESTADO_BUSQUECOLA;	
			}
			else if(ADS_Ctrl->banderas & ADS_DETENGA_ADQUISICION_bm)
			{
				//Detener la adquisición
				ADS_Ctrl->banderas |= ADS_ADQUISICION_DETENIDA_bm;
					
				//Limpiar bandera de solicitud de detención
				ADS_Ctrl->banderas &= ~(ADS_DETENGA_ADQUISICION_bm);
			}
		break;
			
		case ADS_ESTADO_BUSQUECOLA:
			ADS_BusqueColaLibre(ADS_Ctrl);
			//if (ADS_Ctrl->cola_activa)
			//{
				ADS_Ctrl->estado = ADS_ESTADO_EMPIECE_ADQUISICION;
			//}
		break;
			
		case ADS_ESTADO_EMPIECE_ADQUISICION:
			//Active receptor USART
			USARTC1.CTRLB |= (USART_RXEN_bm);
				
			//Inicie interfaz
			PORTC.OUTCLR = PIN_ADS_CS_bm;
				
			//Inicialice contadores de datos
			ADS_Ctrl->n_datos = 0;
			ADS_Ctrl->n_datos_leidos = 0;
				
			//Siguiente estado
			ADS_Ctrl->estado = ADS_ESTADO_ADQUIRIENDO;
				
			//Modo recepción de datos
			ADS_Ctrl->modo_com = ADS_MODO_COM_RECIBIENDO_MUESTRA;
			ADS_Ctrl->banderas &= ~(ADS_MUESTRA_RECIBIDA_bm);
				
			//Empiece la lectura
			USARTC1.DATA = 0x00;
				
			//Habilite interrupciones de recepción completa y DRE
			USARTC1.CTRLA |= ((3<<USART_RXCINTLVL_gp)|(3<<USART_DREINTLVL_gp));
			
		break;
		
		case ADS_ESTADO_ADQUIRIENDO:
			if(ADS_Ctrl->banderas & ADS_MUESTRA_RECIBIDA_bm)
			{
				//Inhabilite el receptor
				USARTC1.CTRLB &= ~(USART_RXEN_bm);
				
				//Vuelva al estado inicial
				ADS_Ctrl->estado = ADS_ESTADO_ESPEREDRDY;
			}
		break;
		
	}
}

void ADS_BusqueColaLibre(volatile ADS_Control *ADS_Ctrl)
{
	if (cola_vacia(&cola_ctrl1))
	{
		ADS_Ctrl->cola_activa = 1;
	}
	else if(cola_vacia(&cola_ctrl2))
	{
		ADS_Ctrl->cola_activa = 2;
	}
	else
	{
		ADS_Ctrl->cola_activa = 0;
	}
}

void ADS_Detenga(volatile ADS_Control *ADS_Ctrl)
{
	//Detenga conversiones
	PORTD.OUTCLR = PIN_ADS_START_bm;
	
	//Indicar que se solicitó detención de adquisición
	ADS_Ctrl->banderas |= ADS_DETENGA_ADQUISICION_bm;
}


ISR(USARTC1_TXC_vect)
{
	switch (ADS_Ctrl.modo_com)
	{
		//Enviando byte
		case ADS_MODO_COM_ENVIANDO_BYTE:
			ADS_Ctrl.banderas |= ADS_BYTE_ENVIADO_bm;
		break;
		
		//Enviando arreglo
		case ADS_MODO_COM_ENVIANDO_ARREGLO:
			ADS_Ctrl.banderas |= ADS_ARREGLO_ENVIADO_bm;
		break;		
	}
	
	//Apague las interrupciones por transmisión completa
	USARTC1.CTRLA &= ~(3<<USART_TXCINTLVL_gp);
	
	//Finalice interface de datos
	PORTC.OUTSET = PIN_ADS_CS_bm;
}

ISR(USARTC1_DRE_vect)
{
	switch(ADS_Ctrl.modo_com)
	{
		case ADS_MODO_COM_ENVIANDO_ARREGLO:
			//Aumente el contador de datos
			ADS_Ctrl.n_datos++;
			
			//Si no hay más datos, inhabilite la interrupción de DRE
			if (ADS_Ctrl.n_datos == ADS_Ctrl.datos_max)
			{
				USARTC1.CTRLA &= ~(3<<USART_DREINTLVL_gp);
				break;
			}
			
			USARTC1.DATA = ADS_Ctrl.arreglo[ADS_Ctrl.n_datos];
		
		break;
		
		case ADS_MODO_COM_RECIBIENDO_MUESTRA:
			ADS_Ctrl.n_datos++;
			
			//Si no hay más datos, inhabilite la interrupción de DRE
			if (ADS_Ctrl.n_datos == 27)
			{
				USARTC1.CTRLA &= ~(3<<USART_DREINTLVL_gp);
			}
			else
			{
				//Solicite el siguiente dato
				USARTC1.DATA = 0x00;
			}
		break;
	}
}

ISR(USARTC1_RXC_vect)
{
	//Lectura de datos
	ADS_Ctrl.dato = USARTC1.DATA;
		
	//Inserción de datos en las colas
	if(ADS_Ctrl.cola_activa == 1)
	{
		//char letra=0x41;
		cola_inserte(&cola_ctrl1,ADS_Ctrl.dato);//cambiar dato por char
		//cola_inserte(&cola_ctrl1,letra+contador);
		//contador++;
		//if (contador==28)
		//{
			//contador=1;
			//PORTB.OUTTGL=LED_AZUL_bm;
		//}
	}
	else
	{
				//char letra=0x41;
				cola_inserte(&cola_ctrl2,ADS_Ctrl.dato);//cambiar dato por char
				//cola_inserte(&cola_ctrl2,letra+contador);
				//contador++;
				//if (contador==28)
				//{
					//contador=1;
					//PORTB.OUTTGL=LED_AZUL_bm;
				//}

	}
	
	//Incremente contador de datos
	ADS_Ctrl.n_datos_leidos++;
	
	if(ADS_Ctrl.n_datos_leidos == 27)
	{
		//Inhabilite interrupción
		USARTC1.CTRLA &= ~(3<<USART_RXCINTLVL_gp);
		
		//Fin de recepción de la muestra
		ADS_Ctrl.banderas |= ADS_MUESTRA_RECIBIDA_bm;
		
		//Finalice la interfaz
		PORTC.OUTSET = PIN_ADS_CS_bm;	
	}
}