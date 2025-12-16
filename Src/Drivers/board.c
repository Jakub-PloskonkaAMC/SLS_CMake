/*
 * board.c
 *
 *  Created on: 26.06.2019
 *      Author: mantoniak
 */

#include "board.h"
#include "MacAddress.h"
#include "debug.h"
#include "conf.h"

#define EXTRNAL_OSC

ADI_State_t Sensor;

#ifdef  SENSOR_TYPE_LF

ADI_State_t Sensor = {

		.parameters.Type = {'A','M','C','T','e','c','h',' '},
		.parameters.DataBufferSize = 32100,
		.parameters.SampleRate = 4000,
		.parameters.BitResolution = 20,
		.parameters.Vcoding = 0x01,
		.parameters.ScalarCount = SCALARS_AMMOUNT,
		.parameters.AxisCount = 3,

	//*********************************************
	//Pon�ej warto�ci FLOAT zapisane hexami
		.parameters.X_Calibration_A = 0x3f800000,
		.parameters.X_Calibration_B = 0x00,
		.parameters.X_Calibration_Gain = 0x3f800000,
		.parameters.X_Calibration_Range =0x3f800000,
		.parameters.X_Calibration_Sensitivity = 0x47480000,

		.parameters.Y_Calibration_A = 0x3f800000,
		.parameters.Y_Calibration_B = 0,
		.parameters.Y_Calibration_Gain =0x3f800000,
		.parameters.Y_Calibration_Range =0x3f800000,
		.parameters.Y_Calibration_Sensitivity = 0x47480000,

		.parameters.Z_Calibration_A = 0x3f800000,
		.parameters.Z_Calibration_B = 0,
		.parameters.Z_Calibration_Gain = 0x3f800000,
		.parameters.Z_Calibration_Range = 0x3f800000,
		.parameters.Z_Calibration_Sensitivity = 0x47480000,
	//Powy�ej warto�ci FLOAT zapisane hexami
	//*********************************************


};
#elif defined SENSOR_TYPE_HF
ADI_State_t Sensor = {

		.parameters.Type = {'A','M','C','T','e','c','h',' '},
		.parameters.DataBufferSize = 32100,
#ifdef	DOWNSAMPLING
		.parameters.SampleRate = 20000,
#else
		.parameters.SampleRate = 32000,
#endif

		.parameters.BitResolution = 23,
		.parameters.Vcoding = 0x00,
		.parameters.ScalarCount = SCALARS_AMMOUNT,
		.parameters.AxisCount = 1,

	//*********************************************
	//Pon�ej warto�ci FLOAT zapisane hexami
		.parameters.X_Calibration_A = 0x3f800000,
		.parameters.X_Calibration_B = 0,
		.parameters.X_Calibration_Gain = 0x3f800000,
		.parameters.X_Calibration_Range =0x451C4000,
		.parameters.X_Calibration_Sensitivity = 0x3b81df88,



	//Powy�ej warto�ci FLOAT zapisane hexami
	//*********************************************


};

#else

#error  Zdefinuij czujnik

#endif





void Board_Init(void)
{
	Sensor.configuration.sensor_ID = 1;
	for (int var = 0; 			var < MAC_ADDRESS_NUMBER_OF_BYTES;			++var)
	{
		Sensor.configuration.MAC[var] = 				GetMACByte(MAC_ADDRESS_NUMBER_OF_BYTES-1-var);
	}
	Sensor.configuration.RS485_speed = speed_115k;



}

uint32_t Get_Speed(ADI_rs485_speed spd)
{

	uint32_t speed;

	switch (spd)
	{
	case speed_4800:
		speed=4800;
		return speed;
	case speed_7200:
		speed=7200;
		return speed;
	case speed_9600:
		speed=9600;
		return speed;
	case speed_14400:
		speed=14400;
		return speed;
	case speed_19200:
		speed=19200;
		return speed;
	case speed_28800:
		speed=28800;
		return speed;
	case speed_33600:
		speed=33600;
		return speed;
	case speed_38400:
		speed=38400;
		return speed;
	case speed_57600:
		speed=57600;
		return speed;
	case speed_115k:
		speed=115200;
		return speed;
	case speed_500k:
		speed=500000;
		return speed;
	case speed_1500k:
		speed=1500000;
		return speed;
	case speed_1000k:
		speed=1000000;
		return speed;
	default:
		speed=1500000;
		return speed;
	}
}

void Error_Handler(void)
{
	while(1)
	{
		HAL_Delay(100);
		LED_red_Toggle();
	}
}

#ifdef EXTRNAL_OSC
void SystemClock_Config(void)
{
	  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	  /** Configure the main internal regulator output voltage
	  */
	  __HAL_RCC_PWR_CLK_ENABLE();
	  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
//	  for (int i = 0; i < 1000000; i++)
//	  {
//		  __asm("nop");
//	  }
	  /** Initializes the CPU, AHB and APB busses clocks
	  */
	  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	  RCC_OscInitStruct.PLL.PLLM = 6;
#ifdef OLD_MCU_SPEED
	  RCC_OscInitStruct.PLL.PLLN = 80;
#else
	  RCC_OscInitStruct.PLL.PLLN = 100;
#endif
	  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	  RCC_OscInitStruct.PLL.PLLQ = 2;
	  RCC_OscInitStruct.PLL.PLLR = 2;
	  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  /** Initializes the CPU, AHB and APB busses clocks
	  */
	  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
	  {
	    Error_Handler();
	  }

}
#else
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}
#endif
