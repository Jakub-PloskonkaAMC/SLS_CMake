/*
 * ad7768.h
 *
 *  Created on: 17.07.2019
 *      Author: mantoniak
 */

#ifndef DRIVERS_AD7768_H_
#define DRIVERS_AD7768_H_

#include "conf.h"

void ad7768_set_cs(void);
#ifdef SENSOR_TYPE_HF

#define ADC_REGISTER_READ 0x40

#define ADC_SCRATCH_PAD 0x0A
#define ADC_POWER_CLOCK 0x15
#define ADC_ANALOG 0x16
#define ADC_ANALOG2 0x17
#define ADC_CONVERSION 0x18
#define ADC_DIGITAL_FILTER 0x19
#define ADC_SYNC_RESET 0x1D
#define ADC_DATA 0x2C
#define ADC_INTERFACE_FORMAT 0x14
#define ADC_GAIN_HI 0x24
#define ADC_GAIN_MID 0x25
#define ADC_GAIN_LO 0x26
#define ADC_OFFSET_HI 0x21
#define ADC_OFFSET_MID 0x22
#define ADC_OFFSET_LO 0x23

#define VENDOR_L 0x0C /// referencja o wartosci 0x56

#define ADC_CONV_MODE 0x01
#define ADC_SPI_START 0x00
#define ADC_EN_CONT_READ 0x01
#define ADC_DIS_CONT_READ 0x6C
#define ADC_DEC_RATE_128 0x02

#define ADC_MASTER_STATUS 0x2D
#define ADC_SPI_DIAG_STATUS 0x2E
#define ADC_DIAG_STATUS 0x2F
#define ADC_DIG_DIAG_STATUS 0x30



void ad7768_reset_cs(void);
void ad7768_get_data(uint8_t *data_rec);
uint8_t ad7768_random();
void ad7768_Init(void);
void ad7768_start(void);
void ad7768_stop(void);
uint32_t ad7768_get_gain(void);
uint32_t ad7768_get_offset(void);
void ad7768_get_vendor_code(uint8_t* result);

void ad7768_show_registers(void);

uint8_t ad7768_get_ADC_POWER_CLOCK(void);



#endif /*SENSOR_TYPE_HF*/

#endif /* DRIVERS_AD7768_H_ */
