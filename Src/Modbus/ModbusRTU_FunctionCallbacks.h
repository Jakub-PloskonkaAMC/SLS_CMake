/*
 * ModbusRTU_FunctionCallbacks.h
 *
 *  Created on: Jul 4, 2022
 *      Author: midec
 */

#ifndef MODBUS_MODBUSRTU_FUNCTIONCALLBACKS_H_
#define MODBUS_MODBUSRTU_FUNCTIONCALLBACKS_H_

#include "ModbusRTU_Defines.h"

void __attribute__((weak)) modbusRTUFCallbacks_readCoils(modbus_message_t *mess);
void __attribute__((weak)) modbusRTUFCallbacks_readRegisters(modbus_message_t *mess);
void __attribute__((weak)) modbusRTUFCallbacks_writeRegisters(modbus_message_t *mess);
void __attribute__((weak)) modbusRTUFCallbacks_writeCoil(modbus_message_t *mess);

#endif /* MODBUS_MODBUSRTU_FUNCTIONCALLBACKS_H_ */
