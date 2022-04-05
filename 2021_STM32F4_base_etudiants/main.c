#include "Board_LED.h"                  // ::Board Support:LED
#include "Driver_SPI.h"
#include "stm32f4xx_hal.h"

extern ARM_DRIVER_SPI Driver_SPI1;

void Configure_GPIO(void);
void configure_ADC2_Channel_1(void);
void Delay_ms(volatile int time_ms);
void clignogauche(char LED_G [], int x);
void clignodroit(char LED_D [], int y);
void phare (char phares []);
void feux_de_recul (char recul []);

ADC_HandleTypeDef myADC2Handle;


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
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

int main(void)
{
  char tab[50];
  short pumpmyride;
  int i;
  Init_SPI();
  
	Configure_GPIO(); // initialize PA0 pin 
	configure_ADC2_Channel_1(); // configure ADC2
	LED_Initialize();

  while(1){
	  for (i=0;i<4;i++)
			{
			tab[i] = 0;
			}
		 phare(tab);
	     feux_de_recul (tab);
		clignogauche(tab, 3);
		//clignodroit(tab, 3);
	    tab[48] = 0;
		tab[49] = 0;
		
  }
}

void phare (char phares [])
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
		Delay_ms(120);
			
		
		for (nb_led = 0; nb_led <4;nb_led++)
				{
					phares[4+nb_led*4]=0xff;	//Allumage "normal" des phares 
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
		Driver_SPI1.Send(phares,50);	
}
		
void feux_de_recul (char recul [])
{
	    uint32_t Adc_value;
		double voltage;
			
		HAL_ADC_Start(&myADC2Handle); // start A/D conversion
	  
	  	if(HAL_ADC_PollForConversion(&myADC2Handle, 5) == HAL_OK) //check if conversion is completed
			{
			Adc_value  = HAL_ADC_GetValue(&myADC2Handle); // read digital value and save it inside uint32_t variable
			voltage = (Adc_value * 8 ) / 10000;
			}
			
		HAL_ADC_Stop(&myADC2Handle); // stop conversion 
		Delay_ms(120);
		
					recul[44]=0xff;	//Eteint feu de recul
					recul[45]=0x00;
					recul[46]=0x00;
					recul[47]=0x00;
		if( voltage <= 2.5)
		{
		
					recul[44]=0xff;	//Allumage feu de recul
					recul[45]=0x00;
					recul[46]=0x00;
					recul[47]=0xff;
				
		}
		Driver_SPI1.Send(recul,50);	
}
		

void clignogauche(char LED_G[],int x)
		{
		int nb_led, i,j;
						
	for (nb_led = 0; nb_led <x;nb_led++){
			LED_G[20+nb_led*4]=0xff;	
			LED_G[21+nb_led*4]=0x00;
			LED_G[22+nb_led*4]=0x00;
			LED_G[23+nb_led*4]=0x00;
			}
		
	for (nb_led = 0; nb_led <x;nb_led++){
			LED_G[32+nb_led*4]=0xff;	
			LED_G[33+nb_led*4]=0x00;
			LED_G[34+nb_led*4]=0x3f;
			LED_G[35+nb_led*4]=0xff;
		}
		Driver_SPI1.Send(LED_G,78);
		 for(i = 0; i < 400000; i++)
		{}
								

	for (nb_led = 0; nb_led <x;nb_led++){
			LED_G[32+nb_led*4]=0xff;
			LED_G[33+nb_led*4]=0x00;
			LED_G[34+nb_led*4]=0x00;
			LED_G[35+nb_led*4]=0x00;
		}
		Driver_SPI1.Send(LED_G,50);
		 for(j = 0; j < 300000; j++)
		{}
			
		
		
}
  
void clignodroit(char LED_D[],int y)
		{
		int nb_led, i,j;
						
	for (nb_led = 0; nb_led <y;nb_led++){
			LED_D[20+nb_led*4]=0xff;
			LED_D[21+nb_led*4]=0x00;
			LED_D[22+nb_led*4]=0x3f;
			LED_D[23+nb_led*4]=0xff;
			}
		
	for (nb_led = 0; nb_led <y;nb_led++){
			LED_D[32+nb_led*4]=0xe0;
			LED_D[33+nb_led*4]=0x00;
			LED_D[34+nb_led*4]=0x00;
			LED_D[35+nb_led*4]=0x00;
		}
		Driver_SPI1.Send(LED_D,78);
		 for(i = 0; i < 400000; i++)
		{}
								
	for (nb_led = 0; nb_led <y;nb_led++){
			LED_D[20+nb_led*4]=0xe0;
			LED_D[21+nb_led*4]=0x00;
			LED_D[22+nb_led*4]=0x00;
			LED_D[23+nb_led*4]=0x00;
			}
		
	
		Driver_SPI1.Send(LED_D,50);
		 for(j = 0; j < 300000; j++)
		{}
		}
/* Configuration and initialization PA1 pin as analog input pin for A/D conversion */
void Configure_GPIO(void)
{
	GPIO_InitTypeDef ADCpin; //create an instance of GPIO_InitTypeDef C struct
	__HAL_RCC_GPIOA_CLK_ENABLE(); // enable clock to GPIOA
	ADCpin.Pin = GPIO_PIN_0; // Select pin PA1
	ADCpin.Mode = GPIO_MODE_ANALOG; // Select Analog Mode
	ADCpin.Pull = GPIO_NOPULL; // Disable internal pull-up or pull-down resistor
	HAL_GPIO_Init(GPIOA, &ADCpin); // initialize PA1 as analog input pin
}
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
void Delay_ms(volatile int time_ms)
{
	      int j;
        for(j = 0; j < time_ms*4000; j++)
            {}  /* excute NOP for 1ms */		
}
