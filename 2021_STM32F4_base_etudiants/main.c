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
#include "main.h"
#include "Board_LED.h"                  // ::Board Support:LED
#include "Driver_SPI.h"
extern ARM_DRIVER_SPI Driver_SPI1;
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
  int i, nb_led, k;
	
  char tab[22+18];
  Init_SPI();

  /* Add your application code here
     */
	LED_Initialize();

  

	for(i=0;i<4;i++){
		tab[i] = 0;
	}
	
	tab[4]=0xff;
	tab[5]=0x00;
	tab[6]=0x00;
	tab[7]=0xff;
	
	for (i=8;i<12;i++){
		tab[i] = 0xff;	
	}	
		
//	// 4 LED bleues
//		for (nb_led = 0; nb_led <4;nb_led++){
//			tab[4+nb_led*4]=0xff;
//			tab[5+nb_led*4]=0xff;
//			tab[6+nb_led*4]=0x00;
//			tab[7+nb_led*4]=0x00;
//			}

//	// 4 LED rouges
//		for (nb_led = 0; nb_led <4;nb_led++){
//			tab[20+nb_led*4]=0xff;
//			tab[21+nb_led*4]=0x00;
//			tab[22+nb_led*4]=0x00;
//			tab[23+nb_led*4]=0x00;
//			}
//	
//		for (i=36;i<40;i++){
//		tab[i] = 0xff;
		
  
  while(1){
	
		Driver_SPI1.Send(tab,12);
		for (k=0; k<100000;k++)
	  {
	  }
  }
}
