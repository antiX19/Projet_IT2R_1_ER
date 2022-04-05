



void initPwm(void);


/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
// Author: Clément Loiseau
// Date: 15/03/2022






#define osObjectsPublic                     // define objects in main module
//#include "osObjects.h"                      // RTOS object definitions


//#include <cmsis_os.h>                   // ARM::CMSIS:RTOS:Keil RTX

//#include "LPC17xx.h"                    // Device header
#include "GPIO_LPC17xx.h"               // Keil::Device:GPIO

#include "PIN_LPC17xx.h"                // Keil::Device:PIN
#include "GPIO.h"

int etatbot = 0;
int etat = 0;
char etatpin = 0;

void MoteurAV(uint32_t vitesse);

void MoteurAR(uint32_t vitesse);


int main (void){
	
	//ENA = 0.19  GPIO0->FIODIR2 a partir de 16
	//INA = 0.16	GPIO0->FIODIR2 a partir de 16
	//ENB = 0.18
	//INB = 0.17

	initPwm();

	LPC_GPIO1->FIODIR3 &= 0xFA;
	
	LPC_GPIO0->FIODIR2 = LPC_GPIO0->FIODIR2 | 0x0F; 
	
	
	while(1){
		etatpin = LPC_GPIO0->FIOPIN2;
		etatbot = LPC_GPIO1->FIOPIN3 & 0x05;
		
		switch(etat){
			case 0:
				MoteurAV(50);
			
				if(etatbot == 0x04){etat = 1;}
				break;
				
			case 1:
				MoteurAR(50);
			
				if(etatbot == 0x01){etat = 0;}
				
				break;
		}
	}
		
}


void initPwm(void){ 
	
// initialisation de timer 1
	
LPC_SC->PCONP = LPC_SC->PCONP | 0x00000040; // enable PWM1

// TC s'incrémente toutes les 480 ns
LPC_PWM1->PR = 0; // prescaler

LPC_PWM1->MR0 =1249; // Ceci ajuste la période de la PWM à 40 us
	

	
// P3.26 est la sortie PWM Channel 3 de Timer 1
LPC_PINCON->PINSEL7 = LPC_PINCON->PINSEL7 | 0x000C0000;
LPC_PWM1->MCR = LPC_PWM1->MCR |0x00000002; // Timer relancé quand MR0 repasse à 0
// ceci donne le droit de modifier dynamiquement la valeur du rapport cyclique
LPC_PWM1->LER = LPC_PWM1->LER | 0x00000009;
LPC_PWM1->PCR = LPC_PWM1->PCR | 0x00000e00; // autorise la sortie PWM
LPC_PWM1->TCR = 1; /*validation de timer 1 et reset counter */
}




void MoteurAV(uint32_t vitesse)
{
	uint32_t vit=0;
	//LPC_GPIO0->FIOPIN2 = LPC_GPIO0->FIOPIN2 & 0xFC; // mets INA à 0
	LPC_GPIO0->FIOPIN2 = 0x0D; // mets INA à 0
	vit = vitesse *12;
	LPC_PWM1->MR2 = 100;
	LPC_PWM1->MR2 = vit;// ceci ajuste la duree de l'état haut

	
}

void MoteurAR(uint32_t vitesse)
{
	uint32_t vit=0;
	//LPC_GPIO0->FIOPIN2 = LPC_GPIO0->FIOPIN2 & 0xFC; // mets INA à 0
	LPC_GPIO0->FIOPIN2 = 0x0E; // mets INA à 0
	
	vit = vitesse *12;
	LPC_PWM1->MR2 = 100;
	LPC_PWM1->MR2 = vit;// ceci ajuste la duree de l'état haut

	
}
