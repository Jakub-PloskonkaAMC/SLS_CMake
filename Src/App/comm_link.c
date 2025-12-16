/*! ----------------------------------------------------------------------------
 * Include
------------------------------------------------------------------------------*/
#include "comm_link.h"
#include "rs485.h"
#include "crc16.h"
#include "debug.h"
#include "board.h"
#include "comm_app.h"
#include "otc_core.h"

#include "cyclicBuffer.h"

#include <stdbool.h>
#include <string.h>

/*! ----------------------------------------------------------------------------
 * Define
------------------------------------------------------------------------------*/

#define dFrameSize_Preamble     4
#define dFrameSize_Length       2
#define dFrameSize_AddrSensor	1
#define dFrameSize_Command		1
#define dFrameSize_Flags		1
#define dFrameSize_CrcHeader	2
#define dFrameSize_Crc          2
#define dFrameSize_Mac          12

#define dFrameOffset_Preamble    	0
#define dFrameOffset_Length      	(dFrameOffset_Preamble + dFrameSize_Preamble)
#define dFrameOffset_AddrSensor  	(dFrameOffset_Length + dFrameSize_Length)
#define dFrameOffset_Command	 	(dFrameOffset_AddrSensor + dFrameSize_AddrSensor)
#define dFrameOffset_Flags	     	(dFrameOffset_Command + dFrameSize_Command)
#define dFrameOffset_CrcHeader  	(dFrameOffset_Flags + dFrameSize_Flags)
#define dFrameOffset_Data			(dFrameOffset_CrcHeader + dFrameSize_CrcHeader)

#define dFrameSize            		800

//#define COMM_APP_TEST_MODE

typedef struct
{
	uint16_t length;
	uint8_t addrSensor;
	uint8_t command;
	uint8_t flags;
	uint16_t crcHeader;
	uint8_t data[dFrameSize];
	uint16_t crc;
}Comm_Frame_t;



/*! ----------------------------------------------------------------------------
 * Variable
------------------------------------------------------------------------------*/
const uint32_t _preamble = 0xA5AAAAAA;
static Comm_Frame_t Comm_RxMessage;

/*! ----------------------------------------------------------------------------
 * Functions
------------------------------------------------------------------------------*/
static void (*_phyInit_func)(ADI_rs485_speed spd);
static uint16_t (*_phySend_func)(const void *data, uint16_t len);
static bool (*_phyGetChar_func)(uint8_t *ch);
static void (*_phySendChar_func)(const uint8_t *ch);
static void (*_phyTransmitOn_func)(void);
static void (*_phyTransmitOff_func)(void);

/******************************************************************************/


static uint16_t CommLink_CalcFrameCrc(Comm_Frame_t *mess)
{
	uint16_t crc = 0;
	crc = CalCrc16(crc, &mess->length, sizeof(mess->length));
	crc = CalCrc16(crc, &mess->addrSensor, sizeof(mess->addrSensor));
	crc = CalCrc16(crc, &mess->command, sizeof(mess->command));
	crc = CalCrc16(crc, &mess->flags, sizeof(mess->flags));
	crc = CalCrc16(crc, &mess->crcHeader, sizeof(mess->crcHeader));
	crc = CalCrc16(crc, mess->data, mess->length - dFrameSize_Length - dFrameSize_AddrSensor - dFrameSize_Command - dFrameSize_Flags - dFrameSize_CrcHeader - dFrameSize_Crc);

	return crc;
}

