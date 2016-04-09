/*
 * nRF24L01plus.h
 *
 * Created: 22/11/2012 11:24:17 a.m.
 *  Author: Usuario
 */ 


#ifndef NRF24L01PLUS_H_
#define NRF24L01PLUS_H_

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

//Definición de pines
//Puerto C
#define PIN_RF_CSN_bm PIN0_bm
#define PIN_RF_SCK_bm PIN1_bm
#define PIN_RF_MISO_bm PIN2_bm
#define PIN_RF_MOSI_bm PIN3_bm

//Puerto D
#define PIN_RF_CE_bm PIN0_bm
#define PIN_RF_IRQ_bm PIN1_bm

//Comandos
#define RF_R_REGISTER 0x00
#define RF_W_REGISTER 0b00100000
#define RF_R_RX_PAYLOAD 0b01100001
#define RF_W_TX_PAYLOAD 0b10100000
#define RF_FLUSH_TX 0b11100001
#define RF_FLUSH_RX 0b11100010
#define RF_REUSE_TX_PL 0b11100011
#define RF_R_RX_PL_WID 0b01100000
#define RF_W_ACK_PAYLOAD 0b10101111
#define RF_W_TX_PAYLOAD_NOACK 0b10110000
#define RF_NOP 0xff

//Direcciones de registros
#define RF_CONFIG 0x00
#define RF_SETUP_AW 0x03
#define RF_RF_CH 0x05
#define RF_STATUS 0x07
#define RF_RX_ADDR_P0 0x0a
#define RF_TX_ADDR 0x10
#define RF_PW_P0 0x11

//Banderas
#define RF_BYTE_ENVIADO_bm 0x01
#define RF_ARREGLO_ENVIADO_bm 0x02
#define RF_PAYLOAD_RECIBIDO_bm 0x04
#define RF_PAYLOAD_CARGADO_bm 0x08
#define RF_TRANSMITIENDO_bm 0x10

//estados de transmisión
#define RF_ESTADO_TX_HAYDATOS 1
#define RF_ESTADO_TX_INICIOTX 2
#define RF_ESTADO_TX_CARGANDOPAYLOAD 3
#define RF_ESTADO_TX_ESPERANDOTOA 4
#define RF_ESTADO_TX_ESPERANDOACK 5
#define RF_ESTADO_TX_LIMPIAR_TXDS_MAXRT 6
#define RF_ESTADO_TX_ESPEREANTESDETX 7

//estados de recepción
#define RF_ESTADO_RX_INICIERX 1
#define RF_ESTADO_RX_ESPERETO 2
#define RF_ESTADO_RX_ESPERANDOIRQ 3
#define RF_ESTADO_RX_COLA_VACIA 4
#define RF_ESTADO_RX_INITPAYLOAD 5
#define RF_ESTADO_RX_PAYLOAD 6
#define RF_ESTADO_RX_LIMPIAR_RXDR 7

//modos dre
#define RF_MODO_COM_ENVIANDO_ARREGLO 1
#define RF_MODO_COM_ENVIANDO_BYTE 2
#define RF_MODO_COM_CARGANDO_PAYLOAD 3
#define RF_MODO_COM_RECIBIENDO_PAYLOAD 4

typedef struct
{
	char banderas; //banderas
	char arreglo[4]; //arreglo a transmitir al nRF vía SPI
	char n_datos; //contador datos enviados
	char datos_max; //máximo de datos
	char dato; //dato leído
	char n_datos_leidos; //contador datos_leídos
	char estado; //estado en las máquinas de estado de envío o recepción
	char cola_activa; //indicador de cola activa
	char modo_com; //modo de transmisión
}RF_Control;

void RF_Init(volatile RF_Control *RF_ctrl);
			
void RF_EnviarByte(volatile RF_Control *RF_ctrl,
					char comando);

void RF_EnviarArreglo(volatile RF_Control *RF_ctrl,
						char datos_max);

void RF_Recibir(volatile RF_Control *RF_ctrl);

void RF_Transmitir(volatile RF_Control *RF_ctrl);

void RF_BuscarColaLista(volatile RF_Control *RF_ctrl);

void RF_BuscarColaVacia(volatile RF_Control *RF_ctrl);

#endif /* NRF24L01PLUS_H_ */