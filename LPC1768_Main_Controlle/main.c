/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "stdio.h"
#include "GPIO_LPC17xx.h"               // Keil::Device:GPIO
#include "PIN_LPC17xx.h"                // Keil::Device:PIN
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "LPC17xx.h"                    // Device header
#include "Lib.h"



extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;
extern ARM_DRIVER_USART Driver_USART1;


void receptionBT (void const* argument);
void PWM_Servo (void const * argument);
void Pilotage (void const* argument);
void TIMER0_IRQHandler(void);
void Init_TIM0(void);
void Moteur_AV(uint32_t vitesse);
void Moteur_AR(uint32_t vitesse);


//VAL MIN = 3060
//VAL MOY = 4172
//VAL mAX = 5285
uint8_t tab[3];
uint32_t temps_haut = 3100;




osThreadId  ID_receptionBT,ID_Pilotage,ID_test;



osThreadDef (receptionBT, osPriorityNormal, 1, 0);
osThreadDef (Pilotage , osPriorityNormal, 1, 0);








int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	Init_UART();
	Init_Pwm();
	Init_TIM0();
	Init_Moteur();
	
	

	
	Moteur_AV(20);
	


	ID_receptionBT=osThreadCreate (osThread (receptionBT), NULL);
	//ID_Pilotage=osThreadCreate (osThread (Pilotage), NULL);

	
	
  osKernelStart ();                         // start thread execution
	osDelay(osWaitForever);
}









void receptionBT (void const* argument){
	char text[50];

	while(1) {		

		
		while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
		Driver_USART1.Receive(tab,3);
		
		
		GLCD_DrawString(1,1*24, "Reception :           ");
		sprintf(text,"valeurs: X=%2x, Y=%2x", tab[0], tab[1]);
		GLCD_DrawString(1,2*24, text);
		sprintf(text,"valeurs: Z = %2x", tab[2]);
		GLCD_DrawString(1,3*24, text);
		//osSignalSet(ID_Pilotage,0x0001);
		
	}
}




void Pilotage (void const* argument){
	while(1){
		osSignalWait(0x0001,osWaitForever);	
		osSignalSet(ID_receptionBT,0x0002);
	}
}





void TIMER0_IRQHandler(void){
	LPC_TIM0->IR = LPC_TIM0->IR  | (1<<0);
	LPC_GPIO3->FIOPIN3 = LPC_GPIO3->FIOPIN3 ^ 0x04; //inverse l'état de P3,5
	if(LPC_TIM0->MR0 == temps_haut){
	 LPC_TIM0->MR0 = (55600-temps_haut);
	}
	else if(LPC_TIM0->MR0 != temps_haut){
	 LPC_TIM0->MR0 = temps_haut;
	}
 }

 
 
void Moteur_AV(uint32_t vitesse)
{
	uint32_t vit=0;
	//LPC_GPIO0->FIOPIN2 = LPC_GPIO0->FIOPIN2 & 0xFC; // mets INA à 0
	LPC_GPIO0->FIOPIN2 = 0x0D; // mets INA à 0
	vit = vitesse *10;
	LPC_PWM1->MR2 = 100;
	LPC_PWM1->MR2 = vit;// ceci ajuste la duree de l'état haut

	
}

void Moteur_AR(uint32_t vitesse)
{
	uint32_t vit=0;
	//LPC_GPIO0->FIOPIN2 = LPC_GPIO0->FIOPIN2 & 0xFC; // mets INA à 0
	LPC_GPIO0->FIOPIN2 = 0x0E; // mets INA à 0
	
	vit = vitesse *10;
	LPC_PWM1->MR2 = 100;
	LPC_PWM1->MR2 = vit;// ceci ajuste la duree de l'état haut

	
}
