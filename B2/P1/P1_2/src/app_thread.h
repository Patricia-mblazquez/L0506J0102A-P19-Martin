#ifndef APP_THREAD_H
#define APP_THREAD_H

#include "cmsis_os2.h"
#include "leds.h"
#include "B1_reset.h"

// Variable
extern osThreadId_t tid_App_Thread;

// Función
int Init_App_Thread (void);

#endif
