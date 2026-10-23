#ifndef APP_TIMER_H
#define APP_TIMER_H

#include "cmsis_os2.h"
#include "stdbool.h"
#include "leds.h"

// Variables
extern osTimerId_t tim_3s;
extern volatile bool system_blocked;

// Función
int Init_App_Timers(void);

#endif
