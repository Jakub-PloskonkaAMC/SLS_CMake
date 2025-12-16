/*
 * otc_flash.c
 *
 *  Created on: 13 sie 2020
 *      Author: mantoniak
 */

#include "debug.h"

typedef enum
{
	BUSY,
	OK
}OTC_FlashState_t;

static OTC_FlashState_t _flashState = OK;

#define MAX_BOOT_DATA_SIZE 0x60000

static bool OTC_Flash_Unlock(void)
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

static bool OTC_Flash_Lock(void)
{
	FLASH->CR |= FLASH_CR_LOCK;
    if(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != RESET)
    {
      return true;
    }
    return false;
}

static void OTC_Flash_Error(void)
{
  __DSB();
  SCB->AIRCR  = (uint32_t)((0x5FAUL << SCB_AIRCR_VECTKEY_Pos)|(SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk)|SCB_AIRCR_SYSRESETREQ_Msk);
  __DSB();
  for(;;)
  {
    __NOP();
  }
}

void OTC_Erese_Internal(void)
{
	while(_flashState == BUSY)
	{;}
	_flashState = BUSY;

	if (!OTC_Flash_Unlock())
	{
	     PRINTF_MEM("Flash access error: %s\n", __FUNCTION__);
	     return;
	}
     __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR );
     FLASH_Erase_Sector(FLASH_SECTOR_8, VOLTAGE_RANGE_3);
     FLASH_Erase_Sector(FLASH_SECTOR_9, VOLTAGE_RANGE_3);
     FLASH_Erase_Sector(FLASH_SECTOR_10, VOLTAGE_RANGE_3);
     if (!OTC_Flash_Lock())
     {
         PRINTF_MEM("Flash could not been secured, system reset: %s\n", __FUNCTION__);
         OTC_Flash_Error();
     }
     PRINTF_MEM("Bootloader data has been erased\n");

 	_flashState = OK;
}

static void OTC_ReadByte(uint32_t address, uint8_t* data) {
    address = address + 0x08080000;
    *data = *(__IO uint8_t*)address;
    //PRINTF_MEM("Readed byte: 0x%02X from address: 0x%08X\n",*data,address);
}

bool OTC_Read(uint32_t address, uint8_t* data, uint16_t length)
{
	while(_flashState == BUSY)
	{;}
	_flashState = BUSY;

	if (address >= MAX_BOOT_DATA_SIZE)
	{
		PRINTF_MEM("Address: 0x%08X\n",address);
		PRINTF_MEM("Read address overflows user flash size: %s\n", __FUNCTION__);
		return false;
	}
	if (length + address >= MAX_BOOT_DATA_SIZE)
	{
		PRINTF_MEM("Data overflows bootloader data size: %s\n", __FUNCTION__);
		return false;
	}
	for(int i = 0; i < length; i++)
	{
		OTC_ReadByte(address+i,data+i);
	}

 	_flashState = OK;
 	return true;
}

bool OTC_Write(uint32_t address, uint8_t data)
{
	while(_flashState == BUSY)
	{;}
	_flashState = BUSY;

	if (address >= MAX_BOOT_DATA_SIZE)
	{
		PRINTF_MEM("Address: 0x%08X\n",address);
		PRINTF_MEM("Write address overflows user flash size: %s\n", __FUNCTION__);
		return false;
	}
	if (!OTC_Flash_Unlock())
	{
	     PRINTF_MEM("Flash access error: %s\n", __FUNCTION__);
	     return false;
	}

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR );
    HAL_FLASH_Program(TYPEPROGRAM_FASTBYTE, 0x08080000 + address, data);

    //PRINTF_MEM("Saved byte 0x%02X to address: 0x%08X\n",data,0x080E0000 + address);

    if (!OTC_Flash_Lock())
    {
        PRINTF_MEM("Flash could not been secured, system reset: %s\n", __FUNCTION__);
        OTC_Flash_Error();
    }

 	_flashState = OK;
 	return true;
}
