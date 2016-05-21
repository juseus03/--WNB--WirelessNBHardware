/*
 * ADS1298.h
 *
 * Created: 31/01/2013 07:34:04 p.m.
 *  Author: Juan Manuel
 */ 

/* PINES UTILIZADOS Y EQUIVALENCIA
PUERTO C:
ADS1298		PIN			DIRECCIÓN
-------------------------------------
CS			PC4			OUT
SCK			PC5/XCK1	OUT
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



#ifndef ADS1298_H_
#define ADS1298_H_

//Definición de pines
//Puerto C
#define PIN_ADS_CS_bm PIN4_bm
#define PIN_ADS_SCK_bm PIN5_bm
#define PIN_ADS_DOUT_bm PIN6_bm
#define PIN_ADS_DIN_bm PIN7_bm

//Puerto D
#define PIN_ADS_START_bm PIN2_bm
#define PIN_ADS_PWD_bm PIN3_bm
#define PIN_ADS_RESET_bm PIN4_bm
#define PIN_ADS_DRDY_bm PIN5_bm

//Puerto E
#define PIN_ADS_CLK_bm PIN0_bm 

//Comandos
//Sistema
#define ADS_WAKEUP 0x02
#define ADS_STANDBY 0x04
#define ADS_RESET 0x06
#define ADS_START 0x08
#define ADS_STOP 0x0a
//Lectura de datos
#define ADS_RDATAC 0x10
#define ADS_SDATAC 0x11
#define ADS_RDATA 0x12
//Registros
#define ADS_RREG 0x20
#define ADS_WREG 0x40

//Direcciones de registros
//Global settings
#define ADS_REG_CONFIG1 0x01
#define ADS_REG_CONFIG2 0x02
#define ADS_REG_CONFIG3 0x03
#define ADS_REG_LOFF 0x04
//Channel specific settings
#define ADS_REG_CH1SET 0x05
#define ADS_REG_CH2SET 0x06
#define ADS_REG_CH3SET 0x07
#define ADS_REG_CH4SET 0x08
#define ADS_REG_CH5SET 0x09
#define ADS_REG_CH6SET 0x0a
#define ADS_REG_CH7SET 0x0b
#define ADS_REG_CH8SET 0x0c
#define ADS_REG_RLD_SENSP 0x0d
#define ADS_REG_RLD_SENSN 0x0e
#define ADS_REG_LOFF_SENSP 0x0f
#define ADS_REG_LOFF_SENSN 0x10
#define ADS_REG_LOFF_FLIP 0x11
//Lead-Off Status Registers (read only)
#define ADS_REG_LOFF_STATP 0x12
#define ADS_REG_LOFF_STATN 0x13
//Other
#define ADS_REG_GPIO 0x14
#define ADS_REG_PACE 0x15
#define ADS_REG_RESP 0x16
#define ADS_REG_CONFIG4 0x17
#define ADS_REG_WCT1 0x18
#define ADS_REG_WCT2 0x19

//Banderas
#define ADS_BYTE_ENVIADO_bm 0x01
#define ADS_ARREGLO_ENVIADO_bm 0x02
#define ADS_MUESTRA_RECIBIDA_bm 0x04
#define ADS_DETENGA_ADQUISICION_bm 0x08
#define ADS_ADQUISICION_DETENIDA_bm 0x10

//Modos de transmisión
#define ADS_MODO_COM_ENVIANDO_BYTE 1
#define ADS_MODO_COM_ENVIANDO_ARREGLO 2
#define ADS_MODO_COM_RECIBIENDO_MUESTRA 3

//Modos de configuración
#define ADS_MODO_CONFIG_CORTO 1
#define ADS_MODO_CONFIG_TEST 2
#define ADS_MODO_CONFIG_NORMAL 3
#define ADS_MODO_CONFIG_OFF 4
#define ADS_MODO_CONFIG_TEST_CH1 5
#define ADS_MODO_CONFIG_TEST_CH2 6
#define ADS_MODO_CONFIG_TEST_CH3 7
#define ADS_MODO_CONFIG_TEST_CH4 8
#define ADS_MODO_CONFIG_TEST_CH5 9
#define ADS_MODO_CONFIG_TEST_CH6 10
#define ADS_MODO_CONFIG_TEST_CH7 11
#define ADS_MODO_CONFIG_TEST_CH8 12 

//Modos de energía
#define ADS_ENERGIA_HIGHRESOLUTION 0x80
#define ADS_ENERGIA_LOWPOWER 0x00

//Data Rates
#define ADS_DATARATE_HR_32K 0x00
#define ADS_DATARATE_HR_16K 0x01
#define ADS_DATARATE_HR_8K 0x02
#define ADS_DATARATE_HR_4K 0x03
#define ADS_DATARATE_HR_2K 0x04
#define ADS_DATARATE_HR_1K 0x05
#define ADS_DATARATE_HR_500 0x06

#define ADS_DATARATE_LP_16K 0x00
#define ADS_DATARATE_LP_8K 0x01
#define ADS_DATARATE_LP_4K 0x02
#define ADS_DATARATE_LP_2K 0x03
#define ADS_DATARATE_LP_1K 0x04
#define ADS_DATARATE_LP_500 0x05
#define ADS_DATARATE_LP_250 0x06

//Ganancias
#define ADS_GANANCIA_1 0b00010000
#define ADS_GANANCIA_2 0b00100000
#define ADS_GANANCIA_3 0b00110000
#define ADS_GANANCIA_4 0b01000000
#define ADS_GANANCIA_6 0x00
#define ADS_GANANCIA_8 0b01010000
#define ADS_GANANCIA_12 0b01100000

//Mux Modo
#define ADS_MUX_NORMAL 0x00
#define ADS_MUX_CORTO 0b00000001
#define ADS_MUX_RLD 0b00000010
#define ADS_MUX_MVDD 0b00000011
#define ADS_MUX_TEMPER 0b00000100
#define ADS_MUX_TEST 0b00000101
#define ADS_MUX_DRP 0b00000110
#define ADS_MUX_DRN 0b00000111

#define ADS_CHN_OFF 0x80

//Constantes
#define ADS_ARREGLOMAX 17

//Estados
#define ADS_ESTADO_ESPEREDRDY 1
#define ADS_ESTADO_BUSQUECOLA 2
#define ADS_ESTADO_EMPIECE_ADQUISICION 3
#define ADS_ESTADO_ADQUIRIENDO 4

typedef struct
{
	char banderas; //banderas
	char modo_com; //Modo de comunicación
	char arreglo[ADS_ARREGLOMAX]; //Máximo tamaño del arreglo
	char n_datos; //Contador de datos
	char datos_max; //Máximo de datos a transmitir/recibir
	char dato; //dato leído, almacenamiento temporal
	char n_datos_leidos; //contador datos leídos
	char estado; //estado de máquina de estados
	char cola_activa; //cola activa para llenado
}ADS_Control;

void ADS_Init(volatile ADS_Control *ADS_Ctrl, 
				char powermode,
				char datarate);

void ADS_EnviarByte(volatile ADS_Control *ADS_Ctrl, 
				char comando);

void ADS_EnviarArreglo(volatile ADS_Control *ADS_Ctrl,
					char datos_max );

void ADS_ConfigChannels(volatile ADS_Control *ADS_Ctrl,
					char Config_modo,
					char Ganancia);
					
void ADS_Adquiera(volatile ADS_Control *ADS_Ctrl);

void ADS_BusqueColaLibre(volatile ADS_Control *ADS_Ctrl);

void ADS_Detenga(volatile ADS_Control *ADS_Ctrl);

					
					

#endif /* ADS1298_H_ */