/******************************************************************************/
static Comm_FrameError_t CommLink_ProcessByte(uint8_t proc_byte)
{
	static uint16_t idx;

	PRINTF_COMM("Byte proceeded: %X \n", proc_byte);

	if((idx > dFrameOffset_Data) && (idx-dFrameOffset_Data >= dFrameSize))
	{
		idx = 0;

		return FrameError;
	}

	if(idx < dFrameOffset_Length)
	{
		switch(idx)
		{
		case 0:
		case 1:
		case 2:
			if(proc_byte != 0xAA)
			{
				idx = 0;
				//PRINTF_COMM("0xAA \n");
				return FrameError;
			}
			break;
		case 3:
			if(proc_byte != 0xA5)
			{
				idx = 0;
				//PRINTF_COMM("0xA5 \n");
				return FrameError;
			}
			break;
		}
	}
	else if(idx == dFrameOffset_Length)
	{
		Comm_RxMessage.length = proc_byte;
	}
	else if(idx == dFrameOffset_Length + 1)
	{
		Comm_RxMessage.length |= proc_byte << 8;
	}
	else if(idx == dFrameOffset_AddrSensor)
		Comm_RxMessage.addrSensor = proc_byte;
	else if(idx == dFrameOffset_Command)
		Comm_RxMessage.command = proc_byte;
	else if(idx == dFrameOffset_Flags)
	{
		Comm_RxMessage.flags = proc_byte;
		if (Comm_RxMessage.command == getData && Comm_RxMessage.addrSensor != Sensor.configuration.sensor_ID && Comm_RxMessage.flags != 0)
		{
			//TODO: zakomentowane
			///RS485_RX_Sleep(Comm_RxMessage.length);
			idx = 0;
			return CancelListen;
		}
	}
	else if(idx == dFrameOffset_CrcHeader)
		Comm_RxMessage.crcHeader = proc_byte;
	else if(idx == dFrameOffset_CrcHeader + 1)
	{

#ifdef COMM_APP_TEST_MODE

#else
	Comm_RxMessage.crcHeader |= proc_byte << 8;
	uint16_t crcHeader = CalCrc16(0, &Comm_RxMessage.length, sizeof(Comm_RxMessage.length));
	crcHeader = CalCrc16(crcHeader, &Comm_RxMessage.addrSensor, sizeof(Comm_RxMessage.addrSensor));
	crcHeader = CalCrc16(crcHeader, &Comm_RxMessage.command, sizeof(Comm_RxMessage.command));
	crcHeader = CalCrc16(crcHeader, &Comm_RxMessage.flags, sizeof(Comm_RxMessage.flags));
	if(crcHeader != Comm_RxMessage.crcHeader)
	{
		idx = 0;
		PRINTF_COMM("HEADER CRC 0x%04X 0x%04X\n",crcHeader,Comm_RxMessage.crcHeader);
		return FrameError;
	}
#endif

	}
	else //data
	{
		if(idx < dFrameOffset_Data)
		{
			idx = 0;
			PRINTF_COMM("Data \n");
			return FrameError;
		}
		if(idx < (Comm_RxMessage.length-dFrameSize_Length-dFrameSize_AddrSensor-dFrameSize_Command-dFrameSize_Flags-dFrameSize_CrcHeader-dFrameSize_Crc) + dFrameOffset_Data)
		{
			Comm_RxMessage.data[idx - dFrameOffset_Data] = proc_byte;
		}
		else
		{
			if(idx == Comm_RxMessage.length-dFrameSize_Crc + dFrameSize_Preamble)
			{
				Comm_RxMessage.crc = proc_byte;
			}
			else if(idx == Comm_RxMessage.length-dFrameSize_Crc + dFrameSize_Preamble + 1)
			{
				Comm_RxMessage.crc += proc_byte << 8;
				idx = 0;

				if(Comm_RxMessage.crc == CommLink_CalcFrameCrc(&Comm_RxMessage)){
#ifdef SENSOR_TYPE_LF
LED_green_Toggle();
#endif
return FrameReady;

				}
				else
				{
					PRINTF_COMM("DATA CRC \n");
#ifdef COMM_APP_TEST_MODE
					PRINTF_COMM("DATA CRC NIE SPRAWDZANE\n");
					return FrameReady;
#else
					PRINTF_COMM("DATA CRC ERROR\n");
					PRINTF_COMM("CORRECT CRC: %X \n", CommLink_CalcFrameCrc(&Comm_RxMessage));
					return FrameBadCrc;
#endif
				}

			}
		}
	}

	idx++;
	return FrameNotReady;
}

/******************************************************************************/
static void CommLink_TransmitOn(void)
{
	if(_phyTransmitOn_func)
		_phyTransmitOn_func();
}

/******************************************************************************/
static void CommLink_TransmitOff(void)
{
	if(_phyTransmitOff_func)
		_phyTransmitOff_func();
}

