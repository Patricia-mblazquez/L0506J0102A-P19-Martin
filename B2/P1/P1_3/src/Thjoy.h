#ifndef THJOY_H
#define THJOY_H

#include "cmsis_os2.h"
#include "leds.h"

// Macros
#define PORT_UP			GPIOB			
#define PIN_UP			GPIO_PIN_10

#define PORT_RIGHT 	GPIOB
#define PIN_RIGHT		GPIO_PIN_11

#define PORT_DOWN		GPIOE
#define PIN_DOWN		GPIO_PIN_12

#define PORT_LEFT		GPIOE
#define PIN_LEFT		GPIO_PIN_14

#define PORT_CENTER	GPIOE
#define PIN_CENTER	GPIO_PIN_15

#define PINS_JOYSTICK (PIN_UP | PIN_RIGHT | PIN_DOWN | PIN_LEFT | PIN_CENTER)

// Variable
extern osThreadId_t tid_Thjoy;

// Función
int Init_Thjoy (void);

#endif