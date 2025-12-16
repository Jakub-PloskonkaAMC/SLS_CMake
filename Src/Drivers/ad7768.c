/*
 * ad7768.c
 *
 *  Created on: 17.07.2019
 *      Author: mantoniak
 */

#include "ad7768.h"
#include "spi.h"
#include "debug.h"
#include "board.h"
#include "gpio.h"


#ifdef SENSOR_TYPE_HF

static inline void delay_normal(int time){
	int volatile i = 0;
	for(i = 0; i<time; ++i){
	}
}

void ad7768_set_cs(void)
{
	HAL_GPIO_WritePin (CS_ADXL_GPIO_Port ,CS_ADXL_Pin , GPIO_PIN_SET);
}

void ad7768_reset_cs(void)
{

	HAL_GPIO_WritePin (CS_ADXL_GPIO_Port ,CS_ADXL_Pin , GPIO_PIN_RESET);
}




static void ad7768_read_reg(uint8_t RegisterAddress, uint8_t* Data, uint16_t Size)
{
	  uint8_t data_send = (RegisterAddress | ADC_REGISTER_READ);

	  ad7768_reset_cs();
	  HAL_SPI_Transmit(&hspi1, &data_send, 1, 100);

	  delay_normal(20);

	  HAL_SPI_Receive(&hspi1, Data, Size, 100);
	  ad7768_set_cs();
}

static void ad7768_write_reg(uint8_t RegisterAddress, uint8_t RegisterState)
{
	  uint8_t data_send[2] = {RegisterAddress,RegisterState};
	  ad7768_reset_cs();
	  HAL_SPI_Transmit(&hspi1, data_send, 2, 100);
	  ad7768_set_cs();
}

void ad7768_get_data(uint8_t *data_rec)
{
	uint8_t data_send[] = {0,0,0};
	ad7768_reset_cs();
	if(HAL_SPI_TransmitReceive_IT_Lightsome(&hspi1, data_send, data_rec, 3)==HAL_OK){
		//LED_green_Toggle();
	}
	else{
		//LED_red_Toggle();
	}
}


uint8_t ad7768_random()
{
	uint8_t data[3];

	ad7768_get_data(data);
	return data[2];
}



