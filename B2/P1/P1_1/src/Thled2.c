
#include "Thled2.h"
 
/*----------------------------------------------------------------------------
 *      Thread 2 'Thled2': Control periodico del LED2
 *---------------------------------------------------------------------------*/
 
/******** MACROS ********/
#define PIN		GPIO_PIN_7
#define PORT	GPIOB 

/******** VARIABLES ********/
static osThreadId_t tid_Thled2;                        // thread id
 
/******** DECLARACIÓN DE FUNCIONES ********/
int Init_Thled2 (void);
static void Thled2 (void *argument);                   // thread function
static void initLed(void);


/*==========================================================
=                         FUNCIONES                        =
==========================================================*/

/******** CREACIÓN DEL HILO ********/
int Init_Thled2 (void) {
 
  tid_Thled2 = osThreadNew(Thled2, NULL, NULL);
  if (tid_Thled2 == NULL) {
    return(-1);
  }
 
  return(0);
}
 
/******** FUNCIÓN DEL HILO ********/
static void Thled2 (void *argument) {
	
	initLed();
	
  while (1) {
		HAL_GPIO_WritePin(PORT, PIN, GPIO_PIN_SET);
		osDelay(137);
		HAL_GPIO_WritePin(PORT, PIN, GPIO_PIN_RESET);
		osDelay(137);
		
  }
}

/******** INICIALIZACIÓN EL LED ********/
static void initLed(void){
	GPIO_InitTypeDef GPIO_InitStruct;
    
	__HAL_RCC_GPIOB_CLK_ENABLE();		// SE HACE 3 VECES, INECESARIO. SOLUCION? Da igual o incluir leds.c 
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	GPIO_InitStruct.Pin = PIN;
	HAL_GPIO_Init(PORT, &GPIO_InitStruct);
	
}

