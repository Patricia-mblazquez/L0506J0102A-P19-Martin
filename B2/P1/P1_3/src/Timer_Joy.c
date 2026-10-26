#include "Timer_Joy.h"
#include "Thjoy.h"


/*----------------------------------------------------------------------------
 *      Timer 'Timer_Joy': Temporización de 50 ms para el filtrado de rebotes
 *---------------------------------------------------------------------------*/
 
/******** VARIABLES ********/
osTimerId_t tim_joy;                            // timer id

/******** DECLARACIÓN DE FUNCIONES PRIVADAS ********/
static void Timer_Joy_Callback (void const *arg);

/*==========================================================
=                         FUNCIONES                        =
==========================================================*/

/******** CALLBACK DEL TIMER ********/
static void Timer_Joy_Callback (void const *arg) {
	(void)arg;
	
	osThreadFlagsSet(tid_Thjoy, 0x02);

}
 
/******** CREACIÓN DEL TIMER ********/
int Init_Timer_Joy (void) {
  
  tim_joy = osTimerNew((osTimerFunc_t)&Timer_Joy_Callback, osTimerOnce, NULL, NULL);
  if (tim_joy == NULL) {
    return -1;
  }
	
  return NULL;
}
