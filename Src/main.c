// SDA: PB9
// SCL: PB8
// SET: PB3
//	UP: PB4
//DOWN: PB5
// ENT: PB6
//Version 			 -> 0501202101//
//Version Format -> DDMMYYYYxx//
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c-lcd.h"
#include "stdbool.h"
/* Private define ------------------------------------------------------------*/
#define HOURS_CLOCK 21
#define MIN_CLOCK 	30
#define SEC_CLOCK 	30
/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
RTC_HandleTypeDef hrtc;
//  -----------------------  //
int8_t HOURS_ALARM = 20;
int8_t MIN_ALARM =	30;
int8_t SEC_ALARM =	45;
//  --------RTC CLOCK------  //
RTC_TimeTypeDef TempsTime = {0};
int8_t Thours;
int8_t Tmin;
int8_t Tsec;
char tempHours [10];
char tempMin   [10];
char tempSec   [10];
//  --------RTC DATE-------  //
RTC_DateTypeDef TempDateToUpdate = {0};
int8_t Tyear;
int8_t Tmonth;
int8_t Tdate;
int8_t Tweekday;
char tempYear  [10];
char tempMonth [10];
char tempDate  [10];
char tempWDay  [10];
//  -------RTC ALARM-------  //
RTC_AlarmTypeDef TempsAlarm = {0};
int8_t TAhours;
int8_t TAmin;
int8_t TAsec;
char tempAHours [10];
char tempAMin   [10];
char tempASec   [10];
//  --BOOL STATE MACHINE---  //
bool BOOLSET						= 0;
bool BOOLSET_TIME  		= 0;
bool BOOLSET_MIN  			= 0;
bool BOOLSET_SEC  	    = 0;
bool BOOLSET_DONE  		= 0;
bool BOOLSET_DELAY 		= 0;
bool BOOLDELAY_DONE		= 0;
bool BOOLSET_FEED_TIME = 0;
bool BOOLSET_FEED_MIN  = 0;
bool BOOLSET_FEED_SEC  = 0;
bool BOOLSET_FEED_DONE = 0;
bool BOOLSET_FEED_NOW  = 0;
//  --SET TIME&DATE ARREY--  //
int8_t SET_HOUR_ARR [24] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
int8_t SET_MIN_ARR  [60] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59};
int8_t SET_SEC_ARR  [60] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59};
int8_t SET_FHOUR_ARR[24] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
int8_t SET_FMIN_ARR [60] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59};
int8_t SET_FSEC_ARR [60] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59};
int8_t CHOSEN_HOUR  = 0;
int8_t CHOSEN_MIN   = 0;
int8_t CHOSEN_SEC   = 0;
int8_t CHOSEN_FEED_HOUR  = 0;
int8_t CHOSEN_FEED_MIN   = 0;
int8_t CHOSEN_FEED_SEC   = 0;
//  -----------------------  //
uint16_t USER_DELAY = 2000;
//  -----------------------  //
	
//  ------ Declaration ------//
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
char* itoa(int i, char b[]);
void delayMS(int delay);
void FeedMilka(void);
void INIT_SERVO(void);
void IDLE_STATE (void);
void SET_STATE (void);
void SET_HOUR_STATE (void);
void SET_MIN_STATE (void);
void SET_FEED_HOUR(void);
void SET_FEED_MIN(void);	
void SET_FEED_SEC(void);	
//  -----------------------  //
//  ---------Main----------  //
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();										// Reset of all peripherals, Initializes the Flash interface and the Systick
  SystemClock_Config();    		  // Configure the system clock
	MX_GPIO_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
	lcd_init();
	INIT_SERVO();
//  -----------------------  //
//  --------While Loop-----  //
  while (1)
  {
		IDLE_STATE();
  }
}
//  -----------------------  //
//  -------Sys Init--------  //
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 156;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef DateToUpdate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
    
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date 
  */
  sTime.Hours = HOURS_CLOCK;
  sTime.Minutes = MIN_CLOCK;
  sTime.Seconds = SEC_CLOCK;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  
	DateToUpdate.WeekDay = RTC_WEEKDAY_FRIDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 2;
  DateToUpdate.Year = 21;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable the Alarm A 
  */
  sAlarm.AlarmTime.Hours = HOURS_ALARM;
  sAlarm.AlarmTime.Minutes = MIN_ALARM;
  sAlarm.AlarmTime.Seconds = SEC_ALARM;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB3 PB4 PB5 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}





