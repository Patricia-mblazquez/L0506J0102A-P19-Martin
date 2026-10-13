#include "square_wave.h"


/*==========================================================
=                           MACROS                          =
==========================================================*/

// Para la salida de la señal cuadrada
#define PIN_OUTPUT 				GPIO_PIN_11
#define PORT_OUTPUT				GPIOB
#define CHANNEL_OUTPUT		TIM_CHANNEL_4

// Para el pulsador azul
#define PIN_BUTTON				GPIO_PIN_13
#define PORT_BUTTON				GPIOC

/*==========================================================
=                  DECLARACIÓN DE FUNCIONES                 =
==========================================================*/

static void initPIN_OUTPUT(void);
static void initTIM2_CH4(void);
static void initButton(void);

/*==========================================================
=                VARIABLES Y OBJETOS GLOBALES               =
==========================================================*/

TIM_HandleTypeDef tim2;
static uint32_t p = 41999;

/*==========================================================
=                         FUNCIONES                        =
==========================================================*/

void square_wave(void) {
	initPIN_OUTPUT();
	initTIM2_CH4();
	initButton();
}

/******** INICIALIZAR EL PB11 ********/

static void initPIN_OUTPUT(void) {
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = PIN_OUTPUT;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
	
	HAL_GPIO_Init(PORT_OUTPUT, &GPIO_InitStruct);
}

/******** INICIALIZAR EL TIM2-CH4 ********/

static void initTIM2_CH4(void){
	
	TIM_OC_InitTypeDef TIM_Channel_InitStruct;
	
	__HAL_RCC_TIM2_CLK_ENABLE();
	
	// Configuración del Timer-2
	tim2.Instance = TIM2;
	tim2.Init.Prescaler = 0;		// TIM2 clk 84 MHz
	tim2.Init.Period = p;				// para una freq. de 1 KHz = 1ms -> toggle cada 0.5 ms = 2 KHz
															// 84 MHz / p = freq -> p = 84MHz/2KHz = 42000 ciclos	
	HAL_TIM_OC_Init(&tim2);
	
	//Configuración del Channel-4
	TIM_Channel_InitStruct.OCMode = TIM_OCMODE_TOGGLE;
	TIM_Channel_InitStruct.OCPolarity = TIM_OCPOLARITY_HIGH;
	TIM_Channel_InitStruct.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_OC_ConfigChannel (&tim2, &TIM_Channel_InitStruct, CHANNEL_OUTPUT);
	
	HAL_TIM_OC_Start(&tim2, CHANNEL_OUTPUT);
	
}

/******** INICIALIZAR BOTÓN PARA INTERRUPCIÓN ********/

static void initButton(void) {
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = PIN_BUTTON;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(PORT_BUTTON, &GPIO_InitStruct);
	
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	
}

/******** CALLBACK PARA VARIAR LA FRECUENCIA ********/

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == PIN_BUTTON){
		if (p != 16799){
			 p =  16799;		// para una freq. de 2500 Hz = 0.4ms -> toggle cada 0.2 ms = 5 KHz
											// 84 MHz / p = freq -> p = 84MHz/5KHz = 16800 ciclos	
		}
		else{
			 p = 41999;			// para una freq. de 1 KHz
		}
		
		tim2.Init.Period = p;
		HAL_TIM_OC_Init(&tim2); 
	}
}

