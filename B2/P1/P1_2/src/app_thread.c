#include "app_thread.h"
#include "app_timer.h"

/*----------------------------------------------------------------------------
 *      Thread 'app_thread': Control de flujo principal y reinicio del temporizador
 *---------------------------------------------------------------------------*/

/******** VARIABLES ********/
osThreadId_t tid_App_Thread;                        // thread id

/******** DECLARACIÓN DE FUNCIONES PRIVADAS ********/
static void App_Thread (void *argument);                   // thread function
 
/*==========================================================
=                         FUNCIONES                        =
==========================================================*/

/******** CREACIÓN DEL HILO ********/
int Init_App_Thread (void) {
 
  tid_App_Thread = osThreadNew(App_Thread, NULL, NULL);
  if (tid_App_Thread == NULL) {
    return(-1);
  }
 
  return(0);
}

/******** FUNCIÓN DEL HILO ********/
static void App_Thread (void *argument) {
	(void)argument;		// Silencia el warning de 'unused parameter'
	
	// Inicialización de periféricos
	B1_Reset_Init();
	LED_Init(LED_GREEN);
	LED_Init(LED_RED);
	
	// Estado inicial
	LED_Off(LED_RED);
	LED_On(LED_GREEN);
	
	// Arranca la primera cuenta de 3 segundos
	osTimerStart(tim_3s, 3000); 
 
  while (1) {
		osThreadFlagsWait(0x0001, osFlagsWaitAll, osWaitForever);
		
		// Si se pulsa el botón y el sistema no está bloqueado, se reinicia la cuenta
		if(!system_blocked){
			osTimerStart(tim_3s, 3000); 
		}
  }
}