char* itoa(int i, char b[])
{
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}


void FeedMilka(void)
{
/* in case that we use PA0 as a PWM Output
	TIM2->CCER		=1;										// Channel 1 enable
	HAL_Delay(USER_DELAY);
	TIM2->CCER		=0;										// disable
*/
/* in case that we use PA0 as a digital Output*/
	GPIOA->BSRR = GPIO_BSRR_BS0;									// enable
	HAL_Delay(USER_DELAY);
	GPIOA->BSRR = GPIO_BSRR_BR0;									// disable
}

void INIT_SERVO(void)
{
	//PA0 - Digital output
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;		// PA Enable
	GPIOA->CRL    = GPIO_CRL_MODE0;				// PA0: Output, max 50MHz
	
	/*
	in case that we use PA0 as a PWM Output
	
	GPIOA->CRL   |= GPIO_CRL_CNF0_1;			// PA0: AF, Output Push pull
	//timer
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;		// TIM2 Enable
	TIM2->PSC		  = 1;
	TIM2->ARR		  = 18000-1;						  // F = 50Hz
	TIM2->CNT		  = 0;										// counter = 0
	TIM2->CCMR1		= 0x0060;								// PWM Mode 1
	TIM2->CCER		= 0;										// Channel 1 enable
	TIM2->CCR1 		= 1150;									// pulse width = arr/ccr1 = 10%      
	TIM2->CR1			= TIM_CR1_CEN;					// Counter enable
	*/
}
//  -----------------------  //
//  ---- STATE MACHINE ----  //
void SET_FEED_SEC (void)
{
	BOOLSET_DELAY=0;
	int ii=0;
	lcd_clear();
	while(!(GPIOB->IDR & GPIO_IDR_IDR6))  // enter not pressed
	{
		if(ii>-1 && ii<60){
			if(GPIOB->IDR & GPIO_IDR_IDR4)      //	UP: PB4
			ii++;
			else if(GPIOB->IDR & GPIO_IDR_IDR5)			 //	DOWN: PB5
			ii--;
		}
		else if(ii==60)
			ii=0;
		else if(ii==-1)
			ii=59;

		int temp;
		char tempc [10];
		temp = SET_FSEC_ARR[ii];
		itoa(temp, tempc);

		lcd_send_cmd(0x80|0x40);
		lcd_send_string("CHOOSE FEED SECOND:");
		if(temp<10){
			lcd_send_cmd(0x80|0x1D);
			lcd_send_string("0");
			lcd_send_cmd(0x80|0x1E);
			lcd_send_string(tempc);
		}
		else{
			lcd_send_cmd(0x80|0x1D);
			lcd_send_string(tempc);
		}
		lcd_send_cmd(0x80|0x54);
		lcd_send_string("USE UP/DOWN BOTTUNS");
		
		if(GPIOB->IDR & GPIO_IDR_IDR6){		 			 //	ENT: PB6
			BOOLSET_FEED_DONE=1;
		}
		if(BOOLSET_FEED_DONE==1){		 	 								//	ENT: PB6
			lcd_clear();
			CHOSEN_FEED_SEC = SET_FSEC_ARR[ii];;
			BOOLSET=0;
			BOOLSET_FEED_TIME =0;
			BOOLSET_FEED_MIN  =0;
			BOOLSET_FEED_SEC  =0;
			BOOLSET_FEED_DONE =0;
			HOURS_ALARM = CHOSEN_FEED_HOUR;
			MIN_ALARM =	CHOSEN_FEED_MIN;
			SEC_ALARM =	CHOSEN_FEED_SEC;	
			TempsAlarm.AlarmTime.Hours = CHOSEN_FEED_HOUR;
			TempsAlarm.AlarmTime.Minutes = CHOSEN_FEED_MIN;
			TempsAlarm.AlarmTime.Seconds = CHOSEN_FEED_SEC;
			if (HAL_RTC_SetAlarm_IT(&hrtc, &TempsAlarm, RTC_FORMAT_BIN) != HAL_OK)
			{
				Error_Handler();
			}
			HAL_Delay(100);
		}
	}
}
void SET_FEED_MIN (void)
{
	BOOLSET_DELAY=0;
	int  j=0;
	lcd_clear();
	while(!(GPIOB->IDR & GPIO_IDR_IDR6))  // enter not pressed
	{
		if(j>-1 && j<60){
			if(GPIOB->IDR & GPIO_IDR_IDR4)      //	UP: PB4
			j++;
			else if(GPIOB->IDR & GPIO_IDR_IDR5)			 //	DOWN: PB5
			j--;
		}
		else if(j==60)
			j=0;
		else if(j==-1)
			j=59;

		int temp;
		char tempc [10];
		temp = SET_FMIN_ARR[j];
		itoa(temp, tempc);

		lcd_send_cmd(0x80|0x40);
		lcd_send_string("CHOOSE FEED MINUTE:");
		
		if(temp<10){
			lcd_send_cmd(0x80|0x1D);
			lcd_send_string("0");
			lcd_send_cmd(0x80|0x1E);
			lcd_send_string(tempc);
		}
		else{
			lcd_send_cmd(0x80|0x1D);
			lcd_send_string(tempc);
		}
		
		lcd_send_cmd(0x80|0x54);
		lcd_send_string("USE UP/DOWN BOTTUNS");
		
		if(GPIOB->IDR & GPIO_IDR_IDR6){		 			 //	ENT: PB6
			BOOLSET_FEED_SEC=1;
			BOOLSET_DELAY=0;
		}
		if(BOOLSET_FEED_SEC==1){		 	 								//	ENT: PB6
			CHOSEN_FEED_MIN = SET_FMIN_ARR[j];
			HAL_Delay(100);
			SET_FEED_SEC();
		}
	}
}

