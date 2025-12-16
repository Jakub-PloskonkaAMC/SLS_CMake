/*
 * gpio_hf.h
 *
 *  Created on: 17.07.2019
 *      Author: mantoniak
 */

#ifndef DRIVERS_GPIO_HF_H_
#define DRIVERS_GPIO_HF_H_

#include "conf.h"

#ifdef SENSOR_TYPE_HF

#define LED_G_Pin GPIO_PIN_5
#define LED_G_GPIO_Port GPIOB
#define LED_R_Pin GPIO_PIN_15
#define LED_R_GPIO_Port GPIOA

#define USART__R_E_Pin GPIO_PIN_12
#define USART__R_E_GPIO_Port GPIOB
#define USART_DE_Pin GPIO_PIN_10
#define USART_DE_GPIO_Port GPIOB

#define DRDY_ADC_Pin GPIO_PIN_0
#define DRDY_ADC_GPIO_Port GPIOB

#define RESET_AD_Pin GPIO_PIN_3
#define RESET_AD_GPIO_Port GPIOA
#define CLOCK_SEL_Pin GPIO_PIN_14
#define CLOCK_SEL_GPIO_Port GPIOC

#define CS_ADXL_Pin GPIO_PIN_4
#define CS_ADXL_GPIO_Port GPIOA
#define SCLK_ADXL_Pin GPIO_PIN_5
#define SCLK_ADXL_GPIO_Port GPIOA
#define MISO_ADXL_Pin GPIO_PIN_6
#define MISO_ADXL_GPIO_Port GPIOA
#define MOSI_ADXL_Pin GPIO_PIN_7
#define MOSI_ADXL_GPIO_Port GPIOA

void LED_green_Toggle(void);
void LED_green_On(void);
void LED_green_Off(void);
void LED_red_Toggle(void);
void LED_red_On(void);
void LED_red_Off(void);
void GPIO_Init(void);
void EXTI3_IRQHandler(void);

#endif

#endif /* DRIVERS_GPIO_HF_H_ */
