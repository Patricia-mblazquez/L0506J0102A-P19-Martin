#include "Thjoy.h"
#include "Timer_Joy.h"
#include "stdbool.h"

/*----------------------------------------------------------------------------
 *      Thread 'Thjoy': Procesado de interrupciones del joystick y actualización de LEDs
 *---------------------------------------------------------------------------*/
 
/******** VARIABLES ********/
osThreadId_t tid_Thjoy;                        // thread id

uint32_t contador_gestos[5] = {0};
uint8_t contador = 0;
uint8_t valor_joystick = 0;

/******** DECLARACIÓN DE FUNCIONES PRIVADAS ********/
static void Thjoy (void *argument);                   // thread function
static void Joystick_Init(void);
static uint8_t Joystick_Read(void);

/*==========================================================
=                         FUNCIONES                        =
==========================================================*/

/******** CREACIÓN DEL HILO ********/
int Init_Thjoy (void) {
 
  tid_Thjoy = osThreadNew(Thjoy, NULL, NULL);
  if (tid_Thjoy == NULL) {
    return(-1);
  }
 
  return(0);
}

/******** FUNCIÓN DEL HILO ********/
static void Thjoy (void *argument) {
	(void)argument;
	
	uint32_t flags;
 
	Joystick_Init();
	LEDS_Init();
	Init_Timer_Joy();
	
  while (1) {
    flags = osThreadFlagsWait(0x01 | 0x02, osFlagsWaitAny, osWaitForever);
		
		if (flags & 0x01){
			osTimerStart(tim_joy, 50); 
		}
		
		if (flags & 0x02){
			valor_joystick = Joystick_Read();
			
			if(valor_joystick != 0){
				LEDS_Show(valor_joystick);
				contador_gestos[valor_joystick - 1]++;
				contador++;
			}
		} 
  }
}

/******** INICIALIZACIÓN DEL JOYSTICK ********/
static void Joystick_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct;

	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;	
	
	GPIO_InitStruct.Pin = PIN_UP | PIN_RIGHT;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = PIN_DOWN | PIN_LEFT | PIN_CENTER;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/******** LECTURA DEL JOYSTICK ********/
static uint8_t Joystick_Read(void){
	if(HAL_GPIO_ReadPin(PORT_UP,		PIN_UP) == GPIO_PIN_SET) 			return 1;
	if(HAL_GPIO_ReadPin(PORT_RIGHT,	PIN_RIGHT) == GPIO_PIN_SET) 	return 2;
	if(HAL_GPIO_ReadPin(PORT_DOWN, 	PIN_DOWN) == GPIO_PIN_SET) 		return 3;
	if(HAL_GPIO_ReadPin(PORT_LEFT, 	PIN_LEFT) == GPIO_PIN_SET) 		return 4;
	if(HAL_GPIO_ReadPin(PORT_CENTER,PIN_CENTER) == GPIO_PIN_SET) 	return 5;
	
	return 0;
		
}
