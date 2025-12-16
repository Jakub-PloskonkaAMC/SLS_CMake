/*
 * flash_handler.c
 *
 *  Created on: 01.07.2019
 *      Author: mantoniak
 */

#include "flash_handler.h"
#include "debug.h"

#define MAX_USER_FLASH_SIZE 0x10000

__attribute__((__section__(".USDATA"))) const char userFlashData[0x8000];

static bool Flash_Unlock(void)
{
	  if(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != RESET)
	  {
	    WRITE_REG(FLASH->KEYR, FLASH_KEY1);
	    WRITE_REG(FLASH->KEYR, FLASH_KEY2);

	    if(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != RESET)
	    {
	      return false;
	    }
	  }
	  return true;
}

static bool Flash_Lock(void)
{
	FLASH->CR |= FLASH_CR_LOCK;
    if(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != RESET)
    {
      return true;
    }
    return false;
}

static void Flash_Error(void)
{
  __DSB();
  SCB->AIRCR  = (uint32_t)((0x5FAUL << SCB_AIRCR_VECTKEY_Pos)|(SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk)|SCB_AIRCR_SYSRESETREQ_Msk);
  __DSB();
  for(;;)
  {
    __NOP();
  }
}

typedef enum
{
	BUSY,
	OK
}FlashState_t;

static FlashState_t _flashState;

static void Flash_Erese_Internal(void)
{
	while(_flashState == BUSY)
	{;}
	_flashState = BUSY;

	if (!Flash_Unlock())
	{
	     PRINTF_MEM("Flash access error: %s\n", __FUNCTION__);
	     return;
	}
     __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR );
     FLASH_Erase_Sector(FLASH_SECTOR_11, VOLTAGE_RANGE_3);
     if (!Flash_Lock())
     {
         PRINTF_MEM("Flash could not been secured, system reset: %s\n", __FUNCTION__);
         Flash_Error();
     }
     PRINTF_MEM("User flash has been erased\n");

 	_flashState = OK;
}

static bool Flash_Write(uint32_t address, uint8_t data)
{
	while(_flashState == BUSY)
	{;}
	_flashState = BUSY;

	if (address >= MAX_USER_FLASH_SIZE)
	{
		PRINTF_MEM("Address: 0x%08X\n",address);
		PRINTF_MEM("Write address overflows user flash size: %s\n", __FUNCTION__);
		return false;
	}
	if (!Flash_Unlock())
	{
	     PRINTF_MEM("Flash access error: %s\n", __FUNCTION__);
	     return false;
	}

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR );
    HAL_FLASH_Program(TYPEPROGRAM_FASTBYTE, 0x080E0000 + address, data);

    //PRINTF_MEM("Saved byte 0x%02X to address: 0x%08X\n",data,0x080E0000 + address);

    if (!Flash_Lock())
    {
        PRINTF_MEM("Flash could not been secured, system reset: %s\n", __FUNCTION__);
        Flash_Error();
    }

 	_flashState = OK;
 	return true;
}

static void Flash_ReadByte(uint32_t address, uint8_t* data) {
    address = address + 0x080E0000;
    *data = *(__IO uint8_t*)address;
    //PRINTF_MEM("Readed byte: 0x%02X from address: 0x%08X\n",*data,address);
}

static bool Flash_Read(uint32_t address, uint8_t* data, uint16_t length)
{
	while(_flashState == BUSY)
	{;}
	_flashState = BUSY;

	if (address >= MAX_USER_FLASH_SIZE)
	{
		PRINTF_MEM("Address: 0x%08X\n",address);
		PRINTF_MEM("Read address overflows user flash size: %s\n", __FUNCTION__);
		return false;
	}
	if (length + address >= MAX_USER_FLASH_SIZE)
	{
		PRINTF_MEM("Data overflows user flash size: %s\n", __FUNCTION__);
		return false;
	}
	for(int i = 0; i < length; i++)
	{
		Flash_ReadByte(address+i,data+i);
	}

 	_flashState = OK;
 	return true;
}

void Flash_Init(void)
{
 	_flashState = OK;
 	uint8_t readedData[4];
 	Memory_LL_Read(FLASH_MAGIC_NUMBER_ADDRESS,readedData,4);
 	uint32_t readMagicNumber;
 	memcpy(&readMagicNumber,readedData,4);
 	while (readMagicNumber != FLASH_MAGIC_NUMBER)
 	{
 		PRINTF_MEM("Flash memory not initialized\n");
 		Flash_Erese_Internal();
 		uint32_t numberToSave = (uint32_t)FLASH_MAGIC_NUMBER;
 		uint8_t dataToSave[4];
 		memcpy(dataToSave,&numberToSave,4);

 		Memory_LL_Save(FLASH_MAGIC_NUMBER_ADDRESS,dataToSave,4);
 	 	Memory_LL_Read(FLASH_MAGIC_NUMBER_ADDRESS,readedData,4);
 	 	memcpy(&readMagicNumber,readedData,4);
 	 	HAL_Delay(1000);
 	 	PRINTF_MEM("Initializing\n");

 	}
 	PRINTF_MEM("Flash init done\n");
}

void Memory_LL_Clear(void)
{
	PRINTF_MEM("Flash clearing\n");
	Flash_Erese_Internal();
	uint8_t readedData[4];
	Memory_LL_Read(FLASH_MAGIC_NUMBER_ADDRESS, readedData, 4);
	uint32_t readMagicNumber;
	memcpy(&readMagicNumber, readedData, 4);
	while (readMagicNumber != FLASH_MAGIC_NUMBER)
	{
		Flash_Erese_Internal();
		uint32_t numberToSave = (uint32_t) FLASH_MAGIC_NUMBER;
		uint8_t dataToSave[4];
		memcpy(dataToSave, &numberToSave, 4);
		Memory_LL_Save(FLASH_MAGIC_NUMBER_ADDRESS, dataToSave, 4);
		Memory_LL_Read(FLASH_MAGIC_NUMBER_ADDRESS, readedData, 4);
		memcpy(&readMagicNumber, readedData, 4);
	}
	PRINTF_MEM("Flash clear done\n");
}

bool Memory_LL_Save(uint32_t address, uint8_t* data, uint16_t length)
{
	//PRINTF_MEM("%s address: 0x%08X\n",__FUNCTION__,address);
	bool response = true;
	for (int i = 0; i < length; i++)
	{
		response = Flash_Write(address+i,data[i]);
	}
	return response;
}

void Memory_LL_Read(uint32_t address, uint8_t* data, uint16_t length)
{
	//PRINTF_MEM("%s address: 0x%08X\n",__FUNCTION__,address);
	Flash_Read(address,data,length);
}
