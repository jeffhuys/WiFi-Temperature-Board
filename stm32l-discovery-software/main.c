#include <stddef.h>
#include "stm32l1xx.h"

#ifdef USE_STM32L152_EVAL
 #include "stm32l152_eval.h"
 #include "stm32l152_eval_lcd.h"
#elif defined USE_STM32L_DISCOVERY
 #include "discover_board.h"
 #include "stm32l_discovery_lcd.h"
#endif


/* Private typedef */
/* Private define  */
#ifdef USE_STM32L152_EVAL
  #define MESSAGE1   "    STM32L ULP MD   "
  #define MESSAGE2   " Device running on  "
  #define MESSAGE3   "   STM32L152-EVAL   "
  #define MESSAGE4   "                    "
#endif
  #define MESSAGE5   " program built with "
  #define MESSAGE6   " Atollic TrueSTUDIO "


/* Private macro */
/* Private variables */
 USART_InitTypeDef USART_InitStructure;

/* Private function prototypes */
/* Private functions */
#ifdef USE_STM32L_DISCOVERY
 void Init_GPIOs (void);
 void RCC_Configuration(void);
 void Delay(uint32_t time);
 float DecodeTemp(int arr[]);

 static volatile uint32_t TimingDelay;
 RCC_ClocksTypeDef RCC_Clocks;

 void Config_Systick()
 {
   RCC_GetClocksFreq(&RCC_Clocks);
   SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
 }
#endif

 // reverses a string 'str' of length 'len'
 void reverse(char *str, int len)
 {
     int i=0, j=len-1, temp;
     while (i<j)
     {
         temp = str[i];
         str[i] = str[j];
         str[j] = temp;
         i++; j--;
     }
 }

  // Converts a given integer x to string str[].  d is the number
  // of digits required in output. If d is more than the number
  // of digits in x, then 0s are added at the beginning.
 int intToStr(int x, char str[], int d)
 {
     int i = 0;
     while (x)
     {
         str[i++] = (x%10) + '0';
         x = x/10;
     }

     // If number of digits required is more, then
     // add 0s at the beginning
     while (i < d)
         str[i++] = '0';

     reverse(str, i);
     str[i] = '\0';
     return i;
 }

 // Converts a floating point number to string.
 void ftoa(float n, char *res, int afterpoint)
 {
     // Extract integer part
     int ipart = (int)n;

     // Extract floating part
     float fpart = n - (float)ipart;

     // convert integer part to string
     int i = intToStr(ipart, res, 0);

     // check for display option after point
     if (afterpoint != 0)
     {
         res[i] = '.';  // add dot

         // Get the value of fraction part upto given no.
         // of points after dot. The third parameter is needed
         // to handle cases like 233.007
         fpart = fpart * pow(10, afterpoint);

         intToStr((int)fpart, res + i + 1, afterpoint);
     }
 }

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{
  int i = 0;

  /**
  *  IMPORTANT NOTE!
  *  See the <system_*.c> file and how/if the SystemInit() function updates
  *  SCB->VTOR register. Sometimes the symbol VECT_TAB_SRAM needs to be defined
  *  when building the project if code has been located to RAM and interrupts
  *  are used. Otherwise the interrupt table located in flash will be used.
  *  E.g.  SCB->VTOR = 0x20000000;
  */

  /**
  *  At this stage the microcontroller clock setting is already configured,
  *  this is done through SystemInit() function which is called from startup
  *  file (startup_stm32l1xx_md.s) before to branch to application main.
  *  To reconfigure the default setting of SystemInit() function, refer to
  *  system_stm32l1xx.c file
  */

  /* TODO - Add your application code here */


  RCC_Configuration();
  /* Init I/O ports */
  Init_GPIOs ();

  /* Init Systick */
  Config_Systick();

  /* Initializes the LCD glass */
  LCD_GLASS_Init();
  LCD_GLASS_DisplayString((uint8_t*)"STM32L-DISCOVERY");
  //LCD_GLASS_ScrollSentence((uint8_t*)"  ** Atollic TrueSTUDIO **  ",1,200);

  /* Switch on the leds at start */
  GPIO_HIGH(LD_PORT,LD_GREEN);
  GPIO_HIGH(LD_PORT,LD_BLUE);


  int STAGE = 0;
  int DECODING = 0;
  int DECODED_ARR[8] = { 0 };
  int PREV_GPIO = 0x0;
  int delayTime = 100;
  float temperatureToDisplay = -1.0;

  /* Infinite loop */
  while (1)
  {
	i++;
#ifdef USE_STM32L152_EVAL
	STM_EVAL_LEDToggle(LED1);
#elif defined USE_STM32L_DISCOVERY
    /* Check if the User Button is pressed */
    //if ((GPIOA->IDR & USER_GPIO_PIN) != 0x0)



//
//	switch(STAGE) {
//		case 0:
//			LCD_GLASS_DisplayString((uint8_t*) "STAGE0 ");
//			if ((GPIOB->IDR & GPIO_Pin_2) != 0x0) {
//				STAGE = 1;
//			}
//			break;
//		case 1:
//			LCD_GLASS_DisplayString((uint8_t*) "STAGE1 ");
//			break;
//
//	}

//	LCD_GLASS_DisplayString((uint8_t*)"WAIT   ");

	if ((GPIOB->IDR & GPIO_Pin_2) != 0x0) {
		// Start decoding
		LCD_GLASS_DisplayString((uint8_t*)"DECODE ");

		int bit = 0;
		while(bit < 8) {
			// wait x ms
			Delay(delayTime);
//			if(toggleMe == 0) {
//				LCD_GLASS_DisplayString((uint8_t*)"-      ");
//				toggleMe = 1;
//			} else {
//				LCD_GLASS_DisplayString((uint8_t*)"|       ");
//				toggleMe = 0;
//			}
			DECODED_ARR[bit] = (GPIOB->IDR & GPIO_Pin_2);
			bit++;
		}

		temperatureToDisplay = DecodeTemp(DECODED_ARR);

		char res[7];
		ftoa(temperatureToDisplay, res, 1);

		LCD_GLASS_DisplayString((uint8_t*)"       ");
		LCD_GLASS_DisplayString((uint8_t*) res);
		//LCD_GLASS_DisplayString((uint8_t) DecodeTemp(DECODED_ARR));

	}


//	if ((GPIOB->IDR & GPIO_Pin_2) != 0x0) {
//      LCD_GLASS_DisplayString((uint8_t*)"HIGH   ");
//      /* Toggle blue LED */
//      GPIO_TOGGLE(LD_PORT,LD_BLUE);
//    } else {
//      LCD_GLASS_DisplayString((uint8_t*)"LOW    ");
//      /* Toggle green LED */
//      GPIO_TOGGLE(LD_PORT,LD_GREEN);
//    }
#endif
  }
  return 0;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
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

/*
 * Minimal __assert_func used by the assert() macro
 * */
void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
  while(1)
  {}
}

