/*
 * gpio.h
 *
 *  Created on: 26.06.2019
 *      Author: mantoniak
 */

#ifndef DRIVERS_GPIO_H_
#define DRIVERS_GPIO_H_

#include "conf.h"

#ifdef SENSOR_TYPE_LF
#include "gpio_lf.h"
#elif defined SENSOR_TYPE_HF
#include "gpio_hf.h"
#endif
#endif /* DRIVERS_GPIO_H_ */
