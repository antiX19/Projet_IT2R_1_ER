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



void PWM_Servo (void const * argument);
void Pilotage (void const* argument);
void TIMER0_IRQHandler(void);
void Init_TIM0(void);
void Moteur_AV(uint32_t vitesse);
void Moteur_AR(uint32_t vitesse);
void Callback_UART(uint32_t event);
void Callback_BT(uint32_t 	event);
void Reception_BT(void const* argument);
void Error_BT(void const* argument);


//VAL MIN = 5500
//VAL MOY = 7500
//VAL mAX = 9500


uint8_t tab[6];
uint32_t temps_haut = 9800;
uint32_t temps_tot = 110200;
bool TIMER = 1;
bool Debug = 0;
int test = 0;
char first_val[1];
bool verif = 1;
int compteur = 0;



osThreadId  ID_Pilotage,ID_Reception_BT,ID_Error_BT;




osThreadDef (Pilotage , osPriorityNormal, 1, 0);
osThreadDef (Reception_BT , osPriorityNormal, 1, 0);
osThreadDef (Error_BT , osPriorityNormal, 1, 0);






int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	Init_UART();
	Init_Pwm();
	Init_TIM0();
	Init_Moteur();
	
	
	LPC_TIM0->MR0 = temps_haut;
	

	ID_Pilotage=osThreadCreate (osThread (Pilotage), NULL);
	ID_Reception_BT=osThreadCreate (osThread (Reception_BT), NULL);
	ID_Error_BT=osThreadCreate (osThread (Error_BT), NULL);
	
	
  osKernelStart ();                         // start thread execution
	osDelay(osWaitForever);
}











void Pilotage (void const* argument){
	int i;
	char text[50];
	uint32_t status;

	int32_t vitesse_moteur;
	
	first_val[0] = 0xaa;
	
	while(1) {		
		
		
		//174 val median
		//X de 105 à 254
		//Y de 105 a 255
		
		if (tab[0]>174){
			temps_haut =  -25*tab[0] + 11850;
		}
		
		else if (tab[0]<174){
			temps_haut = -12*tab[0] + 9622;
		}
		
		if (tab[1]>165){
			vitesse_moteur = tab[1]-150;
			if(vitesse_moteur < 1){vitesse_moteur = 0;} 
			//if(vitesse_moteur > 100){vitesse_moteur = 100;}
			Moteur_AR(vitesse_moteur);
		}
		
		else if (tab[1]<140){
			vitesse_moteur = 100-tab[1];
			if(vitesse_moteur < 1){vitesse_moteur = 0;} 
			//if(vitesse_moteur > 100){vitesse_moteur = 100;} 
			Moteur_AV(vitesse_moteur);
		}
		else{
			vitesse_moteur = 0;
			Moteur_AV(vitesse_moteur);
		}
		
		//sprintf(text,": t_haut = %4d", temps_haut);
		//GLCD_DrawString(1,4*24, text);
		
		
		/*
		if (tab[1]>174){
			vitesse_moteur = 1.25 * tab[1] - 217;
			Moteur_AR(vitesse_moteur);
		}
		
		else if (tab[1]<174){
			vitesse_moteur = -0.6098*tab[1] + 106;
			Moteur_AV(vitesse_moteur);
		}*/
		
		
		
		if(Debug == 1){
			
			sprintf(text,"X = %3d               ", tab[0]);
			GLCD_DrawString(1,1*24, text);
			
			sprintf(text,"Y = %3d", tab[1]);
			GLCD_DrawString(1,2*24, text);
			
			sprintf(text,"ZC = %2d", tab[2]);
			GLCD_DrawString(1,3*24, text);
				
			sprintf(text,"vm = %3d", vitesse_moteur);
			GLCD_DrawString(1,5*24, text);
		}
		
		
	}
}

void Callback_BT(uint32_t 	event){

	if(event == 2){
		osSignalSet(ID_Reception_BT,0x0001);
	}
	
	if(event == 32){
		osSignalSet(ID_Error_BT,0x0002);
	}

	


	
	
}

void Reception_BT(void const* argument){
	uint8_t tab_test[6];
	char text[50];
	int i;
	
	while(1){
		
		osSignalWait(0x0001,osWaitForever);
		
		for(i=0;i<5;i++){
			tab_test[i] = 0x00;
		}
		
		
		
		Driver_USART1.Receive(tab_test,6);
		while(Driver_USART1.GetRxCount()<6);
		if((tab_test[0] == 0x0F) & (tab_test[1] == 0xF0)){
			tab[0] = tab_test[2];
			tab[1] = tab_test[3];
			tab[2] = tab_test[4];
			sprintf(text,"X = %3d               ", tab[0]);
			GLCD_DrawString(1,1*24, text);
			
			sprintf(text,"Y = %3d", tab[1]);
			GLCD_DrawString(1,2*24, text);
			
			sprintf(text,"ZC = %3d", tab[2]);
			GLCD_DrawString(1,3*24, text);
			
		}
		
			
		
		
		
		
	}
}

void Error_BT(void const* argument){
	char text[50];
	uint8_t poubelle[200];
	int compteur2 = 0;
	while(1){
		
		osSignalWait(0x0002,osWaitForever);


		Driver_USART1.Receive(poubelle,200);
		while(Driver_USART1.GetRxCount()<200);
		compteur2++;
	}
	
}









void TIMER0_IRQHandler(void){
	LPC_TIM0->IR = LPC_TIM0->IR  | (1<<0);
	LPC_GPIO3->FIOPIN3 = LPC_GPIO3->FIOPIN3 ^ 0x04; //inverse l'état de P3,5
	
	if(TIMER == 1){
	 LPC_TIM0->MR0 = (temps_tot-temps_haut);
		TIMER = 0;
	}
	else if(TIMER == 0){
	 LPC_TIM0->MR0 = temps_haut;
		TIMER = 1;
	}
 }

 
 
void Moteur_AV(uint32_t vitesse)
{
	uint32_t vit=0;
	//LPC_GPIO0->FIOPIN2 = LPC_GPIO0->FIOPIN2 & 0xFC; // mets INA à 0
	LPC_GPIO0->FIOPIN2 = 0x0D; // mets INA à 0
	vit = vitesse *10;
	//LPC_PWM1->MR2 = 100;
	LPC_PWM1->MR2 = vit;// ceci ajuste la duree de l'état haut

	
}

void Moteur_AR(uint32_t vitesse)
{
	uint32_t vit=0;
	//LPC_GPIO0->FIOPIN2 = LPC_GPIO0->FIOPIN2 & 0xFC; // mets INA à 0
	LPC_GPIO0->FIOPIN2 = 0x0E; // mets INA à 0
	
	vit = vitesse *10;
	//LPC_PWM1->MR2 = 100;
	LPC_PWM1->MR2 = vit;// ceci ajuste la duree de l'état haut

	
}





void Init_UART(void){
	Driver_USART1.Initialize(Callback_BT);
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



