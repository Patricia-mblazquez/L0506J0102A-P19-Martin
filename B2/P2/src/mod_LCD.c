#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "Driver_SPI.h"
#include "stm32f4xx_hal.h"

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
	
  while (1) {
    // Esperar un flag desde el thread de test
		osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
		
		// Representar en el LCD la información
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
		ARM_SPI_MSB_LSB |	ARM_SPI_DATA_BITS(8), 1000000); 		//poner a 1MHz para ver mejor todo en el analizador logico 1000000 el correcto es de: 20000000
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
