/* Includes & Defines------------------------------------------------------------------*/
#ifndef __MAIN_H

#define __MAIN_H
#define osObjectsPublic
#define long_tab 54
// define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "Board_LED.h"                  // ::Board Support:LED
#include "Driver_SPI.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

ADC_HandleTypeDef myADC2Handle;
// thread function
osThreadId ID_LED_Thread;                                          // thread id
osThreadDef (LED, osPriorityNormal, 1, 0);                   // thread object


/* extern------------------------------------------------------------------*/
extern ARM_DRIVER_SPI Driver_SPI1;
/* Exported functions ------------------------------------------------------- */
void configure_ADC2_Channel_1(void)
{
	ADC_ChannelConfTypeDef Channel_AN0; // create an instance of ADC_ChannelConfTypeDef
__HAL_RCC_ADC2_CLK_ENABLE(); // enable clock to ADC2 module
	myADC2Handle.Instance = ADC2; // create an instance of ADC2
	myADC2Handle.Init.Resolution = ADC_RESOLUTION_12B; // select 12-bit resolution 
	myADC2Handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV; //select  single conversion as a end of conversion event
	myADC2Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT; // set digital output data right justified
	myADC2Handle.Init.ClockPrescaler =ADC_CLOCK_SYNC_PCLK_DIV8; 
	HAL_ADC_Init(&myADC2Handle); // initialize AD2 with myADC2Handle configuration settings
	
  /*select ADC2 channel */
	
	Channel_AN0.Channel = ADC_CHANNEL_0; // select analog channel 0
	Channel_AN0.Rank = 1; // set rank to 1
	Channel_AN0.SamplingTime = ADC_SAMPLETIME_15CYCLES; // set sampling time to 15 clock cycles
	HAL_ADC_ConfigChannel(&myADC2Handle, &Channel_AN0); // select channel_0 for ADC2 module. 
}

void Configure_GPIO(void)
{
	GPIO_InitTypeDef ADCpin; //create an instance of GPIO_InitTypeDef C struct
	__HAL_RCC_GPIOA_CLK_ENABLE(); // enable clock to GPIOA
	ADCpin.Pin = GPIO_PIN_0; // Select pin PA1
	ADCpin.Mode = GPIO_MODE_ANALOG; // Select Analog Mode
	ADCpin.Pull = GPIO_NOPULL; // Disable internal pull-up or pull-down resistor
	HAL_GPIO_Init(GPIOA, &ADCpin); // initialize PA1 as analog input pin
}

void phare (char phares [])	// Fonction d'allumage des phares
	{
		uint32_t Adc_value;
		double voltage;
		int nb_led;
		HAL_ADC_Start(&myADC2Handle); // start A/D conversion
	  
	  	if(HAL_ADC_PollForConversion(&myADC2Handle, 5) == HAL_OK) //check if conversion is completed
			{
			Adc_value  = HAL_ADC_GetValue(&myADC2Handle); // read digital value and save it inside uint32_t variable
			voltage = (Adc_value * 8 ) / 10000;
			}
		HAL_ADC_Stop(&myADC2Handle); // stop conversion 	
			
		for (nb_led = 0; nb_led <4;nb_led++)
				{
					phares[4+nb_led*4]=0xff;	//Allumage "standard" des phares 
					phares[5+nb_led*4]=0x00;
					phares[6+nb_led*4]=0x0e;
					phares[7+nb_led*4]=0x0e;
				}
		if( voltage <= 2.5)
		{
		for (nb_led = 0; nb_led <4;nb_led++)
				{
					phares[4+nb_led*4]=0xff;	//Allumage plein phares
					phares[5+nb_led*4]=0x00; 
					phares[6+nb_led*4]=0xff;
					phares[7+nb_led*4]=0xff;
				}
		}
}
	
void LED (void const *argument) {
	osEvent event;
	char tab[long_tab];
	int i;
  while (1) {
	  for (i=0;i<4;i++)	// 4 octets de début de trame
			{
			tab[i] = 0;
			}
	phare(tab);	//Allumer phares -> si déclaré = OUI
	tab[52] = 0;	// 1er octet de fin de trame 
	tab[53] = 0;	// 2e octet de fin de trame 
    event = osSignalWait(0x01, osWaitForever);	// sommeil fin emission
		osDelay(1000);		
  }
}
void Init_SPI(void)
	  {
		Driver_SPI1.Initialize(NULL);
		Driver_SPI1.PowerControl(ARM_POWER_FULL);
		Driver_SPI1.Control(ARM_SPI_MODE_MASTER | 
											ARM_SPI_CPOL1_CPHA1 | 
											ARM_SPI_MSB_LSB | 
											ARM_SPI_SS_MASTER_UNUSED |
											ARM_SPI_DATA_BITS(8), 1000000);
		Driver_SPI1.Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
		 
	  }
	  
void mySPI_callback(uint32_t event)
{
	switch (event) {
		case ARM_SPI_EVENT_TRANSFER_COMPLETE  : 	 osSignalSet(ID_LED_Thread, 0x01);
		break;
		default : break;
	}
}

#endif 