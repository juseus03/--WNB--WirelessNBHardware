#ifndef BTKC21_H_
#define BTKC21_H_

/* PINES UTILIZADOS Y EQUIVALENCIA
PUERTO C:
KC21	PIN			DIRECCI�N
TX		PC2			IN
RX		PC3			OUT
-------------------------------------
*/
//Definici�n de pines
//Puerto C
#define PIN_TX_bm PIN2_bm
#define PIN_RX_bm PIN3_bm

//Definici�n Par�metros de serial
#define BAUD_B 0b0
#define BAUD_A 0x03
#define USART_INTERRUPT 0//0x29
#define USART_TYPE USART_CHSIZE_8BIT_gc

//estados de transmisi�n
#define BT_ESTADO_TX_HAYDATOS 1
//#define BT_ESTADO_TX_INICIOTX 2
#define BT_ESTADO_TX_CARGANDOPAYLOAD 3

//Banderas
#define RF_BYTE_ENVIADO_bm 0x01
#define RF_ARREGLO_ENVIADO_bm 0x02
#define RF_PAYLOAD_RECIBIDO_bm 0x04
#define RF_PAYLOAD_CARGADO_bm 0x08
//Cuando est� leyendo la cola y cuando haya finalizado
#define BT_TRANSMITIENDO_bm 0x10 
#define BT_HAYDATOS_bm 0x20


typedef struct
{
	char banderas; //banderas
	char arreglo[4]; //arreglo a transmitir al BT v�a UART
	char n_datos; //contador datos enviados
	char datos_max; //m�ximo de datos
	char dato; //dato le�do
	char n_datos_leidos; //contador datos_le�dos
	char estado; //estado en las m�quinas de estado de env�o o recepci�n
	char cola_activa; //indicador de cola activa
	char modo_com; //modo de transmisi�n
}BT_Control;

void setupSerial();
void sendChar(char c);
void sendString(char *text);
void BT_Init(volatile BT_Control *BT_ctrl);
void BT_Transmitir(volatile BT_Control *BT_ctrl);

#endif