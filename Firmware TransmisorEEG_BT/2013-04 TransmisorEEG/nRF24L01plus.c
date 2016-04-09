/*
 * nRF24L01plus.c
 *
 * Created: 22/11/2012 11:29:25 a.m.
 *  Author: Usuario
 */ 

/* PINES UTILIZADOS Y EQUIVALENCIA
PUERTO C:
nRF24L01	PIN			DIRECCIÓN
-------------------------------------
CSN			PC0			OUT
SCK			PC1			OUT
MISO		PC2/MISO	IN
MOSI		PC3/MOSI	OUT
-------------------------------------

PUERTO D:
nRF24L01	PIN			DIRECCIÓN
-------------------------------------
CE			PD0			OUT
IRQ			PD1			IN

 */

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 32000000UL
#include <util/delay.h>
#include "nRF24L01plus.h"
#include "to_tcd0_delays.h"
#include "cola.h"
#include "LEDs.h"

volatile extern to_tcd0_delays_control to_d0_ctrl;
volatile extern cola_control cola_ctrl1;
volatile extern cola_control cola_ctrl2;
volatile extern RF_Control RF_ctrl;

void RF_Init(volatile RF_Control *BT_ctrl)
{
	_delay_ms(500);
	
	//Configuración de pines
	PORTC.OUTCLR = (PIN_RF_CSN_bm|PIN_RF_SCK_bm|PIN_RF_MOSI_bm);
	PORTC.DIRSET = (PIN_RF_CSN_bm|PIN_RF_SCK_bm|PIN_RF_MOSI_bm);
	
	PORTD.OUTCLR = (PIN_RF_CE_bm);
	PORTD.DIRSET = (PIN_RF_CE_bm);
	
	//CSN es activo bajo
	PORTC.OUTSET = PIN_RF_CSN_bm;
	
	//Configuración de USARTC0
	//Se deben enviar bytes empezando por el MSBite del LSByte
	//Control de velocidad de transferencia
	//BSEL= fPER/(2fBAUD) - 1
	//16Mbps = 1
	USARTC0.BAUDCTRLA = 0x07; //2Mbps
	USARTC0.BAUDCTRLB = 0x00;
	
	//Modo Master SPI
	USARTC0.CTRLC |= 0b11000000;
	
	//Habilite transmisor
	USARTC0.CTRLB |= (USART_TXEN_bm);
	
	//Configuración del Transmisor Principal PTX
	
	//Tamaño de la dirección
	RF_ctrl->arreglo[0] = (RF_W_REGISTER|RF_SETUP_AW);
	RF_ctrl->arreglo[1] = 0x01; //3 bytes
	RF_EnviarArreglo(RF_ctrl,2);
	//Espere a que se haya enviado el arreglo
	while(!(RF_ctrl->banderas & RF_ARREGLO_ENVIADO_bm));
	
	//Configuración Canal
	RF_ctrl->arreglo[0] = (RF_W_REGISTER|RF_RF_CH);
	RF_ctrl->arreglo[1] = 20;
	RF_EnviarArreglo(RF_ctrl,2);
	//Espere a que se haya enviado el arreglo
	while(!(RF_ctrl->banderas & RF_ARREGLO_ENVIADO_bm));
	
	//Dirección RX_Pipe 0
	RF_ctrl->arreglo[0] = (RF_W_REGISTER|RF_RX_ADDR_P0);
	RF_ctrl->arreglo[1] = (0xcb);
	RF_ctrl->arreglo[2] = (0x1c);
	RF_ctrl->arreglo[3] = (0x67);
	RF_EnviarArreglo(RF_ctrl,4);
	//Espere a que se haya enviado el arreglo
	while(!(RF_ctrl->banderas & RF_ARREGLO_ENVIADO_bm));
	
	//Dirección de Transmisión TX_ADDR
	RF_ctrl->arreglo[0] = (RF_W_REGISTER|RF_TX_ADDR);
	RF_ctrl->arreglo[1] = (0xcb);
	RF_ctrl->arreglo[2] = (0x1c);
	RF_ctrl->arreglo[3] = (0x67);
	RF_EnviarArreglo(RF_ctrl,4);
	//Espere a que se haya enviado el arreglo
	while(!(RF_ctrl->banderas & RF_ARREGLO_ENVIADO_bm));
	
	//Tamaño de payload en data pipe 0
	RF_ctrl->arreglo[0] = (RF_W_REGISTER|RF_PW_P0);
	RF_ctrl->arreglo[1] = 27;
	RF_EnviarArreglo(RF_ctrl,2);
	//Espere a que se haya enviado el arreglo
	while(!(RF_ctrl->banderas & RF_ARREGLO_ENVIADO_bm));
	
	//Ajuste de la configuración: Encienda dispositivo - PTX
	RF_ctrl->arreglo[0] = (RF_W_REGISTER|RF_CONFIG);
	RF_ctrl->arreglo[1] = (0b00001010);
	RF_EnviarArreglo(RF_ctrl,2);
	//Espere a que se haya enviado el arreglo
	while(!(RF_ctrl->banderas & RF_ARREGLO_ENVIADO_bm));
	
	RF_ctrl->estado = 1; //estado inicial de las máquinas de estados
	
	_delay_ms(500);
}

