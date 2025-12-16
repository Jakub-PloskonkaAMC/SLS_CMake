/*
 * ModbusMath.h
 *
 *  Created on: 5 sie 2022
 *      Author: midec
 */

#ifndef APP_MODBUSMATH_MODBUSMATH_H_
#define APP_MODBUSMATH_MODBUSMATH_H_

void ModbusMath_main();

void ModbusMath_ACQStart_Write(bool state);
bool ModbusMath_ACQStart_Read();


typedef struct __attribute__((__packed__))
{
   uint32_t MaxMin_MaxValue;
   uint32_t MaxMin_MinValue;


}ModbusMathOutput_t;



#endif /* APP_MODBUSMATH_MODBUSMATH_H_ */
