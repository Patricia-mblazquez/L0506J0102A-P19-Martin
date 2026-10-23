#include "app_timer.h"

/*----------------------------------------------------------------------------
 *      Timer 'app_timer': Temporización de 3 segundos y bloqueo del sistema
 *---------------------------------------------------------------------------*/
 
/******** VARIABLES ********/
osTimerId_t tim_3s;                            // timer id
volatile bool system_blocked = false;

/******** DECLARACIÓN DE FUNCIONES PRIVADAS ********/
static void Timer3s_Callback (void const *arg);

/*==========================================================
=                         FUNCIONES                        =
==========================================================*/

/******** CALLBACK DEL TIMER ********/
static void Timer3s_Callback (void const *arg) {
	(void)arg;		// Silencia el warning de 'unused parameter'
	
  LED_On(LED_RED);
	LED_Off(LED_GREEN);
	system_blocked = true;
}
 
/******** CREACIÓN DEL TIMER ********/
int Init_App_Timers (void) {
 
  tim_3s = osTimerNew((osTimerFunc_t)&Timer3s_Callback, osTimerOnce, NULL, NULL);
  if (tim_3s == NULL) {
     return -1;
  }
	
  return NULL;
}