void SET_FEED_HOUR(void)
{
	BOOLSET_DELAY=0;
	int  i=0;
	lcd_clear();	
	while(!(GPIOB->IDR & GPIO_IDR_IDR6))          // enter not pressed
	{
		if(i>-1 && i<24){
			if(GPIOB->IDR & GPIO_IDR_IDR4)    	   	  //	UP: PB4
			i++;
			else if(GPIOB->IDR & GPIO_IDR_IDR5)			  //	DOWN: PB5
			i--;
		}
		else if(i==24)
			i=0;
		else if(i==-1)
			i=23;
		
		int temp;
		char tempc [10];
		temp = SET_FHOUR_ARR[i];
		itoa(temp, tempc);

		lcd_send_cmd(0x80|0x41);
		lcd_send_string("CHOOSE FEED HOUR:");
		
		if(temp<10){
			lcd_send_cmd(0x80|0x1D);
			lcd_send_string("0");
			lcd_send_cmd(0x80|0x1E);
			lcd_send_string(tempc);
		}
		else{
			lcd_send_cmd(0x80|0x1D);
			lcd_send_string(tempc);
		}
		
		lcd_send_cmd(0x80|0x54);
		lcd_send_string("USE UP/DOWN BOTTUNS");

		if(GPIOB->IDR & GPIO_IDR_IDR6){		 			 //	ENT: PB6
			BOOLSET_FEED_MIN=1;
			BOOLSET_DELAY=0;
		}
		if(BOOLSET_FEED_MIN==1){		 	 								
			CHOSEN_FEED_HOUR = SET_FHOUR_ARR[i];
			HAL_Delay(100);
			SET_FEED_MIN();
		}
	}
}

