#include "main.h"
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "Driver_I2C.h"                 // ::CMSIS Driver:I2C
#include "Driver_CAN.h"                 // ARM::CMSIS Driver:CAN:Custom


#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif
#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#endif

/*******CODE*I2C*PING********
*Afin de trouver le code pour l'intégration
*Suivre les commentaires CODE*I2C*PING
*******CODE*I2C*PING********/

/*******CODE*I2C*PING********/
extern ARM_DRIVER_I2C Driver_I2C1;

// Adresse esclaves sur 8 bits à décaler
//Pour l'avant
#define SLAVE_I2C_ADDR_AVM 0xEA	
#define SLAVE_I2C_ADDR_AVD 0xE0
#define SLAVE_I2C_ADDR_AVG 0xE2
//Pour l'arrière
#define SLAVE_I2C_ADDR_ARPARKING 0xE4	
#define SLAVE_I2C_ADDR_ARD 0xE0
#define SLAVE_I2C_ADDR_ARG 0xE2

//Commandes et registre I2C
#define COMMAND_REG 0x00
#define LOWBYTE_ADDR 0x03
#define HIGHBYTE_ADDR 0x02
#define COMMAND_GET_DATA_CM 0x51

//Driver Can
extern   ARM_DRIVER_CAN  Driver_CAN2;

//Prototype des fonctions d'appels sur liaison I2C (PING)
void triggerMeasure(unsigned char composant);
char readLowByte(unsigned char composant);
char readHighByte(unsigned char composant);
void Init_I2C(void);
void PING_AV(void const* argument);
void PING_AR (void const* argument);
void CANthreadT (void const* argument);
void Init_Can(void);


typedef struct{
	const	unsigned char addr;
	unsigned short dist;
} capt_i2c;

//Pour l'avant
osThreadId ID_tache_PING_AV;
osThreadDef(PING_AV, osPriorityNormal, 1, 1);
//Pour l'arrière
osThreadId ID_tache_PING_AR;
osThreadDef(PING_AR, osPriorityNormal, 1, 1);
//Pour l'envoi avec le bus can
osThreadId  ID_CANthreadT;
osThreadDef(CANthreadT, osPriorityNormal, 1, 1);

capt_i2c captAv_i2c[3] = {
		{SLAVE_I2C_ADDR_AVD,0},
		{SLAVE_I2C_ADDR_AVM,0},
		{SLAVE_I2C_ADDR_AVG,0},
	};

/*******CODE*I2C*PING********/



// Id du bus can des us
#define ID_Capteur_US			0x008


#ifdef RTE_CMSIS_RTOS2_RTX5
/**
  * Override default HAL_GetTick function
  */
uint32_t HAL_GetTick (void) {
  static uint32_t ticks = 0U;
         uint32_t i;

  if (osKernelGetState () == osKernelRunning) {
    return ((uint32_t)osKernelGetTickCount ());
  }

  /* If Kernel is not running wait approximately 1 ms then increment 
     and return auxiliary tick counter value */
  for (i = (SystemCoreClock >> 14U); i > 0U; i--) {
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
  }
  return ++ticks;
}

#endif

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void){
  HAL_Init();
  SystemClock_Config();
  SystemCoreClockUpdate();
  osKernelInitialize ();
	
	/*******CODE*I2C*PING********/
	//Initialisation de la lisaison I2C
	Init_I2C();
	Init_Can();
	//Pour l'avant
	ID_tache_PING_AV = osThreadCreate(osThread(PING_AV), NULL ) ;
	//Pour l'arrière
	//ID_tache_PING_AR = osThreadCreate(osThread(PING_AR), NULL ) ;
  /*******CODE*I2C*PING********/
	
	osKernelStart();
	osDelay(osWaitForever);
	return 0;
}

/*******CODE*I2C*PING********/
//Fonctions en I2C
void PING_AV (void const* argument){
	unsigned char highByte, lowByte;
	unsigned short distance;
	int i;
	
	
	while(1){
		for(i=0; i < 3; i++){
			triggerMeasure(captAv_i2c[i].addr); 
		}
		osDelay(70);
		
		for(i= 0; i < 3; i++){
			highByte = readHighByte(captAv_i2c[i].addr);
			lowByte = readLowByte(captAv_i2c[i].addr);
			distance = 0;
			distance = ((short)highByte << 8) | lowByte;
			captAv_i2c[i].dist = distance;
		}
	}
}

