#include "LPC17xx.h"                    // Device header
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GPIO_LPC17xx.h"               // Keil::Device:GPIO
#include "GPIO.h"

extern ARM_DRIVER_USART Driver_USART1;


int arrayToAngle(char char1,char char2)
{
	int mesureAngle;
	mesureAngle = (int)char2<<7 | (int)char1>>1;
	return mesureAngle;
}



int arrayToRange(char char3,char char4)
{
	int mesureRange;
	mesureRange = (int)char4<<8 | (int)char3;
	return mesureRange;	
}



void Init_UART1(void){
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

void initPwm(void){ 
	


Initialise_GPIO(); // init GPIO
// initialisation de timer 1
	
	
LPC_SC->PCONP = LPC_SC->PCONP | 0x00000040; // enable PWM1

// TC s'incrémente toutes les 480 ns
LPC_PWM1->PR = 0; // prescaler

LPC_PWM1->MR0 =1000; // Ceci ajuste la période de la PWM à 40 us

// P3.26 est la sortie PWM Channel 3 de Timer 1
LPC_PINCON->PINSEL7 = LPC_PINCON->PINSEL7 |0x00300000;
LPC_PWM1->MCR = LPC_PWM1->MCR |0x00000002; // Timer relancé quand MR0 repasse à 0
// ceci donne le droit de modifier dynamiquement la valeur du rapport cyclique
LPC_PWM1->LER = LPC_PWM1->LER | 0x00000009;
LPC_PWM1->PCR = LPC_PWM1->PCR | 0x00000e00; // autorise la sortie PWM
LPC_PWM1->TCR = 1; /*validation de timer 1 et reset counter */
}


void Start_LIDAR(void){
	int TRAME_START = 0x20 ;
	int TRAME_STOP = 0x25 ;
	int TRAME_RESET = 0x40 ;
	int PRE_TRAME = 0xA5 ;
	char first[1]={0xAA};
	
	while(Driver_USART1.GetStatus().tx_busy == 1);
	Driver_USART1.Send(&PRE_TRAME,1);
		
	while(Driver_USART1.GetStatus().tx_busy == 1);
	Driver_USART1.Send(&TRAME_START,1);
	
	while(first[0] != 0x81){
		
		Driver_USART1.Receive(first,1);
		while(Driver_USART1.GetRxCount()<1);
	
	}
	
}



void Stop_LIDAR(void){
	int TRAME_START = 0x20 ;
	int TRAME_STOP = 0x25 ;
	int TRAME_RESET = 0x40 ;
	int PRE_TRAME = 0xA5 ;
	
	while(Driver_USART1.GetStatus().tx_busy == 1);
	Driver_USART1.Send(&PRE_TRAME,1);
		
	while(Driver_USART1.GetStatus().tx_busy == 1);
	Driver_USART1.Send(&TRAME_STOP,1);

}


void delay( unsigned long duration)
{
    while ( ( duration -- )!= 0);
}