void RF_EnviarByte(volatile RF_Control *RF_ctrl, char comando)
{
	//Active transmisión de comando
	PORTC.OUTCLR = PIN_RF_CSN_bm;
	
	//Limpie bandera de envío completo y de envío de arreglo
	RF_ctrl->banderas &= ~(RF_BYTE_ENVIADO_bm);
	
	//Indique que se está enviando un byte
	RF_ctrl->modo_com = RF_MODO_COM_ENVIANDO_BYTE;
	
	//Ponga el dato en el registro
	USARTC0.DATA = comando;
	
	//habilite interrupciones de txc
	USARTC0.CTRLA |= (3<<USART_TXCINTLVL_gp);
}

void RF_EnviarArreglo(volatile RF_Control *RF_ctrl,
					char datos_max)
{
	//Active transmisión de comando
	PORTC.OUTCLR = PIN_RF_CSN_bm;
	
	//Limpie bandera de envío de arreglo
	RF_ctrl->banderas &= ~(RF_ARREGLO_ENVIADO_bm);
	
	//Indique que se está enviando un arreglo
	RF_ctrl->modo_com = RF_MODO_COM_ENVIANDO_ARREGLO;
	
	//Fije el número de datos
	RF_ctrl->datos_max = datos_max;
	RF_ctrl->n_datos = 0;
	
	//Envíe el primer dato
	USARTC0.DATA = RF_ctrl->arreglo[0];
	
	//habilite interrupciones de TXC y de DRE
	USARTC0.CTRLA |= ((3<<USART_TXCINTLVL_gp)|(3<<USART_DREINTLVL_gp));
}

void RF_BuscarColaLista(volatile RF_Control *RF_ctrl)
{
	if(cola_llena(&cola_ctrl1))
	{
		RF_ctrl->cola_activa = 1;
	}
	else if(cola_llena(&cola_ctrl2))
	{
		RF_ctrl->cola_activa = 2;
	}
	else
	{
		RF_ctrl->cola_activa = 0;
	}
	
}

void RF_BuscarColaVacia(volatile RF_Control *RF_ctrl)
{
	if (cola_vacia(&cola_ctrl1))
	{
		RF_ctrl->cola_activa = 1;
	}
	else if (cola_vacia(&cola_ctrl2))
	{
		RF_ctrl->cola_activa = 2;
	}
	else
	{
		RF_ctrl->cola_activa = 0;
	}
}

