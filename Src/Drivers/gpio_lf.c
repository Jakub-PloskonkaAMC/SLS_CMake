/*
 * gpio_lf.c
 *
 *  Created on: 17.07.2019
 *      Author: mantoniak
 */
#include "gpio_lf.h"

#ifdef SENSOR_TYPE_LF

#include "acquisition.h"

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
  HAL_GPIO_WritePin(GPIOB, LED_G_Pin|LED_R_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, CS_ADXL_Pin, GPIO_PIN_SET);


  /*Configure GPIO pins : LED_G_Pin LED_R_Pin */
  GPIO_InitStruct.Pin = USART_DE_Pin|USART__R_E_Pin|LED_G_Pin|LED_R_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = CS_ADXL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = DRDY_ADXL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void EXTI3_IRQHandler(void)
{
	ACQ_DoJob();
    __HAL_GPIO_EXTI_CLEAR_IT(DRDY_ADXL_Pin);

	HAL_GPIO_TogglePin(LED_R_GPIO_Port,LED_R_Pin);
    //ACQ_DoJob();
}

#endif
