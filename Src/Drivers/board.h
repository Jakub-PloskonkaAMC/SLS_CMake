/*
 * board.h
 *
 *  Created on: 26.06.2019
 *      Author: mantoniak
 */

#ifndef DRIVERS_BOARD_H_
#define DRIVERS_BOARD_H_

#include "conf.h"
#include "memory.h"

typedef struct
{
	const char Type[12] ;
	const uint32_t DataBufferSize;
	const uint16_t SampleRate;
	const uint8_t BitResolution;
	const uint8_t Vcoding;
	const uint8_t ScalarCount;
	const uint8_t AxisCount;
	//*********************************************
	const uint32_t X_Calibration_A;
	const uint32_t X_Calibration_B;
	const uint32_t X_Calibration_Gain ;
	const uint32_t X_Calibration_Range;
	const uint32_t X_Calibration_Sensitivity ;
	const uint32_t Y_Calibration_A;
	const uint32_t Y_Calibration_B;
	const uint32_t Y_Calibration_Gain ;
	const uint32_t Y_Calibration_Range;
	const uint32_t Y_Calibration_Sensitivity ;
	const uint32_t Z_Calibration_A;
	const uint32_t Z_Calibration_B;
	const uint32_t Z_Calibration_Gain ;
	const uint32_t Z_Calibration_Range;
	const uint32_t Z_Calibration_Sensitivity ;
	//*********************************************

}ADI_Parameters_t;

typedef struct
{
   bool sensoraddress_exist;
   bool addressing_by_MAC;
   bool not_broadcast_mode;
   bool aqusition;
   bool overflow;
   bool infinite_aqusition;
   uint32_t number_of_samples;
   uint32_t sample_counter;
   uint8_t command;
   const ADI_Parameters_t parameters;
   AdiConf_t configuration;
}ADI_State_t;

extern ADI_State_t Sensor;

void SystemClock_Config(void);
uint32_t Get_Speed(ADI_rs485_speed spd);
void LED_green_Toggle(void);
void LED_green_On(void);
void LED_green_Off(void);
void LED_red_Toggle(void);
void LED_red_On(void);
void LED_red_Off(void);


#endif /* DRIVERS_BOARD_H_ */