/******************************************************************************/
void CommLink_Init(void)
{
	_phyInit_func = RS485_Init;
	_phySend_func = RS485_Send;
	_phyGetChar_func = RS485_GetByte;
	_phyTransmitOn_func = RS485_TransmitOn;
	_phyTransmitOff_func = RS485_TransmitOff;

	OTC_RegisterResponseHandler(CommLink_SendData);
}

/******************************************************************************/
bool CommLink_SendData(uint8_t addrSensor, uint8_t command, uint8_t flags, const void * const data, uint16_t len)
{
	if(data == 0 && len != 0)
		return false;
	if(_phySend_func == 0)
		return false;
	uint16_t ret, crcHeader;
	uint16_t length = len + dFrameSize_Length + dFrameSize_AddrSensor + dFrameSize_Command + dFrameSize_Flags + dFrameSize_CrcHeader + dFrameSize_Crc;
	uint16_t crc = CalCrc16(0, &length, sizeof(length));
	crc = CalCrc16(crc, &addrSensor, sizeof(addrSensor));
	crc = CalCrc16(crc, &command, sizeof(command));
	crc = CalCrc16(crc, &flags, sizeof(flags));
	crcHeader = crc;
	crc = CalCrc16(crc, &crcHeader, sizeof(crcHeader));
	crc = CalCrc16(crc, data, len);
	uint8_t header[11];
	memcpy(header, &_preamble, 4);
	memcpy(header+4, &length, 2);
	header[6] = addrSensor;
	header[7] = command;
	header[8] = flags;
	header[9] = crcHeader & 0x0ff;
	header[10] = (crcHeader>>8) & 0x0ff;
	CommLink_TransmitOn();
	ret =  _phySend_func(&header, sizeof(header));
	ret += _phySend_func(data, len);
	ret += _phySend_func(&crc, dFrameSize_Crc);
	CommLink_TransmitOff();
	if(ret == dFrameSize_Preamble + dFrameSize_Length + dFrameSize_AddrSensor + dFrameSize_Command + dFrameSize_Flags + dFrameSize_CrcHeader + len + dFrameSize_Crc)
		return true;
	else
		return false;
}

/******************************************************************************/
bool CommLink_SendDataFromRingBuff(uint8_t addrSensor, uint8_t command, uint8_t flags, bool(*func)(uint8_t *byte), uint16_t lenData)
{
	uint8_t header[12];
	memcpy(header, &_preamble, 4);

	return true;
}


/******************************************************************************/
void CommLink_SendByte(const uint8_t *byte)
{
	if(_phySendChar_func)
		_phySendChar_func(byte);
}

/******************************************************************************/
Comm_FrameError_t CommLink_Process(void)
{
	uint8_t byte;
	Comm_FrameError_t status = FrameNotReady;

	if(_phyGetChar_func == 0)
		return FrameNotReady;
	while(_phyGetChar_func(&byte))
	{
		status = CommLink_ProcessByte(byte);

		if(FrameReady == status)
		{
			PRINTF_COMM("Frame Ready\r\n");
		}
		else if(FrameError == status)
		{
			PRINTF_COMM("Frame Error\r\n");
		}
		else if(FrameBadCrc == status)
		{
			PRINTF_COMM("Frame Bad crc\r\n");
		}
		else if (status == CancelListen)
		{
			RS485_Clear();
		}
	}
	return status;
}

/******************************************************************************/
void *CommLink_GetPrtDataFrame(void)
{
	return Comm_RxMessage.data;
}

/******************************************************************************/
uint16_t CommLink_GetDataSize(void)
{
	return (Comm_RxMessage.length - dFrameSize_Length - dFrameSize_AddrSensor - dFrameSize_Command - dFrameSize_Flags - dFrameSize_CrcHeader - dFrameSize_Crc);
}

/******************************************************************************/
uint8_t CommLink_GetAddr(void)
{
	return Comm_RxMessage.addrSensor;
}

/******************************************************************************/
uint8_t CommLink_GetCommand(void)
{
	return Comm_RxMessage.command;
}

/******************************************************************************/
uint8_t CommLink_GetFlags(void)
{
	return Comm_RxMessage.flags;
}



