#ifndef INPUT_CAPTURE_H			// si no esta definido INPUT_CAPTURE_H
#define INPUT_CAPTURE_H			// lo define

#include "stdbool.h"
#include "stm32f4xx_hal.h"


// Funciones
void initInput_Capture(void);

// Variables
extern volatile uint32_t periodo;

#endif 
