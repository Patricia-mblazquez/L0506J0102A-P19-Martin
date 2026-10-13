#include "pwm.h"

/*==========================================================
=                           MACROS                          =
==========================================================*/

// Para la salida de la señal cuadrada
#define PIN 				GPIO_PIN_7
#define PORT				GPIOA
#define CHANNEL			TIM_CHANNEL_2


// Para el pulsador azul
#define PIN_BUTTON				GPIO_PIN_13
#define PORT_BUTTON				GPIOC


/*==========================================================
=                  DECLARACIÓN DE FUNCIONES                 =
==========================================================*/

void initPWM(void);
static void initPIN_OUTPUT(void);
static void initTIM3_CH2(void);
static void initButton(void);

/*==========================================================
=                VARIABLES Y OBJETOS GLOBALES               =
==========================================================*/

static GPIO_InitTypeDef GPIO_InitStruct;
static TIM_HandleTypeDef htim3;
static TIM_OC_InitTypeDef TIM_Channel_InitStruct;

static uint32_t pulse = 100;		// Ciclo de trabajo = (pulse/period)*100 -> pulse = (cicloTrabajo*period)/100 = (20*500)/100 = 100

/*==========================================================
=                         FUNCIONES                        =
==========================================================*/

void initPWM(void){
	initPIN_OUTPUT();
	initTIM3_CH2();
	initButton();
}


/******** INICIALIZAR EL PA7 ********/
static void initPIN_OUTPUT(void){
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Pin = PIN;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
	
	HAL_GPIO_Init(PORT, &GPIO_InitStruct);
}

/******** INICIALIZAR EL TIM3-CH2 ********/ 
static void initTIM3_CH2(void){
	
	__HAL_RCC_TIM3_CLK_ENABLE();

	// Configuración del Timer-3
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 83;			// 84 MHz / 84 = 1 MHz (TIM3 clk 84 MHz); cada cuenta dura 1 us
	htim3.Init.Period = 499;				// para una freq. de 2 KHz -> 1 MHz / p = freq -> p = 1MHz/2kHz = 500 ciclos
	HAL_TIM_PWM_Init(&htim3);
	
	//Configuración del Channel-2
	TIM_Channel_InitStruct.OCMode = TIM_OCMODE_PWM1;
	TIM_Channel_InitStruct.Pulse = pulse;		
	TIM_Channel_InitStruct.OCPolarity = TIM_OCPOLARITY_HIGH;
	TIM_Channel_InitStruct.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(&htim3, &TIM_Channel_InitStruct, CHANNEL);
	
	HAL_TIM_PWM_Start(&htim3, CHANNEL);
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

/******** CALLBACK PARA VARIAR EL CICLO DE TRABAJO ********/

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == PIN_BUTTON){
		if (pulse < 400){
			 pulse += 100;		
		}
		else{
			 pulse = 100;		
		}
		
		TIM_Channel_InitStruct.Pulse = pulse;		
		__HAL_TIM_SET_COMPARE(&htim3, CHANNEL, pulse);
		
	}
}