void RF_Transmitir(volatile RF_Control *RF_ctrl)
{	
	switch(RF_ctrl->estado)
	{
		//Estado 1: Verificar datos para cargar
		case RF_ESTADO_TX_HAYDATOS:
			RF_BuscarColaLista(RF_ctrl);
			if(RF_ctrl->cola_activa)
			{
				RF_ctrl->estado = RF_ESTADO_TX_INICIOTX;
			
				//indique que se está cargando payload
				RF_ctrl->modo_com = RF_MODO_COM_CARGANDO_PAYLOAD;
			}
		
		break;
		
		//Estado 2: Transmita comando de escritura de payload
		//y continúa con el envío consecutivo de datos
		case RF_ESTADO_TX_INICIOTX:
			//Active transmisión de comando
			PORTC.OUTCLR = PIN_RF_CSN_bm;
		
			//Inicialice número de datos
			RF_ctrl->n_datos = 0;
		
			//Pase al siguiente estado
			RF_ctrl->estado = RF_ESTADO_TX_CARGANDOPAYLOAD;
			
			//Limpie bandera de payload cargado
			RF_ctrl->banderas &= ~(RF_PAYLOAD_CARGADO_bm);
			
			//cambie mode de comunicación
			RF_ctrl->modo_com = RF_MODO_COM_CARGANDO_PAYLOAD;
		
			//Envíe el primer dato
			USARTC0.DATA = RF_W_TX_PAYLOAD;
		
			//habilite interrupciones de TXC y de DRE
			USARTC0.CTRLA |= ((3<<USART_TXCINTLVL_gp)|(3<<USART_DREINTLVL_gp));
		
		break;
		
		//Espere a que se haya cargado el payload
		case RF_ESTADO_TX_CARGANDOPAYLOAD:
			if (RF_ctrl->banderas & RF_PAYLOAD_CARGADO_bm)
			{
				PORTB.OUTTGL = LED_AZUL_bm;
				
				//Pase al siguiente estado
				RF_ctrl->estado = RF_ESTADO_TX_ESPERANDOTOA;
			
				//ponga en alto CE para el inicio de transmisión
				PORTD.OUTSET = PIN_RF_CE_bm;
			
				//Ponga un to para TOA
				To_tcd0_InitTo(&to_d0_ctrl,0x10,0x90,0x01);
			
			}
		
		break;
		
		case RF_ESTADO_TX_ESPERANDOTOA:
			if(To_tcd0_HuboFin(&to_d0_ctrl))
			{
				//Pase al siguiente estado
				RF_ctrl->estado = RF_ESTADO_TX_ESPERANDOACK;
			
				//baje CE
				PORTD.OUTCLR = PIN_RF_CE_bm;
			}
		break;
		
		case RF_ESTADO_TX_ESPERANDOACK:
		
			//Espere interrupción de ack
			if(!(PORTD.IN & PIN_RF_IRQ_bm))
			{
				RF_ctrl->arreglo[0] = (RF_W_REGISTER|RF_STATUS);
				RF_ctrl->arreglo[1] = 0b00110000;
				RF_EnviarArreglo(RF_ctrl,2);
				RF_ctrl->estado = RF_ESTADO_TX_LIMPIAR_TXDS_MAXRT;
			}
		break;
		
		case RF_ESTADO_TX_LIMPIAR_TXDS_MAXRT:
			//Espere fin de arreglo
			if (RF_ctrl->banderas & RF_ARREGLO_ENVIADO_bm)
			{
				
				RF_ctrl->estado = RF_ESTADO_TX_HAYDATOS;				
				
				//Ponga un to para TOA
				//To_tcd0_InitTo(&to_d0_ctrl,0x7a,0x12,0x07);
				//RF_ctrl->estado = RF_ESTADO_TX_ESPEREANTESDETX;
			}
		break;
		
		case RF_ESTADO_TX_ESPEREANTESDETX:
		
			if(To_tcd0_HuboFin(&to_d0_ctrl))
			{
				//Pase al siguiente estado
				RF_ctrl->estado = RF_ESTADO_TX_HAYDATOS;
			}
		
		break;
	}
}

