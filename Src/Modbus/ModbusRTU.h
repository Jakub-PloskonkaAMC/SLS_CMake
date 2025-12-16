/*
 * ModbusRTU.h
 *
 *  Created on: Jul 4, 2022
 *      Author: midec
 */

#ifndef MODBUS_MODBUSRTU_H_
#define MODBUS_MODBUSRTU_H_

#include "ModbusRTU_Defines.h"
#include "ModbusRTU_FunctionCallbacks.h"
#include "rs485.h"

void modbusRTU_loop(void);
uint8_t modbusRTU_sendMessage(modbus_message_t *message);
void modbusRTU_init(uint8_t address);
void modbusRTU_getChar(char c);
void modbusRTU_BufferFlush();

#endif /* MODBUS_MODBUSRTU_H_ */