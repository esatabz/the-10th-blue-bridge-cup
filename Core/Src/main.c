#include "main.h"
#include "adc.h"
#include "tim.h"
#include "gpio.h"
#include "lcd.h"
#include<stdio.h>
#include<string.h>

/*-----led-----*/
uint8_t led_buff[9] = {0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
uint8_t led_choose;
uint8_t led1 = 0x01;
uint8_t led2 = 0x02;
uint8_t ledrun = 0x00;
uint8_t j = 1;
uint8_t k = 2;


/*-----lcd-----*/
uint8_t Lcdstring[21];
float max_volt = 2.40;
float min_volt = 1.20;
uint8_t i = 1;

/*-----error-----*/
uint8_t error = 0;

/*-----setting-----*/
uint8_t set = 0;

/*-----Adc-----*/
float Adc = 0;


/*-------------------*/
void LED(uint8_t led);
void KEY_Scan(void);
void LCD_Proc(void);
void ADC_Proc(void);
void Error(void);
void LED_Proc(void);


void SystemClock_Config(void);


int main(void)
{
	
  HAL_Init();
	LCD_Init();
  SystemClock_Config();


  MX_GPIO_Init();
  MX_ADC2_Init();
  MX_TIM6_Init();
	LCD_Clear(Black);
	LCD_SetTextColor(White);
	LCD_SetBackColor(Black);
	LED(0x00);

  while (1)
  {
		Error();
		KEY_Scan();
		LCD_Proc();

  }
  /* USER CODE END 3 */
}




void LED(uint8_t led)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(GPIOC, led<<8,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
}	


void KEY_Scan(void)
{
	if((HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)) == GPIO_PIN_RESET)
	{
		HAL_Delay(30);
		if((HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)) == GPIO_PIN_RESET)
		{
			if(error == 0)
			{
				LCD_Clear(Black);
				set =! set;
				error = 0;
			}
		}
		while(!(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)));
	}
	else if((HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)) == GPIO_PIN_RESET)
	{
		HAL_Delay(30);
		if((HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)) == GPIO_PIN_RESET)
		{
			if(set == 1)
			{
				i++;
				if(i == 5)
				{
					i = 1;
				}
			}
		}
		while(!(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)));
	}
	else if((HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)) == GPIO_PIN_RESET)
	{
		HAL_Delay(30);
		if((HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)) == GPIO_PIN_RESET)
		{
			if(set == 1)
			{
				if(i == 1)
				{
					max_volt = max_volt + 0.30;
					if(max_volt > 3.30)
					{
						max_volt = 3.30;
					}
			  }
			  else if(i == 2)
				{
					min_volt = min_volt + 0.30;
					if(min_volt > 3.30)
					{
						min_volt = 3.30;
					}
			  }
				else if(i == 3)
				{
					j += 1;
					if(j > 8)
					{
						j = 8;
					}
				}
				else if(i == 4)
				{
					k += 1;
					if(k > 8)
					{
						k = 8;
					}
				}
	  	}
		  while(!(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)));
	  } 
  }
	else if((HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)) == GPIO_PIN_RESET)
	{
		HAL_Delay(30);
		if((HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)) == GPIO_PIN_RESET)
		{
			if(set == 1)
			{
				if(i == 1)
				{
					max_volt = max_volt - 0.30;
					if(max_volt < 0.00)
					{
						max_volt = 0.00;
					}
			  }
			  else if(i == 2)
				{
					min_volt = min_volt - 0.30;
					if(min_volt < 0.00)
					{
						min_volt = 0.00;
					}
			  }
				else if(i == 3)
				{
					j -= 1;
					if(j < 1)
					{
						j = 1;
					}
				}
				else if(i == 4)
				{
					k -= 1;
					if(k < 1)
					{
						k = 1;
					}
				}
	  	}
		  while(!(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)));
	  } 
  }
}


void LCD_Proc(void)
{
	ADC_Proc();
	if(set == 0)
	{
		LCD_DisplayStringLine(Line1, (unsigned char *)"        Main        ");
		sprintf((char *)Lcdstring,"   Volt: %1.2f V       ",Adc);
		LCD_DisplayStringLine(Line3,Lcdstring);
		if(Adc < min_volt)
		{
			led_choose = 1;
			LCD_DisplayStringLine(Line5, (unsigned char *)"  Status: Lower      ");
			LED_Proc();
		}
		else if(Adc > max_volt)
		{
			led_choose = 2;
			LCD_DisplayStringLine(Line5, (unsigned char *)"  Status: Upper      ");
			LED_Proc();
		}
		else
		{
			led_choose = 3;
			LCD_DisplayStringLine(Line5, (unsigned char *)"  Status: Normal     ");
			LED_Proc();
		}
	}
	else
	{
		LCD_DisplayStringLine(Line1, (unsigned char *)"      Setting        ");
		LCD_ClearLine(Line5);
		if(i == 1)
		{
			LCD_SetBackColor(White);
			LCD_SetTextColor(Blue);
		}
		sprintf((char *)Lcdstring, "  Max Volt: %1.1f V    ",max_volt);
		LCD_DisplayStringLine(Line3, Lcdstring);
		LCD_SetBackColor(Black);
		LCD_SetTextColor(White);
		
		if(i == 2)
		{
			LCD_SetBackColor(White);
			LCD_SetTextColor(Blue);
		}
		sprintf((char *)Lcdstring, "  Min Volt: %1.1f V    ",min_volt);
		LCD_DisplayStringLine(Line4, Lcdstring);
		LCD_SetBackColor(Black);
		LCD_SetTextColor(White);
		
		if(i == 3)
		{
			LCD_SetBackColor(White);
			LCD_SetTextColor(Blue);
		}
		sprintf((char *)Lcdstring, "  Upper: LD%d     ",j);
		LCD_DisplayStringLine(Line6, Lcdstring);
		LCD_SetBackColor(Black);
		LCD_SetTextColor(White);
		
		if(i == 4)
		{
			LCD_SetBackColor(White);
			LCD_SetTextColor(Blue);
		}
		sprintf((char *)Lcdstring, "  Lower: LD%d        ",k);
		LCD_DisplayStringLine(Line7, Lcdstring);
		LCD_SetBackColor(Black);
		LCD_SetTextColor(White);
	}
}



void ADC_Proc(void)
{
	HAL_ADC_Start(&hadc2);
	Adc = ((float)(HAL_ADC_GetValue(&hadc2))/4096*3.3f);
}


void Error(void)
{
	if(set == 1)
	{
		if(((max_volt > min_volt) && (max_volt - min_volt) > 1e-6) && (j != k))
		{
			error = 0;
		}
		else
		{
			error = 1;
		}
	}
}


void LED_Proc(void)
{
	if(led_choose == 1)
	{
		LED(led_buff[j]);
		HAL_Delay(200);
		LED(led_buff[0]);
		HAL_Delay(200);
	}
	else if(led_choose == 2)
	{
		LED(led_buff[k]);
		HAL_Delay(200);
		LED(led_buff[0]);
		HAL_Delay(200);
	}
	else if(led_choose == 3)
	{
		LED(led_buff[0]);
	}
	else
	{
		LED(0x00);
	}
}

















void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV3;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

