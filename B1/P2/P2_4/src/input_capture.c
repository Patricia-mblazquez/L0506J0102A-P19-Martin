
#include "input_capture.h"

/*==========================================================
=                           MACROS                          =
==========================================================*/

#define PIN 			GPIO_PIN_15
#define PORT			GPIOD
#define CHANNEL		TIM_CHANNEL_4

/*==========================================================
=                VARIABLES Y OBJETOS GLOBALES               =
==========================================================*/

// Variables para la inicialización
static GPIO_InitTypeDef GPIO_InitStruct;
TIM_HandleTypeDef htim4;
static TIM_IC_InitTypeDef TIM_Channel_InitStruct;

// Variables para la callback del input_capture
static bool primera_captura = false;
static uint32_t captura1;
static uint32_t captura2;
volatile uint32_t periodo;
volatile uint32_t frecuencia;

/*==========================================================
=                  DECLARACIÓN DE FUNCIONES                 =
==========================================================*/

void initInput_Capture(void);
static void initPIN_INPUT(void);
static void initTIM4_CH4(void);

/*==========================================================
=                         FUNCIONES                        =
==========================================================*/
 
void initInput_Capture(void){
	initPIN_INPUT();
	initTIM4_CH4();
}

/******** INICIALIZAR EL PD15 ********/

static void initPIN_INPUT(void){
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pin = PIN;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
	
	HAL_GPIO_Init(PORT, &GPIO_InitStruct);
}

/******** INICIALIZAR EL TIM4-CH4 ********/

static void initTIM4_CH4(void){
	
	__HAL_RCC_TIM4_CLK_ENABLE();
	HAL_NVIC_EnableIRQ(TIM4_IRQn);	// Habilita la interrupcion de TIM4
	
	// Configuración del Timer-4
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 83;		// TIM4 clk 84 MHz -> 84 MHz / (83+1) = 1 MHz -> Contador funciona a 1 MHz = 1 cuenta por 1 us	
	htim4.Init.Period = 0xFFFF;										// Contador de 16 bits: cuenta de 0 a 65535 y vuelve a 0 (aprovecha todo el rango)
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;	// El contador aumenta en cada cuenta
	HAL_TIM_IC_Init(&htim4);
	
	//Configuración del Channel-4
	TIM_Channel_InitStruct.ICFilter = 0;																	// Sin filtro
	TIM_Channel_InitStruct.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;	// Captura el flanco de subida
	TIM_Channel_InitStruct.ICPrescaler = TIM_ICPSC_DIV1;									// Cada cuantos eventos de captura se hace una captura -> En este caso captura todos los flancos de subida
	TIM_Channel_InitStruct.ICSelection = TIM_ICSELECTION_DIRECTTI;				// Conecta la captura a la entrada directa del canal 4
	HAL_TIM_IC_ConfigChannel(&htim4, &TIM_Channel_InitStruct, CHANNEL);
	
	HAL_TIM_IC_Start_IT(&htim4, CHANNEL);

}

/******** FUNCION CALLBACK DEL INPUT CAPTURE ********/
void input_capture_callback(void){
	if(!primera_captura){
		captura1 = HAL_TIM_ReadCapturedValue(&htim4, CHANNEL);
		primera_captura = true;
	}
	else{
		captura2 = HAL_TIM_ReadCapturedValue(&htim4, CHANNEL);
		
		if(captura2 >= captura1){
			// No ha habido desbordamiento: el periodo es la diferencia entre capturas
			periodo = captura2 - captura1;
		}
		else{
			// Ha habido un debordamiento: se tiene en cuenta el paso de 65535 a 0
			periodo = 0x10000 - captura1 + captura2;
		}
		
		frecuencia = 1000000 / periodo;
		captura1 = captura2;
	}
}

/******** Para llamar a la funcion callback ********/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM4 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4){
		input_capture_callback();
	}
}
