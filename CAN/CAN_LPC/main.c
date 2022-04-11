/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "Driver_CAN.h"                 // ARM::CMSIS Driver:CAN:Custom
#include "LPC17xx.h"                    // Device header
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "RTE_Device.h"                 // Keil::Device:Startup
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GPIO_LPC17xx.h"               // Keil::Device:GPIO
#include "stdio.h"

#define ID_Capteur_US			0x008
#define ID_GPS_latitude		0x040
#define ID_GPS_longitude	0x042
#define ID_GPS_horodatage	0x044
#define ID_Moto_Dir		    0x400

extern   ARM_DRIVER_CAN  Driver_CAN1;

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

osThreadId  ID_CANthreadT;
osThreadId  ID_CANthreadR;

void CANthreadT(void const *argument)
{
	ARM_CAN_MSG_INFO                tx_msg_info;
	char data_buf[8];
	int i;
	
	while (1) 
		{
		tx_msg_info.id  = ARM_CAN_STANDARD_ID(ID_Capteur_US	);           // ID CAN exemple
		tx_msg_info.rtr = 0;// 0 = trame DATA
		for(i = 0; i < 8; i++)data_buf[i]=0;
		data_buf[0] = 0x82;// data à envoyer exemple
		Driver_CAN1.MessageSend(1, &tx_msg_info, data_buf, 1); // envoi
		/* Sommeil sur fin envoi */
		osSignalWait(0x01, osWaitForever);
		osDelay(100);
		}
}

void CANthreadR(void const *argument)
{
		ARM_CAN_MSG_INFO                rx_msg_info;
		char data_buf[8];
		char nb_data;
		char tab[50];
		osEvent evt;// si besoin de gestion du retour (ex : Timeout)...
		while (1) 
			{
				
				evt = osSignalWait(0x01, osWaitForever);// sommeil sur attente réception
				Driver_CAN1.MessageRead(0, &rx_msg_info, data_buf, 8);// 8 data max
				// on traite la trame reçue
				nb_data = rx_msg_info.dlc ;// récupération nb_data
				
				sprintf(tab,"Caract recu : %x",data_buf[0]);	
				GLCD_DrawString(1,1,tab);	// colonne puis ligne en pixel
			}
}

void myCAN1_callback(uint32_t obj_idx, uint32_t event)// arguments imposés
{
		switch (event)
		{
			case ARM_CAN_EVENT_SEND_COMPLETE:  
				osSignalSet(ID_CANthreadT, 0x01);
				break;
			case ARM_CAN_EVENT_RECEIVE:       
				osSignalSet(ID_CANthreadR, 0x01);
				break;
		}
}


void InitCAN1(){
	Driver_CAN1.Initialize(NULL,myCAN1_callback);
	Driver_CAN1.PowerControl(ARM_POWER_FULL);
	Driver_CAN1.SetMode(ARM_CAN_MODE_INITIALIZATION);
	Driver_CAN1.SetBitrate(ARM_CAN_BITRATE_NOMINAL,    // débit fixe
												 125000,                     // 125 kbits/s (LS)
												 ARM_CAN_BIT_PROP_SEG(5U)   |   //  prop. seg = 5 TQ
												 ARM_CAN_BIT_PHASE_SEG1(1U) | // phase seg1 = 1 TQ
												 ARM_CAN_BIT_PHASE_SEG2(1U) | // phase seg2 = 1 TQ
												 ARM_CAN_BIT_SJW(1U));          // Resync. Seg = 1 TQ
	
	Driver_CAN1.ObjectConfigure(1,ARM_CAN_OBJ_TX); // Objet 1 pour émission
	Driver_CAN1.ObjectConfigure(0,ARM_CAN_OBJ_RX); // Objet 0 pour réception
	Driver_CAN1.SetMode(ARM_CAN_MODE_NORMAL); // fin initialisation
	Driver_CAN1.ObjectSetFilter(0, ARM_CAN_FILTER_ID_EXACT_ADD ,ARM_CAN_STANDARD_ID(ID_Capteur_US),0);
	
	Driver_CAN1.Initialize(NULL,myCAN1_callback);
}

osThreadDef (CANthreadT, osPriorityNormal, 1, 0);
osThreadDef (CANthreadR, osPriorityNormal, 1, 0);
/*
 * main: initialize and start the system
 */
int main (void) {
  osKernelInitialize ();  // initialize CMSIS-RTOS

  // initialize peripherals here
		InitCAN1();
		GLCD_Initialize();
		GLCD_ClearScreen();
    GLCD_SetFont(&GLCD_Font_16x24);
  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	ID_CANthreadT=osThreadCreate (osThread (CANthreadT), NULL);
	ID_CANthreadR=osThreadCreate (osThread (CANthreadR), NULL);


  osKernelStart ();                         // start thread execution 
	osDelay(osWaitForever);
}
