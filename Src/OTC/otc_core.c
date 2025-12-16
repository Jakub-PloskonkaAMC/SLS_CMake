/*
 * otc_core.c
 *
 *  Created on: 13 sie 2020
 *      Author: mantoniak
 */

#include "otc_core.h"
#include "otc_flash.h"
#include "debug.h"
#include "board.h"
#include <string.h>

typedef enum OTC_Subcommand
{
    OTC_GetVersion = 0x01,
	OTC_Preparation = 0x02,
	OTC_WriteData = 0x03,
	OTC_VerifyData = 0x04,
	OTC_Done = 0xDC,
	OTC_Reboot = 0xF0,
}OTC_Subcommand_t;

#define null NULL

send_resp_func send_response = null;

static void Response(uint8_t flags, uint8_t* data, uint16_t dataLen)
{
	if (send_response != null)
		send_response(Sensor.configuration.sensor_ID,0x31,flags,data,dataLen);
}

#define OTC_ACK 1
#define OTC_NACK 0

static void GetVersion(uint8_t* data, uint16_t length)
{
	//PRINTF_HW("In func: %s\n",__FUNCTION__);
	uint8_t responseBuffer[8];

	responseBuffer[0] = OTC_GetVersion;
	responseBuffer[1] = SOFTWARE_MAJOR;
	responseBuffer[2] = SOFTWARE_MINOR;
	responseBuffer[3] = SOFTWARE_PATCH;
	responseBuffer[4] = HARDWARE_MAJOR;
	responseBuffer[5] = HARDWARE_MINOR;
	responseBuffer[6] = HARDWARE_REVISION;
#ifdef SENSOR_TYPE_HF
	responseBuffer[7] = 0x9F;
#else
	responseBuffer[7] = 0x1F;
#endif
	Response(OTC_ACK, responseBuffer, 8);
}

static void Prepare(uint8_t* data, uint16_t length)
{
	OTC_Erese_Internal();
	PRINTF_HW("Flash prepared for OTC\n");
	uint8_t responseData[1];
	responseData[0] = OTC_Preparation;
	Response(OTC_ACK, responseData, 1);
}

static void WriteData(uint8_t* data, uint16_t length)
{
	uint8_t startPage = 0xFF;
	uint16_t startOffset = 0xFFFF;
	uint16_t partLength = 0xFFFF;
	startPage = data[1];
	memcpy(&startOffset,data+2,sizeof(uint16_t));
	memcpy(&partLength,data+4,sizeof(uint16_t));
	uint32_t addressForWriting = 0x1000*startPage+startOffset;
	PRINTF_HW("Write start addr: 0x%05X  curPage: %d curPart %d\n",addressForWriting,startPage,startOffset);
	for(int i = 0; i < length-6; i++)
	{
		OTC_Write(addressForWriting+i, data[i+6]);
	}

	uint8_t responseData[1];
	responseData[0] = OTC_WriteData;
	Response(OTC_ACK, responseData, 1);
}

static uint8_t tempBuffer[0x1000];

static void VerifyData(uint8_t* data, uint16_t length)
{
	uint8_t startPage = 0xFF;
	uint16_t startOffset = 0xFFFF;
	uint16_t partLength = 0xFFFF;
	startPage = data[1];
	memcpy(&startOffset,data+2,sizeof(uint16_t));
	memcpy(&partLength,data+4,sizeof(uint16_t));
	uint16_t lenToVerify = length  - 6;
	/*for (int i = 0; i < 8; i++)
	{
		PRINTF_HW("XXX[%d] = 0x%02X\n",i,data[i]);
	}*/
	uint32_t addressForRead = 0x1000*startPage+startOffset;
	PRINTF_HW("Address to read to: 0x%05X\n",addressForRead);
	OTC_Read(addressForRead, tempBuffer, lenToVerify);
	uint8_t responseData[1];
	responseData[0] = OTC_VerifyData;
	if (memcmp(tempBuffer,data+6,lenToVerify) == 0)
	{
		Response(OTC_ACK, responseData, 1);
	}
	else
	{
		Response(OTC_NACK, responseData, 1);
		/*for (int i = 0; i < lenToVerify; i++)
		{
			PRINTF_HW("C[%d] 0x%02X\t\t0x%02X\n",i,data[i+6],tempBuffer[i]);
		}*/
	}
}

static void Done(uint8_t* data, uint16_t length)
{
	uint8_t responseData[1];
	responseData[0] = OTC_Done;
	Response(OTC_ACK, responseData, 1);
	for(int i = 0; i < 5000; i++)
	{
		for(int j = 0; j < 1000; j++)
		{
			__asm("nop");
		}
	}
	NVIC_SystemReset();
}

void OTC_Body(uint8_t* data, uint16_t length)
{
	OTC_Subcommand_t sub_com = data[0];
	switch(sub_com)
	{
	case OTC_GetVersion:
		GetVersion(data,length);
		break;
	case OTC_Preparation:
		Prepare(data,length);
		break;
	case OTC_WriteData:
		WriteData(data,length);
		break;
	case OTC_VerifyData:
		VerifyData(data,length);
		break;
	case OTC_Done:
		Done(data,length);
		break;
	case OTC_Reboot:
		NVIC_SystemReset();
		break;
	}
}

void OTC_RegisterResponseHandler(send_resp_func handler)
{
	send_response = handler;
}

