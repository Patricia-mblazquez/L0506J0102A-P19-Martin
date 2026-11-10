#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Driver_SPI.h"
#include "stm32f4xx_hal.h"
#include "Arial12x12.h"
#include "string.h"

/*----------------------------------------------------------------------------
 *      Thread 'mod_LCD': Plantilla
 *---------------------------------------------------------------------------*/

/*==========================================================
=                           MACROS                          =
==========================================================*/

#define PIN_LCD_RESET		GPIO_PIN_6
#define PORT_LCD_RESET	GPIOA

#define PIN_LCD_A0			GPIO_PIN_13
#define PORT_LCD_A0			GPIOF

#define PIN_LCD_CS			GPIO_PIN_14
#define PORT_LCD_CS			GPIOD

/*==========================================================
=                VARIABLES Y OBJETOS GLOBALES               =
==========================================================*/

/**** Creación de los Thread ****/
osThreadId_t tid_Th_LCD;  
osThreadId_t tid_Th_LCD_test; 

/**** Inicialización del SPI ****/
extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;
uint32_t spi_errors = 0;

/**** Escritura en LCD ****/
static unsigned char buffer[512];		// Memoria local del LCD (4 páginas x 128 bytes)
static uint16_t positionL1 = 0;

/*==========================================================
=                  DECLARACIÓN DE FUNCIONES                 =
==========================================================*/

/**** Creación de los Thread ****/
static void Th_LCD (void *argument);
static void Th_LCD_test (void *argument);

/**** Inicialización del SPI ****/
void LCD_Reset(void);
void cb_spi(uint32_t event);

/**** Comunicación básica con el LCD ****/
void LCD_wr_data(unsigned char data);
void LCD_wr_cmd(unsigned char cmd);

/**** Configuración del LCD ****/
void LCD_init(void);

/******** Escritura de datos en el LCD ********/
void LCD_update(void);
void LCD_symbolToLocalBuffer_L1(uint8_t symbol);
void Escribe_L1(uint8_t *cadena);

/*---- EJEMPLOS ----*/
static void LCD_buffer_square(void);
static void LCD_buffer_A_up(void);
static void LCD_buffer_A_down(void);
static void LCD_buffer_progress_bar(void);

/*==========================================================
=                         FUNCIONES                        =
==========================================================*/

/******** Thread de LCD ********/
int Init_Th_LCD (void) {
 
  tid_Th_LCD = osThreadNew(Th_LCD, NULL, NULL);
  if (tid_Th_LCD == NULL) {
    return(-1);
  }
 
  return(0);
}
 
static void Th_LCD (void *argument) {
 (void)argument;
	
	// Llamadas a las funciones de inicialización
	LCD_Reset();
	LCD_init();
	LCD_update();
	
  while (1) {
    // Esperar un flag desde el thread de test
		osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
		
		// Representar en el LCD la información
		Escribe_L1((uint8_t *)"Prueba LCD");

  }
}

/******** Thread de TEST ********/
int Init_Th_LCD_test (void) {
 
  tid_Th_LCD_test = osThreadNew(Th_LCD_test, NULL, NULL);
  if (tid_Th_LCD_test == NULL) {
    return(-1);
  }
 
  return(0);
}
 
static void Th_LCD_test (void *argument) {
 (void)argument;
	
  while (1) {
		// Esperar 1 segundo
		osDelay(1000);
		
		// Enviar una señal a th_LCD para representar información en el LCD
		osThreadFlagsSet(tid_Th_LCD, 0x01);
  }
}