void SET_DELAY_STATE(void)
{
	char tempc [10];
	//float temp=0;
	lcd_clear();
	while(!(GPIOB->IDR & GPIO_IDR_IDR6))  			// enter not pressed
	{
	//	temp = USER_DELAY/1000);
		itoa(USER_DELAY, tempc);
		lcd_send_cmd(0x80|0x44);
		lcd_send_string("CHOOSE DELAY:");
		

		if(USER_DELAY<10){
			lcd_send_cmd(0x80|0x1B);
			lcd_send_string(tempc);
			lcd_send_cmd(0x80|0x1D);
			lcd_send_string("ms");
		}
		else if(USER_DELAY==10){
			lcd_send_cmd(0x80|0x1B);
			lcd_send_string(tempc);
			lcd_send_cmd(0x80|0x1E);
			lcd_send_string("ms");
			lcd_clear();
		}
		else if(USER_DELAY<100 && USER_DELAY>10){
			lcd_send_cmd(0x80|0x1B);
			lcd_send_string(tempc);
			lcd_send_cmd(0x80|0x1E);
			lcd_send_string("ms");
		}
		else if(USER_DELAY==100){
			lcd_send_cmd(0x80|0x1B);
			lcd_send_string(tempc);
			lcd_send_cmd(0x80|0x1F);
			lcd_send_string("ms");
			lcd_clear();
		}
		else if(USER_DELAY>100 && USER_DELAY<1000){
			lcd_send_cmd(0x80|0x1B);
			lcd_send_string(tempc);
			lcd_send_cmd(0x80|0x1F);
			lcd_send_string("ms");
		}
		else if(USER_DELAY==1000){
			lcd_send_cmd(0x80|0x1B);
			lcd_send_string(tempc);
			lcd_send_cmd(0x80|0x20);
			lcd_send_string("ms");
			lcd_clear();
		}
		else if(USER_DELAY>1000&& USER_DELAY<10000){
			lcd_send_cmd(0x80|0x1B);
			lcd_send_string(tempc);
			lcd_send_cmd(0x80|0x20);
			lcd_send_string("ms");
		}
		else if(USER_DELAY==10000){
			lcd_send_cmd(0x80|0x1B);
			lcd_send_string(tempc);
			lcd_send_cmd(0x80|0x21);
			lcd_send_string("ms");
			lcd_clear();
		}
		else if(USER_DELAY>10000){
			lcd_send_cmd(0x80|0x1B);
			lcd_send_string(tempc);
			lcd_send_cmd(0x80|0x21);
			lcd_send_string("ms");
		}

		
		lcd_send_cmd(0x80|0x54);
		lcd_send_string("USE UP/DOWN BOTTUNS");
	
		if(GPIOB->IDR & GPIO_IDR_IDR4)    		 	   //	UP: PB4
		USER_DELAY+=10;
		else if(GPIOB->IDR & GPIO_IDR_IDR5)		  	 //	DOWN: PB5
		USER_DELAY-=10;
		
		if(GPIOB->IDR & GPIO_IDR_IDR6){		 		  	 //	ENT: PB6
			BOOLDELAY_DONE=1;
		}
		if(BOOLDELAY_DONE==1){
			lcd_clear();
			BOOLSET=0;
			BOOLDELAY_DONE=0;
			BOOLSET_DELAY=0;
		}
	}
}


void SET_SEC_STATE (void)
{
	BOOLSET_DELAY=0;
	int ii=0;
	lcd_clear();
	while(!(GPIOB->IDR & GPIO_IDR_IDR6)) 				 // enter not pressed
	{
		if(ii>-1 && ii<60){
			if(GPIOB->IDR & GPIO_IDR_IDR4)   			   //	UP: PB4
			ii++;
			else if(GPIOB->IDR & GPIO_IDR_IDR5)			 //	DOWN: PB5
			ii--;
		}
		else if(ii==60)
			ii=0;
		else if(ii==-1)
			ii=59;

		int temp;
		char tempc [10];
		temp = SET_SEC_ARR[ii];
		itoa(temp, tempc);

		lcd_send_cmd(0x80|0x43);
		lcd_send_string("CHOOSE SECOND:");
	
		if(temp<10){
			lcd_send_cmd(0x80|0x1D);
			lcd_send_string("0");
			lcd_send_cmd(0x80|0x1E);
			lcd_send_string(tempc);
		}
		else{
			lcd_send_cmd(0x80|0x1D);
			lcd_send_string(tempc);
		}
		
		lcd_send_cmd(0x80|0x54);
		lcd_send_string("USE UP/DOWN BOTTUNS");
		
		if(GPIOB->IDR & GPIO_IDR_IDR6){		 			 //	ENT: PB6
			BOOLSET_DONE=1;
		}
		if(BOOLSET_DONE==1){		 	 								//	ENT: PB6
			lcd_clear();
			CHOSEN_SEC = SET_SEC_ARR[ii];
			BOOLSET=0;
			BOOLSET_TIME =0;
			BOOLSET_MIN  =0;
			BOOLSET_SEC  =0;
			BOOLSET_DONE =0;
					
			TempsTime.Hours = CHOSEN_HOUR;
			TempsTime.Minutes = CHOSEN_MIN;
			TempsTime.Seconds = CHOSEN_SEC;

			if (HAL_RTC_SetTime(&hrtc, &TempsTime, RTC_FORMAT_BIN) != HAL_OK)
			{
				Error_Handler();
			}		
			HAL_Delay(100);
		}
	}
}



