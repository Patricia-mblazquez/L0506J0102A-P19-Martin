
#include "Thled1.h"
 
#define PIN		GPIO_PIN_0
#define PORT	GPIOB
 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
static osThreadId_t tid_Thled1;                        // thread id
 
int Init_Thled1 (void);
static void Thled1 (void *argument);                   // thread function
 
int Init_Thled1 (void) {
 
  tid_Thled1 = osThreadNew(Thled1, NULL, NULL);
  if (tid_Thled1 == NULL) {
    return(-1);
  }
 
  return(0);
}
 
static void Thled1 (void *argument) {
	GPIO_InitTypeDef GPIO_InitStruct;
    
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	GPIO_InitStruct.Pin = PIN;
	HAL_GPIO_Init(PORT, &GPIO_InitStruct);
	
	/* Para que los tiempos sean la mitad sin cambiar el codigo fuente,
	en RTX_Config.h cambiar OS_TICK_FREQ de 1000 a 2000 Hz.
	Asi, cada tick dura la mitad (de 1 ms a 0.5 ms) y los tiempos
	de osDelay() se reducen a la mitad. */
	
  while (1) {
		HAL_GPIO_WritePin(PORT, PIN, GPIO_PIN_SET);
		osDelay(200);
		HAL_GPIO_WritePin(PORT, PIN, GPIO_PIN_RESET);
		osDelay(800);
		
  }
}
