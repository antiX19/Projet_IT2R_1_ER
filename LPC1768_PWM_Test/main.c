#include "GPIO_LPC17xx.h"               // Keil::Device:GPIO

#include "PIN_LPC17xx.h"                // Keil::Device:PIN


void initPwm(void);

int main (void){
	
	initPwm();

	LPC_PWM1->MR2 = 500;
	LPC_PWM1->MR3 = 900;
		
}

void initPwm(void){ 
	
// initialisation de timer 1
	
LPC_SC->PCONP = LPC_SC->PCONP | 0x00000040; // enable PWM1

// TC s'incrémente toutes les 480 ns
LPC_PWM1->PR = 0; // prescaler

LPC_PWM1->MR0 =1249; // Ceci ajuste la période de la PWM à 40 us
	

	
// P3.26 est la sortie PWM Channel 3 de Timer 1
LPC_PINCON->PINSEL7 = LPC_PINCON->PINSEL7 | 0x000C0000;
LPC_PINCON->PINSEL7 = LPC_PINCON->PINSEL7 |0x00300000;
	
LPC_PWM1->MCR = LPC_PWM1->MCR |0x00000002; // Timer relancé quand MR0 repasse à 0
// ceci donne le droit de modifier dynamiquement la valeur du rapport cyclique
LPC_PWM1->LER = LPC_PWM1->LER | 0x00000009;
LPC_PWM1->PCR = LPC_PWM1->PCR | 0x00000e00; // autorise la sortie PWM
LPC_PWM1->TCR = 1; /*validation de timer 1 et reset counter */
}