void SET_MIN_STATE (void)
{
	BOOLSET_DELAY=0;
	int  j=0;
	lcd_clear();
	while(!(GPIOB->IDR & GPIO_IDR_IDR6))  // enter not pressed
	{
		if(j>-1 && j<60){
			if(GPIOB->IDR & GPIO_IDR_IDR4)      //	UP: PB4
			j++;
			else if(GPIOB->IDR & GPIO_IDR_IDR5)			 //	DOWN: PB5
			j--;
		}
		else if(j==60)
			j=0;
		else if(j==-1)
			j=59;

		int temp;
		char tempc [10];
		temp = SET_MIN_ARR[j];
		itoa(temp, tempc);

		lcd_send_cmd(0x80|0x43);
		lcd_send_string("CHOOSE MINUTE:");	
		
		if(temp<10){
			lcd_send_cmd(0x80|0x1D);
			lcd_send_string("0");
			lcd_send_cmd(0x80|0x1E);
			lcd_send_string(tempc);
		}
		else{
			lcd_send_cmd(0x80|0x1D);
			lcd_send_string(tempc);
		}
		
		lcd_send_cmd(0x80|0x54);
		lcd_send_string("USE UP/DOWN BOTTUNS");
		
		if(GPIOB->IDR & GPIO_IDR_IDR6){		 			 //	ENT: PB6
			BOOLSET_SEC=1;
		}
		if(BOOLSET_SEC==1){		 	 								//	ENT: PB6
			CHOSEN_MIN = SET_MIN_ARR[j];
			HAL_Delay(100);
			SET_SEC_STATE();
		}
	}
}