/******** Inicialización y Reset del LCD ********/
void LCD_Reset(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// PASO 1: Inicialización y configuración del driver SPI
	SPIdrv->Initialize(cb_spi);
	SPIdrv->PowerControl(ARM_POWER_FULL);
	SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | 
		ARM_SPI_MSB_LSB |	ARM_SPI_DATA_BITS(8), 20000000); 		//poner a 1MHz para ver mejor todo en el analizador logico 1000000 el correcto es de: 20000000
	SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
	
	// PASO 2: Configurar los pines de GPIO (RESET, A0, CS)
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	
	// Estado inicial: Nivel ALTO
	HAL_GPIO_WritePin(PORT_LCD_RESET, PIN_LCD_RESET, GPIO_PIN_SET);
	HAL_GPIO_WritePin(PORT_LCD_A0, PIN_LCD_A0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(PORT_LCD_CS, PIN_LCD_CS, GPIO_PIN_SET);
	
	GPIO_InitStruct.Pin = PIN_LCD_RESET;
	HAL_GPIO_Init(PORT_LCD_RESET, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = PIN_LCD_A0;
	HAL_GPIO_Init(PORT_LCD_A0, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = PIN_LCD_CS;
	HAL_GPIO_Init(PORT_LCD_CS, &GPIO_InitStruct);
	
	// PASO 3: Generar la señal de reset (Pulso)
	HAL_GPIO_WritePin(PORT_LCD_RESET, PIN_LCD_RESET, GPIO_PIN_RESET);
	osDelay(10);
	HAL_GPIO_WritePin(PORT_LCD_RESET, PIN_LCD_RESET, GPIO_PIN_SET);
	osDelay(1);
}

/******** Callback del driver SPI ********/
void cb_spi(uint32_t event){
	if(event & ARM_SPI_EVENT_TRANSFER_COMPLETE){
		osThreadFlagsSet(tid_Th_LCD, 0x02);		// Avisar al hilo de que el SPI terminó
	}
	if(event & ARM_SPI_EVENT_DATA_LOST){
		spi_errors++;					// pérdida de datos
	}
	if(event & ARM_SPI_EVENT_MODE_FAULT){
		spi_errors++;					// conflicto de bus
	}
}


/******** Escribe un dato en el LCD ********/
void LCD_wr_data(unsigned char data){
		
	// Seleccionar CS = 0
	HAL_GPIO_WritePin(PORT_LCD_CS, PIN_LCD_CS, GPIO_PIN_RESET);
	// Seleccionar A0 = 1
	HAL_GPIO_WritePin(PORT_LCD_A0, PIN_LCD_A0, GPIO_PIN_SET);
	
	// Escribir un dato (dat) usando la funcion SPIDrv->Send(...);
	SPIdrv->Send(&data, sizeof(data));
	
	// Esperar al flag enviado desde la cb_spi() de transferencia finalizada
	osThreadFlagsWait(0x02, osFlagsWaitAny, osWaitForever);

	// Seleccionar CS = 1;
	HAL_GPIO_WritePin(PORT_LCD_CS, PIN_LCD_CS, GPIO_PIN_SET);
}

/******** Escribe un comando en el LCD ********/
void LCD_wr_cmd(unsigned char cmd){
	
	// Seleccionar CS = 0
	HAL_GPIO_WritePin(PORT_LCD_CS, PIN_LCD_CS, GPIO_PIN_RESET);
	// Seleccionar A0 = 0
	HAL_GPIO_WritePin(PORT_LCD_A0, PIN_LCD_A0, GPIO_PIN_RESET);
	
	// Escribir un comando (cmd) usando la funcion SPIDrv->Send(...);
	SPIdrv->Send(&cmd, sizeof(cmd));
		
	// Esperar al flag enviado desde la cb_spi() de transferencia finalizada
	osThreadFlagsWait(0x02, osFlagsWaitAny, osWaitForever);
	
	// Seleccionar CS = 1;
	HAL_GPIO_WritePin(PORT_LCD_CS, PIN_LCD_CS, GPIO_PIN_SET);
}

/******** Configuración del LCD ********/
void LCD_init(void){
	
	LCD_wr_cmd(0xAE);		// Display off
	LCD_wr_cmd(0xA2);		// Fija el valor de la relación de la tensión de polarización del LCD a 1/9 
	LCD_wr_cmd(0xA0);		// El direccionamiento de la RAM de datos del display es la normal
	LCD_wr_cmd(0xC8);		// El scan en las salidas COM es el normal
	LCD_wr_cmd(0x22);		// Fija la relación de resistencias interna a 2
	LCD_wr_cmd(0x2F);		// Power on
	LCD_wr_cmd(0x40);		// Display empieza en la línea 0
	LCD_wr_cmd(0xAF);		// Display ON
	LCD_wr_cmd(0x81);		// Contraste
	LCD_wr_cmd(0x0F);		// Valor Contraste
	LCD_wr_cmd(0xA4);		// Display all points normal
	LCD_wr_cmd(0xA6);		// LCD Display normal

}

/******** Escritura de datos en el LCD ********/
void LCD_update(void){
	int i;
	
	LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
	LCD_wr_cmd(0x10);	// 4 bits de la parte alta de la dirección a 0
	LCD_wr_cmd(0xB0); // Página 0
	
	for(i=0; i<128; i++){
		LCD_wr_data(buffer[i]);
	}
	
	LCD_wr_cmd(0x00);	// 4 bits de la parte baja de la dirección a 0
	LCD_wr_cmd(0x10);	// 4 bits de la parte alta de la dirección a 0		
	LCD_wr_cmd(0xB1);	// Página 1
	
	for(i=128; i<256; i++){
		LCD_wr_data(buffer[i]);
	}
	
	LCD_wr_cmd(0x00);
	LCD_wr_cmd(0x10);		
	LCD_wr_cmd(0xB2);	// Página 2
	
	for(i=256; i<384; i++){
		LCD_wr_data(buffer[i]);
	}
	
	LCD_wr_cmd(0x00);
	LCD_wr_cmd(0x10);		
	LCD_wr_cmd(0xB3);	// Página 3
	
	for(i=384; i<512; i++){
		LCD_wr_data(buffer[i]);
	}
	
}

/******** Representar un carácter 12x12 en la línea 1 ********/
void LCD_symbolToLocalBuffer_L1(uint8_t symbol){
	
	uint8_t i, value1, value2, char_width;
	uint16_t offset = 0;
	
	offset = 25*(symbol - ' ');
	char_width = Arial12x12[offset];

	if (char_width + positionL1 > 128){
		positionL1 = 0;
		for(int i=0; i<512; i++){
			buffer[i] = 0x00;
		}
	}
	
	for(i=0; i < 12; i++){
		value1 = Arial12x12[offset+i*2+1];
		value2 = Arial12x12[offset+i*2+2];
		
		buffer[i + positionL1]=value1;
		buffer[i + 128 + positionL1]=value2;
	}
	positionL1 = positionL1+Arial12x12[offset];

}

/******** Escritura de una línea de texto en la línea 1 ********/
void Escribe_L1(uint8_t *cadena){
	uint8_t i;
	positionL1 = 0;
	
	// Limpiar la linea 1 antes de escribir
	for(int j=0; j<256; j++){
		buffer[j] = 0x00; 	
	}
	
	// Recorrer la cadena
	for(i=0; cadena[i] != '\0'; i++){
		LCD_symbolToLocalBuffer_L1(cadena[i]);
	}
	LCD_update();
}

/*==========================================================
=                   EJEMPLOS PARA IMPRIMIR                  =
==========================================================*/

/*------- Imprime un cuadrado en la parte superior izquierda -------*/
static void LCD_buffer_square(void){
	
	for(int i=0; i<512; i++){
		buffer[i] = 0x00; 	// Limpiar todo el buffer
	}
	
	for(int i=0; i<8; i++){
		buffer[i] = 0xFF; 
	}
	
	LCD_update();
}

/*------- Escribir la letra 'A' en la página 0 (parte superior) -------*/
static void LCD_buffer_A_up (void){
	
	for(int i=0; i<512; i++){
		buffer[i] = 0x00; 	// Limpiar todo el buffer
	}
	
	buffer[0] = 0x00;
	buffer[1] = 0x78;
	buffer[2] = 0x24;
	buffer[3] = 0x22;
	buffer[4] = 0x22;
	buffer[5] = 0x24;
	buffer[6] = 0x78;
	buffer[7] = 0x00;
	
	LCD_update();
}

/*------- Escribir la letra 'A' en la página 3 (parte inferior) -------*/
static void LCD_buffer_A_down (void){
	for(int i=0; i<512; i++){
		buffer[i] = 0x00; 	// Limpiar todo el buffer
	}
	
	buffer[384] = 0x00;
	buffer[385] = 0x78;
	buffer[386] = 0x24;
	buffer[387] = 0x22;
	buffer[388] = 0x22;
	buffer[389] = 0x24;
	buffer[390] = 0x78;
	buffer[391] = 0x00;
	
	LCD_update();
}

/*------- Dibujar barra que ocupa el alto del LCD -------*/
static void LCD_buffer_progress_bar(void){
	static int ancho_barra = 0; 
	if (ancho_barra < 128) { 
		buffer[ancho_barra]       = 0xFF; // Pinta 8 píxeles en la Pág 0 (arriba)
		buffer[ancho_barra + 128] = 0xFF; // Pinta 8 píxeles en la Pág 1
		buffer[ancho_barra + 256] = 0xFF; // Pinta 8 píxeles en la Pág 2
		buffer[ancho_barra + 384] = 0xFF; // Pinta 8 píxeles en la Pág 3 (abajo)
            
		ancho_barra++;
   }
	
	LCD_update();
}

