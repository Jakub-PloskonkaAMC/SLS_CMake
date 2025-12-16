/*
 * gpio_hf.c
 *
 *  Created on: 17.07.2019
 *      Author: mantoniak
 */

#include "acquisition.h"
#include "gpio_hf.h"
#include "board.h"
#include "ad7768.h"

#ifdef SENSOR_TYPE_HF

void InterruptInit(void);
uint8_t sample[3];
extern uint8_t drdy;


void LED_green_Toggle(void)
{
	HAL_GPIO_TogglePin(LED_G_GPIO_Port,LED_G_Pin);
}

void LED_green_On(void)
{
	HAL_GPIO_WritePin(LED_G_GPIO_Port,LED_G_Pin,GPIO_PIN_RESET);
}

void LED_green_Off(void)
{
	HAL_GPIO_WritePin(LED_G_GPIO_Port,LED_G_Pin,GPIO_PIN_SET);
}

void LED_red_Toggle(void)
{
	HAL_GPIO_TogglePin(LED_R_GPIO_Port,LED_R_Pin);
}

void LED_red_On(void)
{
	HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,GPIO_PIN_RESET);
}

void LED_red_Off(void)
{
	HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,GPIO_PIN_SET);
}

void GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();


  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_R_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, LED_G_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, CS_ADXL_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, RESET_AD_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, CLOCK_SEL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA8 */
   GPIO_InitStruct.Pin = GPIO_PIN_8;
   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = CLOCK_SEL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LED_R_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = USART_DE_Pin|USART__R_E_Pin|LED_G_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = CS_ADXL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = RESET_AD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = DRDY_ADC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  InterruptInit();
}

void InterruptInit(void)
{
	  HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
}

void EXTI0_IRQHandler(void)
{
#ifdef DOWNSAMPLING
	if (drdy){
		ad7768_get_data(sample);
		drdy = 0;
	}

	__HAL_GPIO_EXTI_CLEAR_IT(DRDY_ADC_Pin);
#else
	ad7768_get_data(sample);
	__HAL_GPIO_EXTI_CLEAR_IT(DRDY_ADC_Pin);
#endif
}


#endif
