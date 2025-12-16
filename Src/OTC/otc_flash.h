/*
 * otc_flash.h
 *
 *  Created on: 13 sie 2020
 *      Author: mantoniak
 */

#ifndef OTC_OTC_FLASH_H_
#define OTC_OTC_FLASH_H_

#include <stdint.h>

void OTC_Erese_Internal(void);
bool OTC_Write(uint32_t address, uint8_t data);
bool OTC_Read(uint32_t address, uint8_t* data, uint16_t length);

#endif /* OTC_OTC_FLASH_H_ */
