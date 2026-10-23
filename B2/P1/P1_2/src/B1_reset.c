
#include "B1_reset.h"
#include "stdbool.h"

/******** MACROS ********/

#define PORT_RESET				GPIOC
#define PIN_RESET					GPIO_PIN_13

/******** VARIABLES ********/
static GPIO_InitTypeDef GPIO_InitStruct;
static bool B1_reset_inicializado = false;

/*==========================================================
=                         FUNCIONE                        =
==========================================================*/

void B1_Reset_Init(void){
	
	if (!B1_reset_inicializado){
		__HAL_RCC_GPIOC_CLK_ENABLE();
		
		GPIO_InitStruct.Pin = PIN_RESET;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(PORT_RESET, &GPIO_InitStruct);

		HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
		
		B1_reset_inicializado = true;
		
	}
}
