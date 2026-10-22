#ifndef THLED3_H
#define THLED3_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"


/******** DECLARACIÓN DE FUNCIONES ********/
int Init_Thled3(void);
 
/******** VARIABLES GLOBALES ********/
extern osThreadId_t tid_Thled3;

#endif