void PING_AR (void const* argument){
	unsigned char highByte, lowByte;
	unsigned short distance;
	int i;
	
	while(1){
		for(i=0; i < 3; i++){
			triggerMeasure(captAv_i2c[i].addr); 
		}
		osDelay(70);
		
		for(i= 0; i < 3; i++){
			highByte = readHighByte(captAv_i2c[i].addr);
			lowByte = readLowByte(captAv_i2c[i].addr);
			distance = 0;
			distance = ((short)highByte << 8) | lowByte;
			captAv_i2c[i].dist = distance;
		}
		osSignalSet(ID_CANthreadT,0x0001);
	}
}


void CANthreadT(void const *argument)
{
	ARM_CAN_MSG_INFO                tx_msg_info;
	char data_buf[8],*ptr;
	int i;
	while (1) 
		{
		/* Sommeil sur fin envoi */
		osSignalWait(0x0001, osWaitForever);
		tx_msg_info.id  = ARM_CAN_STANDARD_ID(ID_Capteur_US	);           // ID CAN exemple
		tx_msg_info.rtr = 0;// 0 = trame DATA
		for(i = 0; i < 8; i++)data_buf[i]=0;	
		data_buf[0] = (char)(captAv_i2c[0].dist|0x00);// low byte
		data_buf[1] = (char)((captAv_i2c[0].dist>>8)|0x00);//high byte	
		data_buf[2] = (char)(captAv_i2c[1].dist|0x00);// low byte;
		data_buf[3] = (char)((captAv_i2c[0].dist>>8)|0x00);//high byte	
		data_buf[4] = (char)(captAv_i2c[2].dist|0x00);// low byte;	
		data_buf[5] = (char)((captAv_i2c[0].dist>>8)|0x00);//high byte	

			
		Driver_CAN2.MessageSend(2, &tx_msg_info, data_buf, 6); // envoi

		}
}	

void Init_I2C(void){
	Driver_I2C1.Initialize(NULL);
	Driver_I2C1.PowerControl(ARM_POWER_FULL);
	Driver_I2C1.Control(	ARM_I2C_BUS_SPEED,	// 2nd argument = débit
							ARM_I2C_BUS_SPEED_STANDARD  );	// 100 kHz
	//Driver_I2C1.Control(	ARM_I2C_BUS_CLEAR,
							//0 );
}

void Init_Can(void)
{
			Driver_CAN2.Initialize(NULL,NULL);
			Driver_CAN2.PowerControl(ARM_POWER_FULL);
			Driver_CAN2.SetMode(ARM_CAN_MODE_INITIALIZATION);
			Driver_CAN2.SetBitrate( ARM_CAN_BITRATE_NOMINAL, // débit fixe
															125000, // 125 kbits/s (LS)
															ARM_CAN_BIT_PROP_SEG(5U) | // prop. seg = 5 TQ
															ARM_CAN_BIT_PHASE_SEG1(1U) | // phase seg1 = 1 TQ
															ARM_CAN_BIT_PHASE_SEG2(1U) | // phase seg2 = 1 TQ
															ARM_CAN_BIT_SJW(1U)); // Resync. Seg = 1 TQ
	
	
}

void triggerMeasure(unsigned char composant){
	unsigned char tab[2];
	tab[0] = COMMAND_REG; //Registre où écrire la commande
	tab[1] = COMMAND_GET_DATA_CM;	//Commande à écrire
	
	Driver_I2C1.MasterTransmit((composant>>1), tab, 2, false); // false = avec stop
	while (Driver_I2C1.GetStatus().busy == 1);	// attente fin transmission
}

char readLowByte(unsigned char composant){
	unsigned char lowByteValue, lowByteAdress;
	lowByteAdress = LOWBYTE_ADDR;
	
	Driver_I2C1.MasterTransmit((composant>>1), &lowByteAdress, 1, true); // true = sans stop
	while (Driver_I2C1.GetStatus().busy == 1);	// attente fin transmission

	Driver_I2C1.MasterReceive((composant>>1), &lowByteValue, 1, false);	// false = avec stop
	while (Driver_I2C1.GetStatus().busy == 1);	// attente fin transmission
	
	return lowByteValue;
}

char readHighByte(unsigned char composant){
	unsigned char lowByteValue, highByteAdress;
	highByteAdress = HIGHBYTE_ADDR;
	
	Driver_I2C1.MasterTransmit((composant>>1), &highByteAdress, 1, true);	// true = sans stop
	while (Driver_I2C1.GetStatus().busy == 1);	// attente fin transmission

	Driver_I2C1.MasterReceive((composant>>1), &lowByteValue, 1, false);	// false = avec stop
	while (Driver_I2C1.GetStatus().busy == 1);	// attente fin transmission
	
	return lowByteValue;
}
/*******CODE*I2C*PING********/



/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
