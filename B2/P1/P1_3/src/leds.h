
#ifndef LEDS_H
#define LEDS_H

#include "stm32f4xx_hal.h"

// Números de LED según la placa
#define LED1 0
#define LED2 1
#define LED3 2

// Alias de colores
#define LED_GREEN 	LED1
#define LED_BLUE		LED2
#define LED_RED			LED3

// Funciones
void LEDS_Init(void);
void LED_Init(uint8_t led);
void LEDS_On(void);
void LED_On(uint8_t led);
void LEDS_Off(void);
void LED_Off(uint8_t led);
void LED_Toggle(uint8_t led);
void LEDS_Show(uint8_t value);

#endif
