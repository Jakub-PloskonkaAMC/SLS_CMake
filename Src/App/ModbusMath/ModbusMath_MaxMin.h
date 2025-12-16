/*
 * ModbusMath_MaxMin.h
 *
 *  Created on: 5 sie 2022
 *      Author: midec
 */

#ifndef MODBUSMATH_MODBUSMATH_MAXMIN_H_
#define MODBUSMATH_MODBUSMATH_MAXMIN_H_

void ModbusMath_MaxMin_Start();
void ModbusMath_MaxMin_Stop();
bool ModbusMath_MaxMin_EnableRead();
uint32_t ModbusMath_MaxMin_MaxValueRead ();
uint32_t ModbusMath_MaxMin_MinValueRead ();
void ModbusMath_MaxMin_Calculate(uint8_t* bufferData, uint32_t length);

#endif /* MODBUSMATH_MODBUSMATH_MAXMIN_H_ */
