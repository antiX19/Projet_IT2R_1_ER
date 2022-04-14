#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "GPIO_LPC17xx.h"               // Keil::Device:GPIO




extern ARM_DRIVER_USART Driver_USART1;







void Init_Pwm(void){ 
	
	// initialisation de timer 1
		
	LPC_SC->PCONP = LPC_SC->PCONP | 0x00000040; // enable PWM1

	// TC s'incrémente toutes les 480 ns
	LPC_PWM1->PR = 0; // prescaler

	LPC_PWM1->MR0 = 999; // Ceci ajuste la période de la PWM à 40 us
		

		
	// P3.26 est la sortie PWM Channel 3 de Timer 1
	LPC_PINCON->PINSEL7 = LPC_PINCON->PINSEL7 | 0x000C0000;
	LPC_PWM1->MCR = LPC_PWM1->MCR |0x00000002; // Timer relancé quand MR0 repasse à 0
	// ceci donne le droit de modifier dynamiquement la valeur du rapport cyclique
	LPC_PWM1->LER = LPC_PWM1->LER | 0x00000009;
	LPC_PWM1->PCR = LPC_PWM1->PCR | 0x00000e00; // autorise la sortie PWM
	LPC_PWM1->TCR = 1; /*validation de timer 1 et reset counter */
}



void Init_TIM0(void){
	LPC_GPIO3->FIODIR3 = 	LPC_GPIO3->FIODIR3 | 0x04;
	LPC_GPIO3->FIOPIN3 |= 0x04;

	LPC_SC->PCONP = LPC_SC->PCONP | 0x000000006;  //Enable TIM0
	LPC_TIM0->PR = 4;
	LPC_TIM0->MR0 = 8344;
	LPC_TIM0->MCR = LPC_TIM0->MCR | 0x00000003; //réglage RAZ
	LPC_TIM0->TCR = 1 ;
	
	NVIC_SetPriority(TIMER0_IRQn,0);
	NVIC_EnableIRQ(TIMER0_IRQn);
}

void Init_Moteur(void){
	
	LPC_GPIO1->FIODIR3 = LPC_GPIO1->FIODIR3 & 0xFA;
	LPC_GPIO0->FIODIR2 = LPC_GPIO0->FIODIR2 | 0x0F; 
	
	
}