void RF_Recibir(volatile RF_Control *RF_ctrl)
{
	switch(RF_ctrl->estado)
	{
		//Estado 1: Iniciar Recepción
		case RF_ESTADO_RX_INICIERX:
		
			PORTD.OUTSET = PIN_RF_CE_bm;
		
			//Ponga un to de 130us
			To_tcd0_InitTo(&to_d0_ctrl,0x10,0x40,0x01);
		
			//Pase al siguiente estado a esperar fin de to
			RF_ctrl->estado = RF_ESTADO_RX_ESPERETO;
		
		break;
		
		case RF_ESTADO_RX_ESPERETO:
			if(To_tcd0_HuboFin(&to_d0_ctrl))
			{
				//Pase al siguiente estado
				RF_ctrl->estado = RF_ESTADO_RX_ESPERANDOIRQ;
			}
		
		break;
		
		case RF_ESTADO_RX_ESPERANDOIRQ:
			if(!(PORTD.IN & PIN_RF_IRQ_bm))
			{
				//Pase al siguiente estado
				RF_ctrl->estado = RF_ESTADO_RX_COLA_VACIA;
			
				//Baje CE
				PORTD.OUTCLR = PIN_RF_CE_bm;
			}
		
		break;
		
		case RF_ESTADO_RX_COLA_VACIA:
			RF_BuscarColaVacia(RF_ctrl);
			if (RF_ctrl->cola_activa)
			{
				//Pase al siguiente estado
				RF_ctrl->estado = RF_ESTADO_RX_INITPAYLOAD;
			}
		
		break;
		
		case RF_ESTADO_RX_INITPAYLOAD:
			//Active receptor USART
			USARTC0.CTRLB |= (USART_RXEN_bm);
			
			//Inicie interfaz de comunicación
			PORTC.OUTCLR = PIN_RF_CSN_bm;
			
			//Inicialice contador de datos
			RF_ctrl->n_datos = 0;
			RF_ctrl->n_datos_leidos = 0;
		
			//Pase al siguiente estado
			RF_ctrl->estado = RF_ESTADO_RX_PAYLOAD;
		
			//Modo de recepción de datos
			RF_ctrl->modo_com = RF_MODO_COM_RECIBIENDO_PAYLOAD;
			RF_ctrl->banderas &= ~(RF_PAYLOAD_RECIBIDO_bm);
			
			//Envíe comando de lectura
			USARTC0.DATA = RF_R_RX_PAYLOAD;
			
			//Habilite interrupciones de recepción completa y DRE
			USARTC0.CTRLA |= ((3<<USART_RXCINTLVL_gp)|(3<<USART_DREINTLVL_gp));
			
		break;
		
		case RF_ESTADO_RX_PAYLOAD:
			//Espere fin de lectura de payload
			if(RF_ctrl->banderas & RF_PAYLOAD_RECIBIDO_bm)
			{
				//Inhabilite receptor
				USARTC0.CTRLB &= ~(USART_RXEN_bm);
				
				RF_ctrl->arreglo[0] = (RF_W_REGISTER|RF_STATUS);
				RF_ctrl->arreglo[1] = 0b01000000;
				RF_EnviarArreglo(RF_ctrl,2);
				
				RF_ctrl->estado = RF_ESTADO_RX_LIMPIAR_RXDR;		
			}
		break;
		
		case RF_ESTADO_RX_LIMPIAR_RXDR:
			if (RF_ctrl->banderas & RF_ARREGLO_ENVIADO_bm)
			{
				//Vuelva al estado inicial
				RF_ctrl->estado = RF_ESTADO_RX_INICIERX;
			}
		
		break;
		
	}
}


