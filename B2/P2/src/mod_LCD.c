#include "cmsis_os2.h"                          // CMSIS RTOS header file
 
/*----------------------------------------------------------------------------
 *      Thread 'mod_LCD': Plantilla
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_Th_LCD;  
osThreadId_t tid_Th_LCD_test; 
 
static void Th_LCD (void *argument);
static void Th_LCD_test (void *argument);

/******** Thread de LCD ********/
int Init_Th_LCD (void) {
 
  tid_Th_LCD = osThreadNew(Th_LCD, NULL, NULL);
  if (tid_Th_LCD == NULL) {
    return(-1);
  }
 
  return(0);
}
 
static void Th_LCD (void *argument) {
 (void)argument;
	
	// Llamadas a las funciones de inicialización
	
  while (1) {
    // Esperar un flag desde el thread de test
		osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
		
		// Representar en el LCD la información
		
  }
}

/******** Thread de TEST ********/
int Init_Th_LCD_test (void) {
 
  tid_Th_LCD_test = osThreadNew(Th_LCD_test, NULL, NULL);
  if (tid_Th_LCD_test == NULL) {
    return(-1);
  }
 
  return(0);
}
 
static void Th_LCD_test (void *argument) {
 (void)argument;
	
  while (1) {
		// Esperar 1 segundo
		osDelay(1000);
		
		// Enviar una señal a th_LED para representar información en el LCD
		osThreadFlagsSet(tid_Th_LCD, 0x01);
  }
}
