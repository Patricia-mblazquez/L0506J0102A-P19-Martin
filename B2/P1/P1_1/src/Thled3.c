
#include "Thled3.h"
#include "Thled2.h"
 
/*----------------------------------------------------------------------------
 *      Thread 3 'Thled3': Control periodico del LED3
 *---------------------------------------------------------------------------*/
 
/******** MACROS ********/
#define PIN		GPIO_PIN_14
#define PORT	GPIOB
 
/******** VARIABLES ********/
osThreadId_t tid_Thled3;                        // thread id
 
/******** DECLARACIÓN DE FUNCIONES ********/
int Init_Thled3 (void);
static void Thled3 (void *argument);                   // thread function
static void initLed(void);


/*==========================================================
=                         FUNCIONES                        =
==========================================================*/

/******** CREACIÓN DEL HILO ********/
int Init_Thled3 (void) {
 
  tid_Thled3 = osThreadNew(Thled3, NULL, NULL);
  if (tid_Thled3 == NULL) {
    return(-1);
  }
 
  return(0);
}
 
/******** FUNCIÓN DEL HILO ********/
static void Thled3 (void *argument) {
	
	initLed();
	
  while (1) {
		osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);

		for(int i=0; i<30; i++){
			HAL_GPIO_WritePin(PORT, PIN, GPIO_PIN_SET);
			osDelay(287);
			HAL_GPIO_WritePin(PORT, PIN, GPIO_PIN_RESET);
			osDelay(287);
			
			if(i == 25){
				osThreadFlagsSet(tid_Thled2, 0x0001);
			}
			
		}
  }
}

/******** INICIALIZACIÓN EL LED ********/
static void initLed(void){
	GPIO_InitTypeDef GPIO_InitStruct;
    
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	GPIO_InitStruct.Pin = PIN;
	HAL_GPIO_Init(PORT, &GPIO_InitStruct);
	
}

