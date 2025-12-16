/*
 * conf.h
 *
 *  Created on: 26.06.2019
 *      Author: mantoniak
 */

#ifndef CONF_H_
#define CONF_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#define MODBUS_DEVELOPMENT

#define DEBUG_ENABLED

#ifdef DEBUG_ENABLED
 #define DEBUG_PRINTF
 #define DEBUG_APP
 #define DEBUG_HW
 #define DEBUG_MEMORY
// #define DEBUG_COMM
// #define DEBUG_MODBUS
// #define DEBUG_TEST
// #define DEBUG_SINK_RS485
#endif

#define SCALARS_AMMOUNT 1

//#define SENSOR_TYPE_HF
#define SENSOR_TYPE_LF

#define OLD_MCU_SPEED

#define SOFTWARE_MAJOR 1
#define SOFTWARE_MINOR 1
#define SOFTWARE_PATCH 0

#define HARDWARE_MAJOR 3
#define HARDWARE_MINOR 2
#define HARDWARE_REVISION 0

#define FILTER_TYPE_FIR_SLOW

#ifndef FILTER_TYPE_FIR_SLOW
#define DOWNSAMPLING
#endif

#if !defined(FILTER_TYPE_FIR_SLOW) && !defined(FILTER_TYPE_FIR) && !defined(FILTER_TYPE_SINC)
#error Zdefniuj ustawienia ADC!
#endif

#define EXTRNAL_OSC

/**********************************************************************/

void Error_Handler(void);

typedef enum
{
	SLS,
	MODBUS
} CurrentProtocol;

extern CurrentProtocol Protocol;

#endif /* CONF_H_ */
