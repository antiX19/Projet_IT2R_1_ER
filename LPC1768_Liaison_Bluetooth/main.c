/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD

extern ARM_DRIVER_USART Driver_USART1;
extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

osThreadId  ID_receptionBT;

void receptionBT (void const* argument){
	uint8_t tab[3];
	char menu[30]={"Reception:"};
	char text[50], text2[50];
	
	while(1) {
		while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
		Driver_USART1.Receive(tab,3);
		
		sprintf(text,"valeurs: X=%2x, Y=%2x", tab[0], tab[1]);
		sprintf(text2,"valeurs: ZC=%2x", tab[2]);
		GLCD_DrawString(1,1*24, menu);
		GLCD_DrawString(1,2*24, text);
		GLCD_DrawString(1,3*24, text2);
		
		//osDelay(10);
	}
}

void Init_UART(void){
	Driver_USART1.Initialize(NULL);
	Driver_USART1.PowerControl(ARM_POWER_FULL);
	Driver_USART1.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							115200);
	Driver_USART1.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART1.Control(ARM_USART_CONTROL_RX,1);
}

/*
 * main: initialize and start the system
 */
osThreadDef (receptionBT, osPriorityNormal, 1, 0);

int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	Init_UART();

  // initialize peripherals here

  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	
	ID_receptionBT=osThreadCreate (osThread (receptionBT), NULL);
	
  osKernelStart ();                         // start thread execution 
	osDelay(osWaitForever);
}
