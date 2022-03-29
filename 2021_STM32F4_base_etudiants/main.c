/**
  ******************************************************************************
  * @file    Templates/Src/main.c 
  * @author  MCD Application Team
  * @brief   STM32F4xx HAL API Template project 
  *
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Board_LED.h"                  // ::Board Support:LED
#include "Driver_SPI.h"
#include "stm32f4xx_hal.h"

extern ARM_DRIVER_SPI Driver_SPI1;

void Configure_GPIO(void);
void configure_ADC2_Channel_1(void);
void Delay_ms(volatile int time_ms);
void clignogauche(void);
void clignodroit(void);
void phare (void);
ADC_HandleTypeDef myADC2Handle;

char tab[22+24];

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
  
  int i, nb_led;
  
  Init_SPI();
  
	Configure_GPIO(); // initialize PA0 pin 
	configure_ADC2_Channel_1(); // configure ADC2
	LED_Initialize();

  while(1){
		
		for (i=0;i<4;i++)
			{
			tab[i] = 0;
			}
	
		phare();
		clignogauche ();	
		clignodroit ();
		
		// end
		tab[44] = 0;
		tab[45] = 0;
		
		//envoie de la trame
		Driver_SPI1.Send(tab,46);
	    
  }
}

void phare (void)
		{
		uint32_t Adc_value;
		float voltage;
		int nb_led;
			
		HAL_ADC_Start(&myADC2Handle); // start A/D conversion
	  
	  	if(HAL_ADC_PollForConversion(&myADC2Handle, 5) == HAL_OK) //check if conversion is completed
			{
			Adc_value  = HAL_ADC_GetValue(&myADC2Handle); // read digital value and save it inside uint32_t variable
			voltage = (Adc_value * 8 ) / 10000;
			}
			
		HAL_ADC_Stop(&myADC2Handle); // stop conversion 
		Delay_ms(200);
		
		
		for (nb_led = 0; nb_led <4;nb_led++)
			{
				tab[4+nb_led*4]=0xff;
				tab[5+nb_led*4]=0x00;
				tab[6+nb_led*4]=0x0E;
				tab[7+nb_led*4]=0x0E;
			}

		if( voltage < 2.5)
		{
		for (nb_led = 0; nb_led <4;nb_led++)
				{
					tab[4+nb_led*4]=0xff;
					tab[5+nb_led*4]=0x00;
					tab[6+nb_led*4]=0xff;
					tab[7+nb_led*4]=0xff;
				}
			}	
		}
void clignogauche(void)
		{
		int nb_led;
        
			
		for (nb_led = 0; nb_led <3;nb_led++){
			tab[20+nb_led*4]=0xff;
			tab[21+nb_led*4]=0x00;
			tab[22+nb_led*4]=0x00;
			tab[23+nb_led*4]=0x00;
			}
		
		for (nb_led = 0; nb_led <3;nb_led++){
			tab[32+nb_led*4]=0xff;
			tab[33+nb_led*4]=0x00;
			tab[34+nb_led*4]=0x3f;
			tab[35+nb_led*4]=0xff;
			}
		}

	
void clignodroit(void)
		{
		int nb_led;
        
		for (nb_led = 0; nb_led <3;nb_led++){
			tab[20+nb_led*4]=0xff;
			tab[21+nb_led*4]=0x00;
			tab[22+nb_led*4]=0x3f;
			tab[23+nb_led*4]=0xff;
			}
		
		for (nb_led = 0; nb_led <3;nb_led++){
			tab[32+nb_led*4]=0xff;
			tab[33+nb_led*4]=0x00;
			tab[34+nb_led*4]=0x00;
			tab[35+nb_led*4]=0x00;
			}
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