void SET_HOUR_STATE (void)
{
	BOOLSET_DELAY=0;
	int  i=0;
	lcd_clear();	
	while(!(GPIOB->IDR & GPIO_IDR_IDR6))          // enter not pressed
	{
		if(i>-1 && i<24){
			if(GPIOB->IDR & GPIO_IDR_IDR4)    	   	  //	UP: PB4
			i++;
			else if(GPIOB->IDR & GPIO_IDR_IDR5)			  //	DOWN: PB5
			i--;
		}
		else if(i==24)
			i=0;
		else if(i==-1)
			i=23;
		
		int temp;
		char tempc [10];
		temp = SET_HOUR_ARR[i];
		itoa(temp, tempc);
	
		lcd_send_cmd(0x80|0x44);
		lcd_send_string("CHOOSE HOUR:");
		
		if(temp<10){
			lcd_send_cmd(0x80|0x1D);
			lcd_send_string("0");
			lcd_send_cmd(0x80|0x1E);
			lcd_send_string(tempc);
		}
		else{
			lcd_send_cmd(0x80|0x1D);
			lcd_send_string(tempc);
		}
		
		lcd_send_cmd(0x80|0x54);
		lcd_send_string("USE UP/DOWN BOTTUNS");

		if(GPIOB->IDR & GPIO_IDR_IDR6){		 			 //	ENT: PB6
			BOOLSET_MIN=1;
		}
		if(BOOLSET_MIN==1){		 	 								
			CHOSEN_HOUR = SET_HOUR_ARR[i];
			HAL_Delay(100);
			SET_MIN_STATE();
		}
	}
}
void SET_STATE (void)
{
	if			(GPIOB->IDR & GPIO_IDR_IDR5)					 //	DOWN: PB5
			BOOLSET_TIME 			= 1;
	else if (GPIOB->IDR & GPIO_IDR_IDR6)		 	 		 //	ENT: PB6
			BOOLSET_DELAY			= 1;
	else if(GPIOB->IDR & GPIO_IDR_IDR4)     			 //	UP: PB4
			BOOLSET_FEED_TIME = 1 ;
	
	if			(BOOLSET_TIME == 1){
		HAL_Delay(100);
		SET_HOUR_STATE();
		}
	else if (BOOLSET_DELAY == 1){
		HAL_Delay(100);
		SET_DELAY_STATE();
		}	
	else if (BOOLSET_FEED_TIME == 1){
		HAL_Delay(100);
		SET_FEED_HOUR();
	}
	else{
	Thours = TempsTime.Hours;
		itoa(Thours, tempHours);
	if (Thours<10){	
			lcd_send_cmd(0x80|0x00);
			lcd_send_string("0");
			lcd_send_cmd(0x80|0x01);
			lcd_send_string(tempHours);
		}
		else{
			lcd_send_cmd(0x80|0x00);
			lcd_send_string(tempHours);
		}
		lcd_send_cmd(0x80|0x02);
		lcd_send_string(":");
		// Minute
		// -------------- //
		Tmin = TempsTime.Minutes;
		itoa(Tmin, tempMin);
		if (Tmin<10){	
			lcd_send_cmd(0x80|0x03);
			lcd_send_string("0");
			lcd_send_cmd(0x80|0x04);
			lcd_send_string(tempMin);
		}
		else{
			lcd_send_cmd(0x80|0x03);
			lcd_send_string(tempMin);
		}
		lcd_send_cmd(0x80|0x05);
		lcd_send_string(":");
		// Second
		// -------------- //
		Tsec = TempsTime.Seconds;
		itoa(Tsec, tempSec);
		if (Tsec<10){	
			lcd_send_cmd(0x80|0x06);
			lcd_send_string("0");
			lcd_send_cmd(0x80|0x07);
			lcd_send_string(tempSec);
		}
		else{
			lcd_send_cmd(0x80|0x06);
			lcd_send_string(tempSec);
		}
		// -------------- //
		// ---- date ---- //
		// -------------- //
		Tdate = TempDateToUpdate.Date;
		itoa(Tdate, tempDate);
		if(Tdate <10){
			lcd_send_cmd(0x80|0x0C);
			lcd_send_string("0");
			lcd_send_cmd(0x80|0x0D);
			lcd_send_string(tempDate);
		}
		else{
			lcd_send_cmd(0x80|0x0C);
			lcd_send_string(tempDate);
		}
		lcd_send_cmd(0x80|0x0E);
		lcd_send_string("/");
		// month
		// -------------- //
		Tmonth = TempDateToUpdate.Month;
		itoa(Tmonth, tempMonth);
		if(Tmonth<10){
			lcd_send_cmd(0x80|0x0F);
			lcd_send_string("0");
			lcd_send_cmd(0x80|0x10);
			lcd_send_string(tempMonth);
		}
		else{
			lcd_send_cmd(0x80|0x0F);
			lcd_send_string(tempMonth);
		}
		lcd_send_cmd(0x80|0x11);
		lcd_send_string("/");
		// year
		// -------------- //
		Tyear = TempDateToUpdate.Year;
		itoa(Tyear, tempYear);
		lcd_send_cmd(0x80|0x12);
		lcd_send_string(tempYear);
		// -------------- //
	
	lcd_send_cmd(0x80|0x40);
	lcd_send_string("DOWN: SET TIME");
	lcd_send_cmd(0x80|0x14);
	lcd_send_string("UP  : SET FEED TIME");
	lcd_send_cmd(0x80|0x54);
	lcd_send_string("ENT : SET DELAY");
	}
}	

