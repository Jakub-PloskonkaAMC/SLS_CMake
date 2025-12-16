/*
 * ModbusRTU_crc.h
 *
 *  Created on: 5 sie 2022
 *      Author: midec
 */

#ifndef MODBUS_MODBUSRTU_CRC_H_
#define MODBUS_MODBUSRTU_CRC_H_

#include <stdint.h>

void ModbusRTUCRC(uint8_t *nTemp, uint16_t *wCRCWord, uint8_t nData);

#endif /* MODBUS_MODBUSRTU_CRC_H_ */