void ad7768_Init(void)
{
	HAL_GPIO_WritePin(RESET_AD_GPIO_Port, RESET_AD_Pin, GPIO_PIN_RESET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(RESET_AD_GPIO_Port, RESET_AD_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
	//ad7768_reset_cs();
	PRINTF_HW("At boot\r\n");

	ad7768_show_registers();
	// TODO CHANGE 6/7bit

#ifdef FILTER_TYPE_SINC
	ad7768_write_reg(ADC_POWER_CLOCK,0x33);//0x33); //CMOS
	ad7768_write_reg(ADC_ANALOG,  0xA0);
	ad7768_write_reg(ADC_ANALOG2,  0x00);
	ad7768_write_reg(ADC_DIGITAL_FILTER, 0x03); //sinc5 filter x8   //0x03 32k
	ad7768_write_reg(ADC_CONVERSION, 0x00);
#endif

#ifdef FILTER_TYPE_FIR_SLOW
	ad7768_write_reg(ADC_POWER_CLOCK,0x33);//0x33); //CMOS
	ad7768_write_reg(ADC_ANALOG,  0xA0);
	ad7768_write_reg(ADC_ANALOG2,  0x00);
	ad7768_write_reg(ADC_DIGITAL_FILTER, 0x43); //sinc5 filter x8   //0x03 32k
	ad7768_write_reg(ADC_CONVERSION, 0x00);
#endif

#ifdef FILTER_TYPE_FIR
	ad7768_write_reg(ADC_POWER_CLOCK,0x33);//0x33); //CMOS
	ad7768_write_reg(ADC_ANALOG,  0xA0);
	ad7768_write_reg(ADC_ANALOG2,  0x00);
	ad7768_write_reg(ADC_DIGITAL_FILTER, 0x43); //sinc5 filter x8   //0x03 32k
	ad7768_write_reg(ADC_CONVERSION, 0x00);
#endif

	PRINTF_HW("After initialization\r\n");
	ad7768_show_registers();
	//diagnostic registers
	ad7768_write_reg(0x29, 0x3F);
	ad7768_write_reg(0x28, 0x0E);

	//ad7768_set_cs();
}

void ad7768_start(void)
{
	ad7768_reset_cs();
	ad7768_write_reg(0x14, 0x01);
	ad7768_set_cs();
}

void ad7768_stop(void){
	uint8_t data = (uint8_t) ADC_DIS_CONT_READ;
	ad7768_reset_cs();
	HAL_SPI_Transmit(&hspi1, &data, 1, 100);
	ad7768_set_cs();
}

uint32_t ad7768_get_gain(void)
{
	uint8_t result[3];


	ad7768_read_reg(ADC_GAIN_HI, result, 1);
	ad7768_read_reg(ADC_GAIN_MID, result+1, 1);
	ad7768_read_reg(ADC_GAIN_LO, result+2, 1);

	return (uint32_t)(result[2]+(result[1]<<8)+(result[0]<<16));
}

uint32_t ad7768_get_offset(void)
{
	uint8_t result[3];


	ad7768_read_reg(ADC_OFFSET_HI, result, 1);
	ad7768_read_reg(ADC_OFFSET_MID, result+1, 1);
	ad7768_read_reg(ADC_OFFSET_LO, result+2, 1);

	return (uint32_t)(result[2]+(result[1]<<8)+(result[0]<<16));
}

void ad7768_get_vendor_code(uint8_t *result)
{

	ad7768_read_reg(0x4C, result,1);

}

uint8_t ad7768_get_ADC_reg(uint8_t reg_num)
{
	uint8_t readRes;

	ad7768_read_reg(reg_num, &readRes, 1);

	return (uint8_t)(readRes);
}

void ad7768_get_adc(uint8_t *result)
{

	ad7768_read_reg(0x4C, result,1);

}

uint8_t ad7768_get_gain5(void)
{
	uint8_t result5[1];


	ad7768_read_reg(ADC_ANALOG2, result5, 1);

	return (uint8_t)(result5[0]);
}


void ad7768_show_registers(void)
{
	uint8_t adc_result0 = ad7768_get_ADC_reg(ADC_ANALOG2);
	PRINTF_HW("ADC_ANALOG2: 0x%02X\r\n",adc_result0);

	adc_result0 = ad7768_get_ADC_reg(ADC_DIGITAL_FILTER);
	PRINTF_HW("ADC_DIGITAL_FILTER: 0x%02X\r\n",adc_result0);

	adc_result0 = ad7768_get_ADC_reg(ADC_POWER_CLOCK);
	PRINTF_HW("ADC_POWER_CLOCK: 0x%02X\r\n",adc_result0);

	adc_result0 = ad7768_get_ADC_reg(ADC_MASTER_STATUS);
	PRINTF_HW("MASTER STATUS: 0x%02X\r\n",adc_result0);

	adc_result0 = ad7768_get_ADC_reg(ADC_SPI_DIAG_STATUS);
	PRINTF_HW("SPI_DIAG_STATUS: 0x%02X\r\n",adc_result0);

	adc_result0 = ad7768_get_ADC_reg(ADC_DIAG_STATUS);
	PRINTF_HW("ADC_DIAG_STATUS: 0x%02X\r\n",adc_result0);

	adc_result0 = ad7768_get_ADC_reg(ADC_DIG_DIAG_STATUS);
	PRINTF_HW("DIG_DIAG_STATUS: 0x%02X\r\n",adc_result0);


	adc_result0 = ad7768_get_ADC_reg(VENDOR_L);
	PRINTF_HW("VENDOR_L: 0x%02X\r\n",adc_result0);
}


#endif
