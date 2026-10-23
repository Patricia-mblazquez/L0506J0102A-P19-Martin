#include "leds.h"

/******** MACROS ********/
#define PORT				GPIOB
#define PIN_GREEN		GPIO_PIN_0		//LED 1
#define PIN_BLUE		GPIO_PIN_7		//LED 2
#define PIN_RED			GPIO_PIN_14		//LED 3

/*==========================================================
=                         FUNCIONES                        =
==========================================================*/

void LEDS_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	
	GPIO_InitStruct.Pin = PIN_GREEN | PIN_BLUE | PIN_RED;
	HAL_GPIO_Init(PORT, &GPIO_InitStruct);
}

void LED_Init(uint8_t led){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	switch (led){
		case LED1:
			GPIO_InitStruct.Pin = PIN_GREEN;
			break;
		case LED2:
			GPIO_InitStruct.Pin = PIN_BLUE;
			break;
		case LED3:
			GPIO_InitStruct.Pin = PIN_RED;
			break;
		default:
			return;
	}
	
	HAL_GPIO_Init(PORT, &GPIO_InitStruct);
}

void LEDS_On(void){
	HAL_GPIO_WritePin(PORT, PIN_GREEN | PIN_BLUE | PIN_RED, GPIO_PIN_SET);
}

void LED_On(uint8_t led){
	switch (led){
		case LED1:
			HAL_GPIO_WritePin(PORT, PIN_GREEN, GPIO_PIN_SET);
			break;
		case LED2:
			HAL_GPIO_WritePin(PORT, PIN_BLUE, GPIO_PIN_SET);
			break;
		case LED3:
			HAL_GPIO_WritePin(PORT, PIN_RED, GPIO_PIN_SET);
			break;
		default:
			return;
	}
}


void LEDS_Off(void){
	HAL_GPIO_WritePin(PORT, PIN_GREEN | PIN_BLUE | PIN_RED, GPIO_PIN_RESET);
}

void LED_Off(uint8_t led){
	switch (led){
		case LED1:
			HAL_GPIO_WritePin(PORT, PIN_GREEN, GPIO_PIN_RESET);
			break;
		case LED2:
			HAL_GPIO_WritePin(PORT, PIN_BLUE, GPIO_PIN_RESET);
			break;
		case LED3:
			HAL_GPIO_WritePin(PORT, PIN_RED, GPIO_PIN_RESET);
			break;
		default:
			return;
	}
}

void LED_Toggle(uint8_t led){
	switch (led){
		case LED1:
			HAL_GPIO_TogglePin(PORT, PIN_GREEN);
			break;
		case LED2:
			HAL_GPIO_TogglePin(PORT, PIN_BLUE);
			break;
		case LED3:
			HAL_GPIO_TogglePin(PORT, PIN_RED);
			break;
		default:
			return;
	}
}