//Interrupción que se ejecuta cuando el registro de datos está vacío
ISR(USARTC0_DRE_vect)
{
	switch (RF_ctrl.modo_com)
	{
		//Enviando arreglo
		case RF_MODO_COM_ENVIANDO_ARREGLO:
			//Aumente el contador de datos
			RF_ctrl.n_datos++;
			
			//Si no hay más datos, inhabilite la interrupción de DRE
			if (RF_ctrl.n_datos == RF_ctrl.datos_max)
			{
				USARTC0.CTRLA &= ~(3<<USART_DREINTLVL_gp);
				break;
			}				
			
			USARTC0.DATA = RF_ctrl.arreglo[RF_ctrl.n_datos];	
		
		break;
		
		case RF_MODO_COM_CARGANDO_PAYLOAD:
			RF_ctrl.n_datos++;
			
			//Si no hay más datos, inhabilite la interrupción de DRE
			if (RF_ctrl.n_datos == 28)
			{
				USARTC0.CTRLA &= ~(3<<USART_DREINTLVL_gp);
			}
			//Si hay otro dato, envíelo
			else
			{
				if (RF_ctrl.cola_activa == 1)
				{
					cola_retire(&cola_ctrl1,&(RF_ctrl.dato));	
				}
				else
				{
					cola_retire(&cola_ctrl2,&(RF_ctrl.dato));	
				}
				
				USARTC0.DATA = RF_ctrl.dato;
			}
			
		break;
		
		case RF_MODO_COM_RECIBIENDO_PAYLOAD:
			//incremente contador
			RF_ctrl.n_datos++;
			
			if (RF_ctrl.n_datos == 28)
			{
				//inhabilite interrupción
				USARTC0.CTRLA &= ~(3<<USART_DREINTLVL_gp);
			}
			else
			{
				//Solicite el siguiente dato
				USARTC0.DATA = 0x00;
			}		
		break;			
	}
	
}

//Interrupción que se ejectua al finalizar una recepción por usart
ISR(USARTC0_RXC_vect)
{
	//Lectura de dato
	RF_ctrl.dato = USARTC0.DATA;
	
	if (RF_ctrl.n_datos_leidos)
	{
		if (RF_ctrl.cola_activa == 1)
		{
			//Inserción dato en la cola
			cola_inserte(&cola_ctrl1,RF_ctrl.dato);
		}
		else
		{
			cola_inserte(&cola_ctrl2,RF_ctrl.dato);
		}
		
	}
	
	//Incremente contador datos
	RF_ctrl.n_datos_leidos++;
	
	if (RF_ctrl.n_datos_leidos == 28)
	{
		//Inhabilite interrupción
		USARTC0.CTRLA &= ~(3<<USART_RXCINTLVL_gp);
		
		//Informe fin de recepción de payload
		RF_ctrl.banderas |= RF_PAYLOAD_RECIBIDO_bm;
		
		//Finalice la interfaz
		PORTC.OUTSET = PIN_RF_CSN_bm;
	}
}

//Interrupción que se ejecuta al final de una transmisión por el puerto SPI
ISR(USARTC0_TXC_vect)
{
	switch (RF_ctrl.modo_com)
	{
		//Enviando arreglo
		case RF_MODO_COM_ENVIANDO_ARREGLO:
			//Informe que se envió el arreglo
			RF_ctrl.banderas |= RF_ARREGLO_ENVIADO_bm;
		break;
		
		case RF_MODO_COM_ENVIANDO_BYTE:
			//Informe que se envió el byte
			RF_ctrl.banderas |= RF_BYTE_ENVIADO_bm;
		break;
		
		case RF_MODO_COM_CARGANDO_PAYLOAD:
			//Informe payload cargado
			RF_ctrl.banderas |= RF_PAYLOAD_CARGADO_bm;
		break;
	}
	
	//Apague las interrupciones por transmisión completa
	USARTC0.CTRLA &= ~(3<<USART_TXCINTLVL_gp);
	
	//Finalice interface de datos
	PORTC.OUTSET= PIN_RF_CSN_bm;
}