void IDLE_STATE (void)
{
		if(((HOURS_ALARM == Thours) && (MIN_ALARM == Tmin) && (SEC_ALARM == Tsec)) || (BOOLSET_FEED_NOW == 1)){
			FeedMilka();
			BOOLSET_FEED_NOW=0;
		}	
		if(GPIOB->IDR & GPIO_IDR_IDR3){   	       // SET: PB3
			BOOLSET = 1;
			BOOLSET_DELAY = 0;
			lcd_clear();
		}
		else if (GPIOB->IDR & GPIO_IDR_IDR6){		 	 //	ENT: PB6
			HAL_Delay(1400);
			if(GPIOB->IDR & GPIO_IDR_IDR6)
			BOOLSET_FEED_NOW = 1;
		}
		HAL_RTC_GetTime(&hrtc, &TempsTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &TempDateToUpdate, RTC_FORMAT_BIN);
		HAL_RTC_GetAlarm(&hrtc, &TempsAlarm,1 ,RTC_FORMAT_BIN);
		
		if(BOOLSET == 1){
			HAL_Delay(100);
			SET_STATE();
		}
		else{
		// Header
		lcd_send_cmd(0x80|0x41);
		lcd_send_string("Milka & Tinkerbell");
		lcd_send_cmd(0x80|0x1B);
		lcd_send_string("Feeder");
		// ---- Time ---- //
		// Hours
		// -------------- //
		Thours = TempsTime.Hours;
		itoa(Thours, tempHours);
	if (Thours<10){	
			lcd_send_cmd(0x80|0x00);
			lcd_send_string("0");
			lcd_send_cmd(0x80|0x01);
			lcd_send_string(tempHours);
		}
		else{
			lcd_send_cmd(0x80|0x00);
			lcd_send_string(tempHours);
		}
		lcd_send_cmd(0x80|0x02);
		lcd_send_string(":");
		// Minute
		// -------------- //
		Tmin = TempsTime.Minutes;
		itoa(Tmin, tempMin);
		if (Tmin<10){	
			lcd_send_cmd(0x80|0x03);
			lcd_send_string("0");
			lcd_send_cmd(0x80|0x04);
			lcd_send_string(tempMin);
		}
		else{
			lcd_send_cmd(0x80|0x03);
			lcd_send_string(tempMin);
		}
		lcd_send_cmd(0x80|0x05);
		lcd_send_string(":");
		// Second
		// -------------- //
		Tsec = TempsTime.Seconds;
		itoa(Tsec, tempSec);
		if (Tsec<10){	
			lcd_send_cmd(0x80|0x06);
			lcd_send_string("0");
			lcd_send_cmd(0x80|0x07);
			lcd_send_string(tempSec);
		}
		else{
			lcd_send_cmd(0x80|0x06);
			lcd_send_string(tempSec);
		}
		// -------------- //
		// ---- date ---- //
		// -------------- //
		Tdate = TempDateToUpdate.Date;
		itoa(Tdate, tempDate);
		if(Tdate <10){
			lcd_send_cmd(0x80|0x0C);
			lcd_send_string("0");
			lcd_send_cmd(0x80|0x0D);
			lcd_send_string(tempDate);
		}
		else{
			lcd_send_cmd(0x80|0x0C);
			lcd_send_string(tempDate);
		}
		lcd_send_cmd(0x80|0x0E);
		lcd_send_string("/");
		// month
		// -------------- //
		Tmonth = TempDateToUpdate.Month;
		itoa(Tmonth, tempMonth);
		if(Tmonth<10){
			lcd_send_cmd(0x80|0x0F);
			lcd_send_string("0");
			lcd_send_cmd(0x80|0x10);
			lcd_send_string(tempMonth);
		}
		else{
			lcd_send_cmd(0x80|0x0F);
			lcd_send_string(tempMonth);
		}
		lcd_send_cmd(0x80|0x11);
		lcd_send_string("/");
		// year
		// -------------- //
		Tyear = TempDateToUpdate.Year;
		itoa(Tyear, tempYear);
		lcd_send_cmd(0x80|0x12);
		lcd_send_string(tempYear);
		// -------------- //
		lcd_send_cmd(0x80|0x54);
		lcd_send_string("Next Feed:");
		// ---- alarm ---- //
		// Hours
		// -------------- //
		TAhours = TempsAlarm.AlarmTime.Hours;
		itoa(TAhours, tempAHours);
		if(TAhours<10){
			lcd_send_cmd(0x80|0x5F);
			lcd_send_string("0");
			lcd_send_cmd(0x80|0x60);
			lcd_send_string(tempAHours);
		}
		else{
			lcd_send_cmd(0x80|0x5F);
			lcd_send_string(tempAHours);
		}
		lcd_send_cmd(0x80|0x61);
		lcd_send_string(":");
		// Minute
		// -------------- //
		TAmin = TempsAlarm.AlarmTime.Minutes;
		itoa(TAmin, tempAMin);
		if(TAmin<10){
			lcd_send_cmd(0x80|0x62);
			lcd_send_string("0");
			lcd_send_cmd(0x80|0x63);
			lcd_send_string(tempAMin);
		}
		else{
			lcd_send_cmd(0x80|0x62);
			lcd_send_string(tempAMin);
		}
		lcd_send_cmd(0x80|0x64);
		lcd_send_string(":");
		// Second
		// -------------- //
		TAsec = TempsAlarm.AlarmTime.Seconds;
		itoa(TAsec, tempASec);
		if(TAsec<10){
			lcd_send_cmd(0x80|0x65);
			lcd_send_string("0");
			lcd_send_cmd(0x80|0x66);
			lcd_send_string(tempASec);
		}
		else{
			lcd_send_cmd(0x80|0x65);
			lcd_send_string(tempASec);
		}	
		// -------------- //
		//	
	}
}
//  -----------------------  //
//  ----------EOF----------  //