/*
 * Minimal __assert() uses __assert__func()
 * */
void __assert(const char *file, int line, const char *failedexpr)
{
   __assert_func (file, line, NULL, failedexpr);
}

#ifdef USE_SEE
#ifndef USE_DEFAULT_TIMEOUT_CALLBACK
/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval sEE_FAIL.
  */
uint32_t sEE_TIMEOUT_UserCallback(void)
{
  /* Return with error code */
  return sEE_FAIL;
}
#endif
#endif /* USE_SEE */


#ifdef USE_STM32L_DISCOVERY
/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
  /* Enable the GPIOs Clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC| RCC_AHBPeriph_GPIOD| RCC_AHBPeriph_GPIOE| RCC_AHBPeriph_GPIOH, ENABLE);

  /* Enable comparator clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_COMP | RCC_APB1Periph_LCD | RCC_APB1Periph_PWR,ENABLE);

  /* Enable SYSCFG */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG , ENABLE);

  /* Allow access to the RTC */
  PWR_RTCAccessCmd(ENABLE);

  /* Reset Backup Domain */
  RCC_RTCResetCmd(ENABLE);
  RCC_RTCResetCmd(DISABLE);

  /*!< LSE Enable */
  RCC_LSEConfig(RCC_LSE_ON);

  /*!< Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

  /*!< LCD Clock Source Selection */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
}

/**
  * @brief  To initialize the I/O ports
  * @caller main
  * @param None
  * @retval None
  */
void  Init_GPIOs (void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure User Button pin as input */
  GPIO_InitStructure.GPIO_Pin = USER_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(BUTTON_GPIO_PORT, &GPIO_InitStructure);


  /*Configure a port */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//  GPIO_Init(LD_PORT, &GPIO_InitStructure);


/* Configure the GPIO_LED pins  LD3 & LD4*/
  GPIO_InitStructure.GPIO_Pin = LD_GREEN|LD_BLUE;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(LD_PORT, &GPIO_InitStructure);
  GPIO_LOW(LD_PORT,LD_GREEN);
  GPIO_LOW(LD_PORT,LD_BLUE);


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(LD_PORT, &GPIO_InitStructure);

/* Configure Output for LCD */
/* Port A */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_8 | GPIO_Pin_9 |GPIO_Pin_10 |GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init( GPIOA, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource15,GPIO_AF_LCD) ;

/* Configure Output for LCD */
/* Port B */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9 \
                                 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init( GPIOB, &GPIO_InitStructure);

#ifdef USE_ALL_LCD_SEGMENTS
/**
 * Note!
 * PB3 is connected to C, M, COLON, and DP segments for the second digit on the LCD
 * PB3 is also the SWO pin used for the Serial Wire Viewer (SWV)
 * If PB3 is used by LCD then SWV will not work for the STM32L_DISCOVERY board
 **/
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource3,GPIO_AF_LCD) ;
#endif
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource4,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource12,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource13,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource14,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource15,GPIO_AF_LCD) ;

/* Configure Output for LCD */
/* Port C*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 \
                                 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |GPIO_Pin_11 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init( GPIOC, &GPIO_InitStructure);


  GPIO_PinAFConfig(GPIOC, GPIO_PinSource0,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource1,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource2,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource3,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource8,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10,GPIO_AF_LCD) ;
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11,GPIO_AF_LCD) ;
}


// TODO: Add support for -64
float DecodeTemp(int arr[]) {
	float finalTemp = 0;
	int x = 0;
	for(x = 0; x < 7; x++) {
		if(arr[x] != 0) {
			finalTemp += pow(2, x-1);
		}
	}

	return finalTemp;
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 10 ms.
  * @retval None
  */
void Delay(uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);

}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{

  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}

#endif
