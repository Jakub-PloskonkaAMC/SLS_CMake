/*
 * memory.c
 *
 *  Created on: 01.07.2019
 *      Author: mantoniak
 */

#include "memory.h"
#include "board.h"
#include "flash_handler.h"
#include "debug.h"
#include <stdio.h>
#include "conf.h"

static uint16_t configsInMemory;
static AdiConf_t currentConfig;

#define testnazwy 7
#define STRINGIFY(x) #x
#define VALUE_TO_STRING(x)  STRINGIFY(x)


#ifdef  SENSOR_TYPE_LF
void Memory_MakeDefaultConfig(AdiConf_t* conf)
{
	conf->magicNumber = MAGIC_FLASH_NUMBER;
	conf->conf_id = 0;
	conf->sensor_ID = 1;
	conf->RS485_speed = speed_9600;
	conf->protocol = MODBUS;

	uint32_t idA = HAL_GetUIDw0();
	uint32_t idB = HAL_GetUIDw1();
	uint32_t idC = HAL_GetUIDw2();

	memcpy(conf->MAC,&idA,4);
	memcpy((conf->MAC)+4,&idB,4);
	memcpy((conf->MAC)+8,&idC,4);

	strcpy(conf->sensor_type, "AVS 1003LF");

	conf->X_Calibration_A = 0x3f800000;				// 1
	conf->X_Calibration_B = 0;
	conf->X_Calibration_Sensitivity = 0x3fc00000;	// 1.5

	conf->Y_Calibration_A = 0x3f800000;				// 1
	conf->Y_Calibration_B = 0;
	conf->Y_Calibration_Sensitivity = 0x3fc00000;	// 1.5

	conf->Z_Calibration_A = 0x3f800000;				// 1
	conf->Z_Calibration_B = 0;
	conf->Z_Calibration_Sensitivity = 0x3fc00000;	// 1.5

}
#else
void Memory_MakeDefaultConfig(AdiConf_t* conf)
{
	conf->magicNumber = MAGIC_FLASH_NUMBER;
	conf->conf_id = 0;
	conf->sensor_ID = 1;
	conf->RS485_speed = speed_9600;
	conf->protocol = MODBUS;

	uint32_t idA = HAL_GetUIDw0();
	uint32_t idB = HAL_GetUIDw1();
	uint32_t idC = HAL_GetUIDw2();

	memcpy(conf->MAC,&idA,4);
	memcpy((conf->MAC)+4,&idB,4);
	memcpy((conf->MAC)+8,&idC,4);

	strcpy(conf->sensor_type, "AVS 1001HF");

	conf->X_Calibration_A = 0x3f820370;				// 1.01573 (1.0157299)
	conf->X_Calibration_B = 0x48e81b56;				// 475354.7
	conf->X_Calibration_Sensitivity =0x42200000;	// 40
}

#endif


static void Memory_ReadConfigs(void)
{
	AdiConf_t readedConf;

	for (int i = 0; i < MAX_CONFIGS; i++)
	{
		Memory_LL_Read(MEMORY_FIRST_CONFIG_ADDRESS+i*sizeof(AdiConf_t),(uint8_t*)&readedConf,sizeof(AdiConf_t));
		if (readedConf.conf_id == 0xFFFF)
		{
			if (readedConf.conf_id != configsInMemory)
			{
				break;
			}
			break;
		}
		if (readedConf.magicNumber != (uint32_t)MAGIC_FLASH_NUMBER)
		{
			Memory_LL_Clear();
			for(int i = 0; i < 800; i++)
			{
				for(int ii = 0; ii < 10000; ii++)
				{
					__asm("nop");
				}
			}
			AdiConf_t newConfig;
			Memory_MakeDefaultConfig(&newConfig);
			Memory_SaveConfig(&newConfig);
			for(int i = 0; i < 800; i++)
			{
				for(int ii = 0; ii < 10000; ii++)
				{
					__asm("nop");
				}
			}
			NVIC_SystemReset();
		}
		configsInMemory++;
	}
	if (configsInMemory > 0)
	{
		Memory_LL_Read(MEMORY_FIRST_CONFIG_ADDRESS+(configsInMemory-1)*sizeof(AdiConf_t),(uint8_t*)&readedConf,sizeof(AdiConf_t));
		PRINTF_MEM("So far %d configs has been saved to memory\n",configsInMemory);
	}

	if (configsInMemory == 0)
	{
		PRINTF_MEM("No config found in memory, creating default\n");
		Memory_MakeDefaultConfig(&readedConf);
		Memory_SaveConfig(&readedConf);
	}

	memcpy(&currentConfig,&readedConf,sizeof(AdiConf_t));
}

