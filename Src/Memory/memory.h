/*
 * memory.h
 *
 *  Created on: 01.07.2019
 *      Author: mantoniak
 */

#ifndef MEMORY_MEMORY_H_
#define MEMORY_MEMORY_H_

#include "conf.h"

#define MEMORY_START FLASH_USER_START
#define MEMORY_FIRST_CONFIG_ADDRESS MEMORY_START
#define MAX_CONFIGS 100

#define MAC_ADDRESS_NUMBER_OF_BYTES 12

#define MAGIC_FLASH_NUMBER 0xDAEDDAED

typedef enum __attribute__((packed))
{
   speed_4800 = 0x10,
   speed_7200 = 0x20,
   speed_9600 = 0x30,
   speed_14400 = 0x40,
   speed_19200 = 0x50,
   speed_28800 = 0x60,
   speed_33600 = 0x70,
   speed_38400 = 0x80,
   speed_57600 = 0x90,
   speed_115k = 0xA0,
   speed_500k = 0xB0,
   speed_1000k = 0xC0,
   speed_1500k = 0xD0,
}ADI_rs485_speed;

typedef struct __attribute__((__packed__))
{
    uint32_t magicNumber;
    uint16_t conf_id;
    uint8_t sensor_ID;
    char MAC[MAC_ADDRESS_NUMBER_OF_BYTES];
    ADI_rs485_speed RS485_speed;
    CurrentProtocol protocol;
    char sensor_type[12];
    uint32_t X_Calibration_A;
    uint32_t X_Calibration_B;
    uint32_t X_Calibration_Sensitivity;
    uint32_t Y_Calibration_A;
    uint32_t Y_Calibration_B;
    uint32_t Y_Calibration_Sensitivity;
    uint32_t Z_Calibration_A;
    uint32_t Z_Calibration_B;
    uint32_t Z_Calibration_Sensitivity;
}AdiConf_t;

void Memory_Init(void);
void Memory_TakeConfig(AdiConf_t* conf);
bool Memory_SaveConfig(AdiConf_t* conf);
void Memory_ShowConfig(AdiConf_t* conf);
void Memory_MakeDefaultConfig(AdiConf_t* conf);

#endif /* MEMORY_MEMORY_H_ */
