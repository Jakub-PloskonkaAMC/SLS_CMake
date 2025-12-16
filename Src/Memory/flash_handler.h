/*
 * flash_handler.h
 *
 *  Created on: 01.07.2019
 *      Author: mantoniak
 */

#ifndef MEMORY_FLASH_HANDLER_H_
#define MEMORY_FLASH_HANDLER_H_

#include "conf.h"

#define FLASH_MAGIC_NUMBER 0xDEAD666B
#define FLASH_MAGIC_NUMBER_ADDRESS 0x00
#define FLASH_USER_START 0x04

void Flash_Init(void);
void Memory_LL_Clear(void);
bool Memory_LL_Save(uint32_t address, uint8_t* data, uint16_t length);
void Memory_LL_Read(uint32_t address, uint8_t* data, uint16_t length);

#endif /* MEMORY_FLASH_HANDLER_H_ */
