/*
 * adxl.c
 *
 *  Created on: 11.06.2019
 *      Author: mkruzlak
 */

#include "adxl.h"
#include "stm32f4xx_hal.h"
#include "gpio.h"


#ifdef SENSOR_TYPE_LF

static inline void delay_normal(int time){
	int volatile i = 0;
	for(i = 0; i<time; ++i){
	}
}

uint8_t adxl_read(uint8_t address){

	  //reading value form address

	  uint8_t data_rec = 0;
	  address = address << 1;
	  address = address | 0x01;
	  HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	  delay_normal(2);
	  HAL_SPI_Transmit(&hspi1, &address, 1, 100);
	  delay_normal(2);
	  HAL_SPI_Receive(&hspi1, &data_rec, 1, 100);
	  HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

	  return data_rec;
}

void adxl_read_fast(uint8_t address, uint8_t* data_rec){

	  //reading value form address

	  address = address << 1;
	  address = address | 0x01;
	  HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	  delay_normal(2);
	  HAL_SPI_Transmit(&hspi1, &address, 1, 100);
	  delay_normal(2);
	  HAL_SPI_Receive(&hspi1, data_rec, 9, 100);
	  HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}


uint8_t adxl_GetID(void)
{
	  uint8_t data_rec = 0;
	  data_rec=adxl_read(0);
	  return data_rec;
}

uint8_t adxl_GetMST(void)
{
	  uint8_t data_rec = 0;
	  data_rec=adxl_read(0x01);
	  return data_rec;
}

uint8_t adxl_GetPARTID(void)
{
	  uint8_t data_rec = 0;
	  data_rec=adxl_read(0x02);
	  return data_rec;
}

void adxl_Init(void)
{
	uint8_t data[2];
	uint8_t address = 0x2D << 1;
	data[0] = address;
	data[1] = 0x00;

	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	delay_normal(20);
	HAL_SPI_Transmit(&hspi1, data, 2, 100);
	delay_normal(20);
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}

void adxl_Stop(void)
{
	uint8_t data[2];
	uint8_t address = 0x2D << 1;
	data[0] = address;
	data[1] = 0x01;

	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	delay_normal(20);
	HAL_SPI_Transmit(&hspi1, data, 2, 100);
	delay_normal(20);
	HAL_GPIO_WritePin (GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}

void adxl_GetAxisData_fast(uint8_t* data)
{
//LED_red_On();
	adxl_read_fast(ADXL_XDATA3_ADDRESS,data);
//LED_red_Off();
}

uint32_t adxl_GetAxisData(ADXL_AXIS Ax)
{
	//LED_red_On();
	uint32_t out = 0;

	switch (Ax){

	case DataX:

		out = (adxl_read(ADXL_XDATA3_ADDRESS)<<16);
		out += (adxl_read(ADXL_XDATA2_ADDRESS)<<8);
		out += (adxl_read(ADXL_XDATA1_ADDRESS));
		break;
	case DataY:
		out = (adxl_read(ADXL_YDATA3_ADDRESS)<<16);
		out += (adxl_read(ADXL_YDATA2_ADDRESS)<<8);
		out += (adxl_read(ADXL_YDATA1_ADDRESS));
		break;
	case DataZ:
		out = (adxl_read(ADXL_ZDATA3_ADDRESS)<<16);
		out += (adxl_read(ADXL_ZDATA2_ADDRESS)<<8);
		out += (adxl_read(ADXL_ZDATA1_ADDRESS));
		break;
	default:

		break;
	}
//LED_red_Off();
	return out;
}

//uint16_t U2_to_(uint16_t u2Numb){
//
//	int result = 0;
//	uint16_t sign = u2Numb >> 15;
//
//	if(sign == 1){
//		result = (~u2Numb & 0x7fff) + 1;
//		return -result;
//	}
//	else{
//		result = u2Numb;
//		return result;
//	}
//
//
//}

void adxl_GetTemp(uint8_t * result){
	*result = adxl_read(ADXL_TEMP2_ADDRESS);
	*(result+1) = adxl_read(ADXL_TEMP1_ADDRESS);
}

int U2_to_dec(uint16_t u2Numb){

	int result = 0;
	uint16_t sign = u2Numb >> 15;

	if(sign == 1){
		result = (~u2Numb & 0x7fff) + 1;
		return -result;
	}
	else{
		result = u2Numb;
		return result;
	}


}

#endif