void Memory_Init(void)
{
	configsInMemory = 0;
	Flash_Init();
	Memory_ReadConfigs();
}

bool Memory_SaveConfig(AdiConf_t* conf)
{
	conf->magicNumber = MAGIC_FLASH_NUMBER;
	if (configsInMemory >= MAX_CONFIGS)
	{
		Memory_LL_Clear();
		configsInMemory = 0;
	}
	PRINTF_MEM("Saving config\n");
	conf->conf_id = configsInMemory;
	bool response;
	response = Memory_LL_Save(MEMORY_FIRST_CONFIG_ADDRESS+configsInMemory*sizeof(AdiConf_t),(uint8_t*)conf,sizeof(AdiConf_t));
	memcpy(&currentConfig,conf,sizeof(AdiConf_t));
	configsInMemory++;
	return response;
}

void Memory_TakeConfig(AdiConf_t* conf)
{
	memcpy(conf,&currentConfig,sizeof(AdiConf_t));
	PRINTF_MEM("Config taken\n");
}

void Memory_ShowConfig(AdiConf_t* conf)
{
	PRINTF_MEM("Sensor config:\n");
	PRINTF_MEM("Config id: 0x%02X\n",conf->conf_id);
	PRINTF_MEM("Sensor ID: 0x%02X\n",conf->sensor_ID);
	PRINTF_MEM("Sensor MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\n",conf->MAC[0],conf->MAC[1],conf->MAC[2],conf->MAC[3],conf->MAC[4],conf->MAC[5],conf->MAC[6],conf->MAC[7],conf->MAC[8],conf->MAC[9],conf->MAC[10],conf->MAC[11]);
	PRINTF_MEM("Sensor Type: %c %c %c %c %c %c %c %c",conf->sensor_type[0],conf->sensor_type[1],conf->sensor_type[2],conf->sensor_type[3],conf->sensor_type[4],conf->sensor_type[5],conf->sensor_type[6],conf->sensor_type[7]);
	PRINTF_MEM("\n");
	PRINTF_MEM("Os X: %08X, %08X, %08X \n", conf->X_Calibration_A, conf->X_Calibration_B, conf->X_Calibration_Sensitivity);
	PRINTF_MEM("Os Y: %08X, %08X, %08X \n", conf->Y_Calibration_A, conf->Y_Calibration_B, conf->Y_Calibration_Sensitivity);
	PRINTF_MEM("Os Z: %08X, %08X, %08X \n", conf->Z_Calibration_A, conf->Z_Calibration_B, conf->Z_Calibration_Sensitivity);
	char name[20];
	memset(name,0,20);
	switch (conf->RS485_speed)
	{
	case speed_4800:
		sprintf(name,"speed_4800");
		break;
	case speed_7200:
		sprintf(name,"speed_7200");
		break;
	case speed_9600:
		sprintf(name,"speed_9600");
		break;
	case speed_14400:
		sprintf(name,"speed_14400");
		break;
	case speed_19200:
		sprintf(name,"speed_19200");
		break;
	case speed_28800:
		sprintf(name,"speed_28800");
		break;
	case speed_33600:
		sprintf(name,"speed_33600");
		break;
	case speed_38400:
		sprintf(name,"speed_38400");
		break;
	case speed_57600:
		sprintf(name,"speed_57600");
		break;
	case speed_115k:
		sprintf(name,"speed_115k");
		break;
	case speed_500k:
		sprintf(name,"speed_500k");
		break;
	case speed_1500k:
		sprintf(name,"speed_1500k");
		break;
	case speed_1000k:
		sprintf(name,"speed_1000k");
		break;
	default:
		sprintf(name,"bad speed");
		break;
	}
	PRINTF_MEM("Sensor speed: %s\n", name);
	PRINTF_MEM("Sensor protocol: %s\n", enum2str(conf->protocol));
}
