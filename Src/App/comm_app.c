/*! ----------------------------------------------------------------------------
 * Include
 ------------------------------------------------------------------------------*/
#include "comm_app.h"
#include "comm_link.h"
#include "acquisition.h"
#include "debug.h"
#include "board.h"
#include "rs485.h"
#include "temp.h"
#include "otc_core.h"
#include "ModbusRTU.h"
#include "MacAddress.h"
#include <stdio.h>
#include <math.h>

/*! ----------------------------------------------------------------------------
 * Define
 ------------------------------------------------------------------------------*/
#define dDATA_BUFF_LEN 1024
#define dDATA_MAX_BUFF_LEN 32

enum flags_pos
{
	ACK_Pos = 0,
	NACK_Pos = 1,
	acquisition_Pos = 2
};

typedef void (*handler_func)(uint8_t cmd);

typedef struct
{
	enum command_type cmd;
	handler_func func;
} commmandHandler_t;

#define dSetFlag_ACK(f) (f |= (1 << ACK_Pos))
#define dSetFlag_NACK(f) (f |= (1 << NACK_Pos))
#define dSetFlag_Acquisition(f) (f |= (1 << acquisition_Pos))
/*! ----------------------------------------------------------------------------
 * Functions prototype
 ------------------------------------------------------------------------------*/
static void handler_starAcq(uint8_t cmd);
static void handler_stopAcq(uint8_t cmd);
static void handler_getData(uint8_t cmd);
static void handler_getAddrMac(uint8_t cmd);
static void handler_getStatus(uint8_t cmd);
static void handler_setID(uint8_t cmd);
static void handler_getConfig(uint8_t cmd);
static void handler_setConfig(uint8_t cmd);
static void handler_setSpeed(uint8_t cmd);
static void handler_reset(uint8_t cmd);
static void handler_calibration(uint8_t cmd);
static void ota_body(uint8_t cmd);

/*! ----------------------------------------------------------------------------
 * Variable
------------------------------------------------------------------------------*/
static commmandHandler_t _handlers[] = {
	{.cmd = starAcq, .func = handler_starAcq},
	{.cmd = stopAcq, .func = handler_stopAcq},
	{.cmd = getData, .func = handler_getData},
	{.cmd = getAddrMac, .func = handler_getAddrMac},
	//{.cmd = getTemp,		.func = handler_getTemp},
	{.cmd = getStatus, .func = handler_getStatus},
	{.cmd = setID, .func = handler_setID},
	{.cmd = getConfig, .func = handler_getConfig},
	{.cmd = setConfig, .func = handler_setConfig},
	{.cmd = setSpeed, .func = handler_setSpeed},
	{.cmd = reset, .func = handler_reset},
	{.cmd = calibration, .func = handler_calibration},
	{.cmd = ota, .func = ota_body},
	{.cmd = swap, .func = swap_t}};

#ifdef SENSOR_TYPE_LF
#define MAX_SAMPLES_AT_ONCE 1024
#elif defined SENSOR_TYPE_HF
#define MAX_SAMPLES_AT_ONCE 5120
#endif

static bool dataSendingScheduled;
static uint32_t dataToSendScheduled;

extern modbus_coil_table_entry_t modbus_coils_table[];

/*! ----------------------------------------------------------------------------
 * Functions
 ------------------------------------------------------------------------------*/

void CommApp_Init(void)
{
	dataSendingScheduled = false;
	dataToSendScheduled = 0;
}

static void handler_starAcq(uint8_t cmd)
{
	if (Sensor.addressing_by_MAC)
	{
		PRINTF_APP("No response - addressing by MAC\n");
	}
	else
	{
		PRINTF_APP("Addressing by SensorID\n");
		uint32_t millis = 0;
		memcpy(&millis, CommLink_GetPrtDataFrame(), 4);
		if (millis == 0)
		{
			PRINTF_APP("Infinite Acquisition\n");
		}

		const float millisInFullSampleRate = 1000;
		float scalledMilis = millis / millisInFullSampleRate;
		uint32_t counter_number = floor((float)Sensor.parameters.SampleRate * scalledMilis);
		PRINTF_APP("Start acquisition %d samples\r\n", counter_number);
		ACQ_Start();

#ifdef SENSOR_TYPE_LF
		Sensor.number_of_samples = 3 * counter_number;
#elif defined SENSOR_TYPE_HF
		Sensor.number_of_samples = 1 * counter_number;
#endif

		if (Sensor.not_broadcast_mode)
		{
			uint8_t flags = 0;
			dSetFlag_ACK(flags);
			CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, 0, 0);
		}
		else
		{
			PRINTF_APP("No response - broadcast mode\n");
		}
		PRINTF_APP("Gathering %d SamplesXYZ, %d single samples\r\n", counter_number, Sensor.number_of_samples);
	}
}

/******************************************************************************/
static void handler_stopAcq(uint8_t cmd)
{
	if (Sensor.addressing_by_MAC)
	{
		PRINTF_APP("No response - addressing by MAC\n");
	}
	else
	{
		PRINTF_APP("Addressing by SensorID\n");
		PRINTF_APP("Stop acquisition\r\n");
		ACQ_Stop();
		if (Sensor.not_broadcast_mode)
		{
			uint8_t flags = 0;
			dSetFlag_ACK(flags);
			CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, 0, 0);
		}
		else
		{
			PRINTF_APP("No response - broadcast mode");
		}
	}
}
/******************************************************************************/
void ResetScheduledSending(void)
{
	dataSendingScheduled = false;
	dataToSendScheduled = 0;
}
/******************************************************************************/
uint8_t respData[3 * 3 * MAX_SAMPLES_AT_ONCE + sizeof(float) * SCALARS_AMMOUNT];

// void Comm_RetData(*data)
//     {
//     data = respData;
//     }

static void handler_getData(uint8_t cmd)
{
	if (Sensor.addressing_by_MAC) // jesli trwa adresowanie
	{
		PRINTF_TEST("No response - addressing by MAC\n"); // to nie rob nic i daj w spokoju adresowac
	}
	else // jesli nie trwa adresowanie
	{
		PRINTF_TEST("Addressing by SensorID\n");
		uint8_t flags = 0;
		uint16_t respLen;
		if (CommLink_GetDataSize() != 2) // powinno miec 2 znaki
		{
			PRINTF_TEST("%s bad CommLink_GetDataSize - should be 2\n", __FUNCTION__); // jak nie ma 2 znakow to powiedz, ze musi miec
			return;
		}
		uint32_t samplesRequested = 0;							  // wyczysc samplesRequested
		memcpy(&samplesRequested, CommLink_GetPrtDataFrame(), 2); // wsad� do samplesRequested 2 pierwsze znaki z commlink_getdata

		PRINTF_TEST("Samples requested: %d\n", samplesRequested); // pokaz ile probek
		if (samplesRequested > MAX_SAMPLES_AT_ONCE)				 // je�li jest wi�ksze od max
		{
			PRINTF_TEST("Max samples to send at once %d\n", MAX_SAMPLES_AT_ONCE); // to powiedz, �e nie mozna
			return;
		}

		if (samplesRequested == 0) // jesli nie ma zadnych probek
		{
			samplesRequested = MAX_SAMPLES_AT_ONCE; // przypisz samplesRequested wartosc maksymalna
		}
#ifdef SENSOR_TYPE_LF
		if (Sensor.parameters.ScalarCount == 1)
		{
			Temp_SampleGet(respData);
		}
		respLen = ACQ_GetSamples(respData + (sizeof(float) * Sensor.parameters.ScalarCount), 3 * samplesRequested) + sizeof(float) * Sensor.parameters.ScalarCount;
#elif defined SENSOR_TYPE_HF

		respLen = ACQ_GetSamples(respData + (sizeof(float) * Sensor.parameters.ScalarCount), samplesRequested) + sizeof(float) * Sensor.parameters.ScalarCount;

		if (Sensor.parameters.ScalarCount == 1)
		{
			Temp_SampleGet(respData);
		}
		// respLen = ACQ_GetSamples(respData+(sizeof(float)*Sensor.parameters.ScalarCount),samplesRequested) + sizeof(float)*Sensor.parameters.ScalarCount;
#endif
		if (Sensor.overflow)
		{
			respData[respLen] = 1;
		}
		else
		{
			respData[respLen] = 0;
		}

		ACQ_clearOverFlow();

		dSetFlag_ACK(flags);
		CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, respData, respLen + 1);

		PRINTF_COMM(" Get measurement\r\n");
		PRINTF_COMM(" Send %d bytes\r\n", respLen + 1);
	}
}

/******************************************************************************/
static void handler_getAddrMac(uint8_t cmd)
{
	if (Sensor.addressing_by_MAC)
	{
		PRINTF_APP("No response - addressing by MAC\n");
	}
	else
	{
		if (CommLink_GetDataSize() == 0)
		{
			PRINTF_APP("%s CommLink_GetDataSize - ok \n", __FUNCTION__);

			uint8_t delay_tmp = ACQ_getRandom();
			delay_tmp = delay_tmp & 0x3f;

			PRINTF_APP("=====================================\n");
			PRINTF_APP("Number passed to HAL_Delay: %i\n", delay_tmp);
			PRINTF_APP("=====================================\n");

			HAL_Delay(delay_tmp);
			PRINTF_APP("Addressing by SensorID\n");
			PRINTF_APP("Get MAC address\r\n");

			uint8_t flags = 0;
			dSetFlag_ACK(flags);

			char tmp[24];

			AdiConf_t fromMemoryConfig;
			PRINTF_APP("Odczytano konfiguracje\r\n");
			Memory_TakeConfig(&fromMemoryConfig);

			memcpy(tmp, GetMACAddress(), 12);
			memcpy(tmp + 12, fromMemoryConfig.sensor_type, 12);

			CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, tmp, 24);
		}
		else
		{
			PRINTF_APP("%s CommLink_GetDataSize != 0 Not ok \n", __FUNCTION__);
		}
	}
}
/******************************************************************************/
static void handler_getStatus(uint8_t cmd)
{
	if (Sensor.addressing_by_MAC)
	{
		PRINTF_APP("No response - addressing by MAC\n");
	}
	else
	{
		PRINTF_APP("Addressing by SensorID\n");
		PRINTF_APP("Get status\r\n");
		PRINTF_APP("ToDo\r\n");

		uint8_t flags = 0;
		dSetFlag_ACK(flags);
		if (Sensor.aqusition)
		{
			dSetFlag_Acquisition(flags);
		}
		CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, 0, 0);
	}
}
/******************************************************************************/
static void handler_setID(uint8_t cmd)
{
	if (!Sensor.addressing_by_MAC)
	{
		PRINTF_APP("Addressing by SensorID\n");
		PRINTF_APP("Set ID\r\n");
		uint8_t flags = 0;
		if (CommLink_GetDataSize() != 13)
		{
			PRINTF_APP("%s bad CommLink_GetDataSize - should be 13\n", __FUNCTION__);
			return;
		}
		uint8_t *dataPointer = CommLink_GetPrtDataFrame();
		AdiConf_t newConfig;
		Memory_TakeConfig(&newConfig);
		newConfig.sensor_ID = dataPointer[12];
		PRINTF_APP("New config send_id: %d\n", newConfig.sensor_ID);
		bool flash_save_success = false;
		flash_save_success = Memory_SaveConfig(&newConfig);
		if (flash_save_success)
		{
			dSetFlag_ACK(flags);
			CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, 0, 0);
		}
		else
		{
			PRINTF_APP("Flash not writen ... error");
			dSetFlag_NACK(flags);
			CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, 0, 0);
		}
	}
	else
	{
		PRINTF_APP("Addressing by MAC\n");
		PRINTF_APP("Set ID\r\n");
		uint8_t flags = 0;
		if (CommLink_GetDataSize() != 13)
		{
			PRINTF_APP("%s bad CommLink_GetDataSize - should be 13\n", __FUNCTION__);
			return;
		}
		uint8_t *dataPointer = CommLink_GetPrtDataFrame();
		AdiConf_t newConfig;
		Memory_TakeConfig(&newConfig);
		if (memcmp(newConfig.MAC, dataPointer, 12) == 0)
		{

			newConfig.sensor_ID = dataPointer[12];
			PRINTF_APP("New config send_id: %d\n", newConfig.sensor_ID);

			bool flash_save_success = false;
			flash_save_success = Memory_SaveConfig(&newConfig);

			if (flash_save_success)
			{
				dSetFlag_ACK(flags);
				CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, 0, 0);
			}
			else
			{
				PRINTF_APP("Flash not writen ... error");
				dSetFlag_NACK(flags);
				CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, 0, 0);
			}
		}
		else
		{
			PRINTF_APP("Config ignored: BAD MAC\n"); // dSetFlag_NACK(flags);
		}
	}
}

/******************************************************************************/

static void handler_getConfig(uint8_t cmd)
{
	uint8_t *dataPointer = CommLink_GetPrtDataFrame();
	AdiConf_t fromMemoryConfig;
	PRINTF_APP("Odczytano konfiguracje\r\n");
	Memory_TakeConfig(&fromMemoryConfig);

	if (Sensor.addressing_by_MAC)
	{
		PRINTF_APP("Addressing by SensorID\n");
		PRINTF_APP("COMM_APP: Get configuration\r\n");
		if (memcmp(fromMemoryConfig.MAC, dataPointer, 12) == 0)
		{
			PRINTF_APP("Addressing by SensorID\n");
			PRINTF_APP("COMM_APP: Get configuration\r\n");
#ifdef SENSOR_TYPE_LF
			CommConfig_t Conf_Table = {};
			memcpy(Conf_Table.MAC, &Sensor.configuration.MAC, 12);
			memcpy(Conf_Table.sensor_type, &fromMemoryConfig.sensor_type, 12);
			Conf_Table.RS485_speed = Sensor.configuration.RS485_speed;
			Conf_Table.DataBufferSize = Sensor.parameters.DataBufferSize;
			Conf_Table.SampleRate = Sensor.parameters.SampleRate;
			Conf_Table.BitResolution = Sensor.parameters.BitResolution;
			Conf_Table.Vcoding = Sensor.parameters.Vcoding;
			Conf_Table.ScalarCount = Sensor.parameters.ScalarCount;
			Conf_Table.AxisCount = Sensor.parameters.AxisCount;
			Conf_Table.X_Calibration_A = Sensor.parameters.X_Calibration_A;
			Conf_Table.X_Calibration_B = Sensor.parameters.X_Calibration_B;
			Conf_Table.X_Calibration_Gain = Sensor.parameters.X_Calibration_Gain;
			Conf_Table.X_Calibration_Range = Sensor.parameters.X_Calibration_Range;
			Conf_Table.X_Calibration_Sensitivity = Sensor.parameters.X_Calibration_Sensitivity;
			Conf_Table.Y_Calibration_A = Sensor.parameters.Y_Calibration_A;
			Conf_Table.Y_Calibration_B = Sensor.parameters.Y_Calibration_B;
			Conf_Table.Y_Calibration_Gain = Sensor.parameters.Y_Calibration_Gain;
			Conf_Table.Y_Calibration_Range = Sensor.parameters.Y_Calibration_Range;
			Conf_Table.Y_Calibration_Sensitivity = Sensor.parameters.Y_Calibration_Sensitivity;
			Conf_Table.Z_Calibration_A = Sensor.parameters.Z_Calibration_A;
			Conf_Table.Z_Calibration_B = Sensor.parameters.Z_Calibration_B;
			Conf_Table.Z_Calibration_Gain = Sensor.parameters.Z_Calibration_Gain;
			Conf_Table.Z_Calibration_Range = Sensor.parameters.Z_Calibration_Range;
			Conf_Table.Z_Calibration_Sensitivity = Sensor.parameters.Z_Calibration_Sensitivity;

			uint8_t flags = 0;
			dSetFlag_ACK(flags);
			CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, (const void *)&Conf_Table, sizeof(Conf_Table));
			ACQ_Stop();

#elif defined SENSOR_TYPE_HF

			CommConfig_t Conf_Table = {};

			memcpy(Conf_Table.MAC, &Sensor.configuration.MAC, 12);
			memcpy(Conf_Table.sensor_type, &fromMemoryConfig.sensor_type, 12);
			Conf_Table.RS485_speed = Sensor.configuration.RS485_speed;
			Conf_Table.DataBufferSize = Sensor.parameters.DataBufferSize;
			Conf_Table.SampleRate = Sensor.parameters.SampleRate;
			Conf_Table.BitResolution = Sensor.parameters.BitResolution;
			Conf_Table.Vcoding = Sensor.parameters.Vcoding;
			Conf_Table.ScalarCount = Sensor.parameters.ScalarCount;
			Conf_Table.AxisCount = Sensor.parameters.AxisCount;
			Conf_Table.X_Calibration_A = fromMemoryConfig.X_Calibration_A;
			Conf_Table.X_Calibration_B = fromMemoryConfig.X_Calibration_B;
			Conf_Table.X_Calibration_Gain = Sensor.parameters.X_Calibration_Gain;
			Conf_Table.X_Calibration_Range = Sensor.parameters.X_Calibration_Range;
			Conf_Table.X_Calibration_Sensitivity = fromMemoryConfig.X_Calibration_Sensitivity;

			uint8_t flags = 0;
			dSetFlag_ACK(flags);
			CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, (const void *)&Conf_Table, sizeof(Conf_Table));

#endif
		}
		else
		{
			PRINTF_APP("GetConf ignored: BAD MAC\n");
		}
	}
	else
	{
		PRINTF_APP("Addressing by SensorID\n");
		PRINTF_APP("COMM_APP: Get configuration\r\n");

#ifdef SENSOR_TYPE_LF

		uint8_t Conf_Table[95];

		memcpy(Conf_Table, &Sensor.configuration.MAC, 12);
		memcpy(Conf_Table + 12, &fromMemoryConfig.sensor_type, 12);
		memcpy(Conf_Table + 24, &Sensor.configuration.RS485_speed, 1);
		memcpy(Conf_Table + 25, &Sensor.parameters.DataBufferSize, 4);
		memcpy(Conf_Table + 29, &Sensor.parameters.SampleRate, 2);
		memcpy(Conf_Table + 31, &Sensor.parameters.BitResolution, 1);
		memcpy(Conf_Table + 32, &Sensor.parameters.Vcoding, 1);
		memcpy(Conf_Table + 33, &Sensor.parameters.ScalarCount, 1);
		memcpy(Conf_Table + 34, &Sensor.parameters.AxisCount, 1);
		memcpy(Conf_Table + 35, &fromMemoryConfig.X_Calibration_A, 4);
		memcpy(Conf_Table + 39, &fromMemoryConfig.X_Calibration_B, 4);
		memcpy(Conf_Table + 43, &Sensor.parameters.X_Calibration_Gain, 4);
		memcpy(Conf_Table + 47, &Sensor.parameters.X_Calibration_Range, 4);
		memcpy(Conf_Table + 51, &fromMemoryConfig.X_Calibration_Sensitivity, 4);
		memcpy(Conf_Table + 55, &fromMemoryConfig.Y_Calibration_A, 4);
		memcpy(Conf_Table + 59, &fromMemoryConfig.Y_Calibration_B, 4);
		memcpy(Conf_Table + 63, &Sensor.parameters.Y_Calibration_Gain, 4);
		memcpy(Conf_Table + 67, &Sensor.parameters.Y_Calibration_Range, 4);
		memcpy(Conf_Table + 71, &fromMemoryConfig.Y_Calibration_Sensitivity, 4);
		memcpy(Conf_Table + 75, &fromMemoryConfig.Z_Calibration_A, 4);
		memcpy(Conf_Table + 79, &fromMemoryConfig.Z_Calibration_B, 4);
		memcpy(Conf_Table + 83, &Sensor.parameters.Z_Calibration_Gain, 4);
		memcpy(Conf_Table + 87, &Sensor.parameters.Z_Calibration_Range, 4);
		memcpy(Conf_Table + 91, &fromMemoryConfig.Z_Calibration_Sensitivity, 4);

		uint8_t flags = 0;
		dSetFlag_ACK(flags);
		CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, Conf_Table, 95);
		ACQ_Stop();

#elif defined SENSOR_TYPE_HF

		uint8_t Conf_Table[55];

		memcpy(Conf_Table, &Sensor.configuration.MAC, 12);
		memcpy(Conf_Table + 12, &fromMemoryConfig.sensor_type, 12);
		memcpy(Conf_Table + 24, &Sensor.configuration.RS485_speed, 1);
		memcpy(Conf_Table + 25, &Sensor.parameters.DataBufferSize, 4);
		memcpy(Conf_Table + 29, &Sensor.parameters.SampleRate, 2);
		memcpy(Conf_Table + 31, &Sensor.parameters.BitResolution, 1);
		memcpy(Conf_Table + 32, &Sensor.parameters.Vcoding, 1);
		memcpy(Conf_Table + 33, &Sensor.parameters.ScalarCount, 1);
		memcpy(Conf_Table + 34, &Sensor.parameters.AxisCount, 1);
		memcpy(Conf_Table + 35, &fromMemoryConfig.X_Calibration_A, 4);
		memcpy(Conf_Table + 39, &fromMemoryConfig.X_Calibration_B, 4);
		memcpy(Conf_Table + 43, &Sensor.parameters.X_Calibration_Gain, 4);
		memcpy(Conf_Table + 47, &Sensor.parameters.X_Calibration_Range, 4);
		memcpy(Conf_Table + 51, &fromMemoryConfig.X_Calibration_Sensitivity, 4);

		uint8_t flags = 0;
		dSetFlag_ACK(flags);
		CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, Conf_Table, 55);

#endif
	}
}

/******************************************************************************/

static void handler_setConfig(uint8_t cmd)
{
	PRINTF_APP("Set configuration\r\n");

	uint8_t *dataPointer = CommLink_GetPrtDataFrame();
	uint8_t data_size = CommLink_GetDataSize();
	uint8_t flags = 0;
	UNUSED(flags);
	uint8_t sensor;

	AdiConf_t fromMemoryConfig;
	PRINTF_APP("Odczytano konfiguracje\r\n");
	Memory_TakeConfig(&fromMemoryConfig);
	Memory_ShowConfig(&fromMemoryConfig);

	PRINTF_APP("Otrzymane dane w ramce:\r\n");
	for (int i = 0; i < data_size; i++)
	{
		PRINTF_COMM("Bajt %d: %02X \n", i, *(dataPointer + i));
	}

	if (!Sensor.addressing_by_MAC)
	{
		PRINTF_APP("Addressing by SensorID\n");
		PRINTF_APP("Get config!\r\n");
		if (data_size == 60)
		{
			PRINTF_APP("%s Data size ok! for 3 sensor\n", __FUNCTION__);
			sensor = 3;
		}
		else if (data_size == 36)
		{
			PRINTF_APP("%s Data size ok! for 1 sensor\n", __FUNCTION__);
			sensor = 1;
		}
		else
		{
			PRINTF_APP("%s Data size not ok  at all ! \n", __FUNCTION__);
			sensor = 0;
			return;
		}

		//
		//		   //MANUAL NUMERATIN ONLY!!
		fromMemoryConfig.sensor_type[0] = *(dataPointer + 12);
		fromMemoryConfig.sensor_type[1] = *(dataPointer + 13);
		fromMemoryConfig.sensor_type[2] = *(dataPointer + 14);
		fromMemoryConfig.sensor_type[3] = *(dataPointer + 15);
		fromMemoryConfig.sensor_type[4] = *(dataPointer + 16);
		fromMemoryConfig.sensor_type[5] = *(dataPointer + 17);
		fromMemoryConfig.sensor_type[6] = *(dataPointer + 18);
		fromMemoryConfig.sensor_type[7] = *(dataPointer + 19);
		fromMemoryConfig.sensor_type[8] = *(dataPointer + 20);
		fromMemoryConfig.sensor_type[9] = *(dataPointer + 21);
		fromMemoryConfig.sensor_type[10] = *(dataPointer + 22);
		fromMemoryConfig.sensor_type[11] = *(dataPointer + 23);

		if (sensor == 3)
		{
			memcpy(&fromMemoryConfig.X_Calibration_A, dataPointer + 24, 4);
			memcpy(&fromMemoryConfig.X_Calibration_B, dataPointer + 28, 4);
			memcpy(&fromMemoryConfig.X_Calibration_Sensitivity, dataPointer + 32, 4);
			memcpy(&fromMemoryConfig.Y_Calibration_A, dataPointer + 36, 4);
			memcpy(&fromMemoryConfig.Y_Calibration_B, dataPointer + 40, 4);
			memcpy(&fromMemoryConfig.Y_Calibration_Sensitivity, dataPointer + 44, 4);
			memcpy(&fromMemoryConfig.Z_Calibration_A, dataPointer + 48, 4);
			memcpy(&fromMemoryConfig.Z_Calibration_B, dataPointer + 52, 4);
			memcpy(&fromMemoryConfig.Z_Calibration_Sensitivity, dataPointer + 56, 4);
		}

		else if (sensor == 1)
		{
			memcpy(&fromMemoryConfig.X_Calibration_A, dataPointer + 24, 4);
			memcpy(&fromMemoryConfig.X_Calibration_B, dataPointer + 28, 4);
			memcpy(&fromMemoryConfig.X_Calibration_Sensitivity, dataPointer + 32, 4);
		}

		else
		{
			PRINTF_APP("this is not ok! it should be 1 or 3\r\n");
		}

		bool flash_write_success = false;
		uint8_t flags = 0;

		flash_write_success = Memory_SaveConfig(&fromMemoryConfig);
		PRINTF_APP("Odczytano konfiguracje po zapisie\r\n");
		Memory_ShowConfig(&fromMemoryConfig);

		if (flash_write_success == true)
		{
			dSetFlag_ACK(flags);
			CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, 0, 0);
		}
		else
		{
			dSetFlag_NACK(flags);
			CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, 0, 0);
		}
	}
	else
	{
		PRINTF_APP("Addressing by MAC\n");

		if (memcmp(fromMemoryConfig.MAC, dataPointer, 12) == 0)
		{
			PRINTF_APP("Config: MAC ok!\n");
			PRINTF_APP("Otrzymane dane w ramce:\r\n");
			for (int i = 0; i < data_size; i++)
			{
				PRINTF_COMM("Bajt %d: %02X \n", i, *(dataPointer + i));
			}

			PRINTF_APP("Addressing by SensorID\n");
			PRINTF_APP("Get config!\r\n");
			if (data_size == 60)
			{
				PRINTF_APP("%s Data size ok! for 3 sensor\n", __FUNCTION__);
				sensor = 3;
			}
			else if (data_size == 36)
			{
				PRINTF_APP("%s Data size ok! for 1 sensor\n", __FUNCTION__);
				sensor = 1;
			}
			else
			{
				PRINTF_APP("%s Data size not ok  at all ! \n", __FUNCTION__);
				sensor = 0;
				return;
			}

			AdiConf_t fromMemoryConfig;
			PRINTF_APP("Odczytano konfiguracje\r\n");
			Memory_TakeConfig(&fromMemoryConfig);
			Memory_ShowConfig(&fromMemoryConfig);
			//
			//		   //MANUAL NUMERATIN ONLY!!
			fromMemoryConfig.sensor_type[0] = *(dataPointer + 12);
			fromMemoryConfig.sensor_type[1] = *(dataPointer + 13);
			fromMemoryConfig.sensor_type[2] = *(dataPointer + 14);
			fromMemoryConfig.sensor_type[3] = *(dataPointer + 15);
			fromMemoryConfig.sensor_type[4] = *(dataPointer + 16);
			fromMemoryConfig.sensor_type[5] = *(dataPointer + 17);
			fromMemoryConfig.sensor_type[6] = *(dataPointer + 18);
			fromMemoryConfig.sensor_type[7] = *(dataPointer + 19);
			fromMemoryConfig.sensor_type[8] = *(dataPointer + 20);
			fromMemoryConfig.sensor_type[9] = *(dataPointer + 21);
			fromMemoryConfig.sensor_type[10] = *(dataPointer + 22);
			fromMemoryConfig.sensor_type[11] = *(dataPointer + 23);

			if (sensor == 3)
			{
				memcpy(&fromMemoryConfig.X_Calibration_A, dataPointer + 24, 4);
				memcpy(&fromMemoryConfig.X_Calibration_B, dataPointer + 28, 4);
				memcpy(&fromMemoryConfig.X_Calibration_Sensitivity, dataPointer + 32, 4);
				memcpy(&fromMemoryConfig.Y_Calibration_A, dataPointer + 36, 4);
				memcpy(&fromMemoryConfig.Y_Calibration_B, dataPointer + 40, 4);
				memcpy(&fromMemoryConfig.Y_Calibration_Sensitivity, dataPointer + 44, 4);
				memcpy(&fromMemoryConfig.Z_Calibration_A, dataPointer + 48, 4);
				memcpy(&fromMemoryConfig.Z_Calibration_B, dataPointer + 52, 4);
				memcpy(&fromMemoryConfig.Z_Calibration_Sensitivity, dataPointer + 56, 4);
			}

			else if (sensor == 1)
			{
				memcpy(&fromMemoryConfig.X_Calibration_A, dataPointer + 24, 4);
				memcpy(&fromMemoryConfig.X_Calibration_B, dataPointer + 28, 4);
				memcpy(&fromMemoryConfig.X_Calibration_Sensitivity, dataPointer + 32, 4);
			}

			else
			{
				PRINTF_APP("this is not ok! it should be 1 or 3\r\n");
			}

			bool flash_write_success = false;
			uint8_t flags = 0;

			flash_write_success = Memory_SaveConfig(&fromMemoryConfig);
			PRINTF_APP("Odczytano konfiguracje po zapisie\r\n");
			Memory_ShowConfig(&fromMemoryConfig);

			if (flash_write_success == true)
			{
				dSetFlag_ACK(flags);
				CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, 0, 0);
			}
			else
			{
				dSetFlag_NACK(flags);
				CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, 0, 0);
			}
		}
		else
		{
			PRINTF_APP("Config ignored: BAD MAC\n");
		}
	}
}

/******************************************************************************/

static void handler_calibration(uint8_t cmd)
{
	PRINTF_APP("set calibration \r\n");
	PRINTF_APP("ToDo\r\n");
}
/*

 //----------------------------------------------------------------------------------------------//


 int32_t tab_size =201;
 int G1 = 1;
 int G2 = 2;
 int G;
 //int tab_size =24;//test
 int32_t m;
 int CalibrationSamples = 10;
 int sum_max1 = 0;
 int sum_min1 = 0;
 int sum_max2 = 0;
 int sum_min2 = 0;
 float avarage_max1;
 float avarage_min1;
 float avarage1;
 float avarage_max2;
 float avarage_min2;
 float avarage2;
 float A1;
 float A2;
 float a;
 float b;

 int32_t t1[]={-136358,	-1664447,	-1718466,	-1779650,	-1846722,	-1782210,	-1628610,	-1657794,	-1777090,	-1746114,	-1684930,	-1610178,	-1659842,	-1725122,	-1668290,	-1606338,	-1599682,	-1604290,	-1669570,	-1595074,	-1489346,	-1549250,	-1650114,	-1647554,	-1491906,	-1413058,	-1490370,	-1559490,	-1447362,	-1379778,	-1353154,	-1394114,	-1522882,	-1319618,	-1242562,	-1376450,	-1484226,	-1419970,	-1235906,	-1200578,	-1299906,	-1354946,	-1299650,	-1188802,	-1266626,	-1331138,	-1313474,	-1264066,	-1106370,	-1253826,	-1309634,	-1231042,	-1118658,	-1149634,	-1233346,	-1254338,	-1128130,	-1120962,	-1192130,	-1245634,	-1164226,	-1078466,	-1084354,	-1158594,	-1249474,	-1151682,	-1038786,	-1063362,	-1228226,	-1248962,	-1106882,	-1027266,	-1041090,	-1210306,	-1255874,	-1090754,	-1049282,	-1128386,	-1230786,	-1223106,	-1086402,	-1076162,	-1138114,	-1264322,	-1171650,	-1094082,	-1085122,	-1161410,	-1240002,	-1184450,	-1114050,	-1173954,	-1332930,	-1321154,	-1140418,	-1088194,	-1212098,	-1350594,	-1297090,	-1166018,	-1213122,	-1278914,	-1380802,	-1330370,	-1248962,	-1323202,	-1362370,	-1420482,	-1298370,	-1273538,	-1426626,	-1488578,	-1410498,	-1253570,	-1322690,	-1501890,	-1512642,	-1438658,	-1374146,	-1476034,	-1608898,	-1585858,	-1490114,	-1491394,	-1601986,	-1661890,	-1606850,	-1533890,	-1618114,	-1777090,	-1727938,	-1592002,	-1606082,	-1699778,	-1812674,	-1769666,	-1666242,	-1712834,	-1778882,	-1884866,	-1720770,	-1711554,	-1840322,	-1882306,	-1886402,	-1734850,	-1770178,	-1877954,	-1950658,	-1908674,	-1763266,	-1778882,	-1946818,	-2017730,	-1929154,	-1810114,	-1952194,	-2010562,	-2013890,	-1939650,	-1832130,	-1949890,	-2010306,	-2006722,	-1820354,	-1831362,	-1939650,	-2041282,	-2041794,	-1859266,	-1855170,	-2001858,	-2050242,	-2001602,	-1869762,	-1887426,	-2022594	-2037442	-1982146	-1859266	-1953730	-1995970	-1995970,	-1940674,	-1782722,	-1906370,	-1989826,	-1917122,	-1796546,	-1793474,	-1899202,	-1979330,	-1932482,	-1732290,	-1760962,	-1863618,	-1894850,	-1787842,	-1705410,	-1705410,	-1882306,	-1810114,	-1789634,	-1625282,	-1801410,	-1825730,	-1773506,	-1664706};
 int32_t t2[]={-1664432,	-1664447,	-1718466,	-1779650,	-1846722,	-1782267,	-1628610,	-1657711,	-1757090,	-1746114,	-1684930,	-1610178,	-1659842,	-1725122,	-1668290,	-1606338,	-1599682,	-1604290,	-1669570,	-1595074,	-1489346,	-1549250,	-1650114,	-1647554,	-1491906,	-1413058,	-1490370,	-1553590,	-1447362,	-1379778,	-1356754,	-1394114,	-1522882,	-1319618,	-1242562,	-1376450,	-1484226,	-1419970,	-1235906,	-1210578,	-1299906,	-1354946,	-1299650,	-1188802,	-1266626,	-1331138,	-1313474,	-1264066,	-1106370,	-1253826,	-1309634,	-1231042,	-1118658,	-1149634,	-1233346,	-1254338,	-1128130,	-1124562,	-1192130,	-1245634,	-1164226,	-1078466,	-1084354,	-1158594,	-1249474,	-1151682,	-1038786,	-1063362,	-1256226,	-1248962,	-1106882,	-1027266,	-1041090,	-1210306,	-1255874,	-1090754,	-1049282,	-1128386,	-1230776,	-1223106,	-1086402,	-1076162,	-1138114,	-1264322,	-1171650,	-1094082,	-1085122,	-1161410,	-1240002,	-1184450,	-1114050,	-1173954,	-1332930,	-1321154,	-1140418,	-1088194,	-1212098,	-1350594,	-1297090,	-1166018,	-1213122,	-1278914,	-1380802,	-1330370,	-1248962,	-1323202,	-1362370,	-1420482,	-1298370,	-1273538,	-1426626,	-1488578,	-1410498,	-1253570,	-1322690,	-1501890,	-1512642,	-1438658,	-1374146,	-1476034,	-1608898,	-1585858,	-1490114,	-1491394,	-1601986,	-1661890,	-1606850,	-1533890,	-1618114,	-1877090,	-1727938,	-1592002,	-1606082,	-1699778,	-1812674,	-1769666,	-1666242,	-1712834,	-1778882,	-1884866,	-1720770,	-1711554,	-1840322,	-1882306,	-1886402,	-1734850,	-1770178,	-1877954,	-1950658,	-1908674,	-1763266,	-1778882,	-1946818,	-2017730,	-1929154,	-1810114,	-1952194,	-2010562,	-2013890,	-1939650,	-1832130,	-1949890,	-2010306,	-2006722,	-1820354,	-1831362,	-1939650,	-2041282,	-2041794,	-1859266,	-1855170,	-2001858,	-2050242,	-2001602,	-1869762,	-1887426,	-2022594	-2037442	-1982146	-1859266	-1953730	-1995970	-1995970,	-1940674,	-1782722,	-1906370,	-1989826,	-1917122,	-1796546,	-1793474,	-1899202,	-1979330,	-1932482,	-1732290,	-1760962,	-1863618,	-1894850,	-1787842,	-1705410,	-1705410,	-1882306,	-1810114,	-1789634,	-1625282,	-1801410,	-1825730,	-1773506,	-1664706};

 //int t[]={1,1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,21,23,24,25};//test
 int32_t max1[CalibrationSamples];
 int32_t min1[CalibrationSamples];
 int32_t max2[CalibrationSamples];
 int32_t min2[CalibrationSamples];

 for (int j = 0; j < tab_size; j++){ //sortowanie
 for (int i = 0; i < (tab_size - 1); i++){
 if (t1[i]>t1[i+1]){
 m=t1[i];
 t1[i]=t1[i+1];
 t1[i+1]=m;
 }
 }
 }

 PRINTF_APP ( "najmniejsze elementy G1 to: \n");
 for (int i = 0; i < CalibrationSamples; i++){
 min1[i]=t1[i];
 PRINTF_APP ( "%d = %d \n", i, min1[i]);
 sum_min1 += min1[i];
 }

 PRINTF_APP ( "najwieksze elementy G1 to: \n");
 for (int i = (tab_size-1), j=0;j < CalibrationSamples ; i--, j++){
 max1[j]=t1[i];
 PRINTF_APP ( "%d = %d \n", j, max1[j]);
 sum_max1 += max1[j];
 }

 PRINTF_APP ("suma max = %d \n", sum_max1);
 PRINTF_APP ("suma min = %d \n", sum_min1);

 avarage_max1 = (sum_max1/CalibrationSamples);
 avarage_min1 = (sum_min1/CalibrationSamples);

 avarage1 = ((avarage_max1 + avarage_min1)/ 2);

 PRINTF_APP("srednia minimalnych = %f \n", avarage_min1);
 PRINTF_APP("srednia maksymalnych = %f \n", avarage_max1);
 PRINTF_APP("srednia G1= %f \n", avarage1);

 A1 = (avarage_max1 - avarage1);

 //--------------------------------------------------//

 for (int j = 0; j < tab_size; j++){ //sortowanie
 for (int i = 0; i < (tab_size - 1); i++){
 if (t2[i]>t2[i+1]){
 m=t2[i];
 t2[i]=t2[i+1];
 t2[i+1]=m;
 }
 }
 }

 PRINTF_APP ( "najmniejsze elementy to: \n");
 for (int i = 0; i < CalibrationSamples; i++){
 min2[i]=t2[i];
 PRINTF_APP ( "%d = %d \n", i, min2[i]);
 sum_min2 += min2[i];
 }

 PRINTF_APP ( "najwieksze elementy to: \n");
 for (int i = (tab_size-1), j=0;j < CalibrationSamples ; i--, j++){
 max2[j]=t2[i];
 PRINTF_APP ( "%d = %d \n", j, max2[j]);
 sum_max2 += max2[j];
 }

 PRINTF_APP ("suma max = %d \n", sum_max2);
 PRINTF_APP ("suma min = %d \n", sum_min2);

 avarage_max2 = (sum_max2/CalibrationSamples);
 avarage_min2 = (sum_min2/CalibrationSamples);

 avarage2 = ((avarage_max2 + avarage_min2)/ 2);

 PRINTF_APP("srednia minimalnych = %lf \n", avarage_min2);
 PRINTF_APP("srednia maksymalnych = %lf \n", avarage_max2);
 printf("srednia G2 = %f \n", avarage2);

 A2 = (avarage_max2 - avarage2);
 //------------------------------------------------------//


 G = (G1 - G2);
 a = (G/(A1-A2));
 b = (G1 - (a*A1));

 PRINTF_APP("a = %f \n", a);
 PRINTF_APP("b = %f \n", b);
 }
 */

static bool _check_speed(ADI_rs485_speed spd)
{
	switch (spd)
	{
	   case speed_4800:
	   case speed_7200:
	   case speed_9600:
	   case speed_14400:
	   case speed_19200:
	   case speed_28800:
	   case speed_33600:
	   case speed_38400:
	   case speed_57600:
	   case speed_115k:
	   case speed_500k:
	   case speed_1000k:
	   case speed_1500k:
		   return true;
	   default:
		   return false;
	}
}

/******************************************************************************/

static void handler_setSpeed(uint8_t cmd)
{
	if (!Sensor.addressing_by_MAC)
	{
		PRINTF_APP("Addressing by SensorID\n");
		PRINTF_APP("Set speed\r\n");
		if (CommLink_GetDataSize() != 13)
		{
			PRINTF_APP("%s bad CommLink_GetDataSize - should be 2\n", __FUNCTION__);
			return;
		}
		uint8_t *dataPointer = CommLink_GetPrtDataFrame();
		uint8_t flags = 0;

		// REWRITE MAC AND ID
		AdiConf_t newConfig;
		Memory_TakeConfig(&newConfig);
		newConfig.RS485_speed = dataPointer[12];

		// VALIDATE SPEED
		if (_check_speed(newConfig.RS485_speed))
		{
			Memory_SaveConfig(&newConfig);
			PRINTF_APP("New config send_id: %d\n", newConfig.sensor_ID);
			char name[20];
			memset(name, 0, 20);
			switch (newConfig.RS485_speed)
			{
			case speed_4800:
				sprintf(name, "speed_4800");
				break;
			case speed_7200:
				sprintf(name, "speed_7200");
				break;
			case speed_9600:
				sprintf(name, "speed_9600");
				break;
			case speed_14400:
				sprintf(name, "speed_14400");
				break;
			case speed_28800:
				sprintf(name, "speed_28800");
				break;
			case speed_33600:
				sprintf(name, "speed_33600");
				break;
			case speed_38400:
				sprintf(name, "speed_38400");
				break;
			case speed_57600:
				sprintf(name, "speed_57600");
				break;
			case speed_115k:
				sprintf(name, "speed_115k");
				break;
			case speed_500k:
				sprintf(name, "speed_500k");
				break;
			case speed_1000k:
				sprintf(name, "speed_1000k");
				break;
			case speed_1500k:
				sprintf(name, "speed_1500k");
				break;
			default:
				sprintf(name, "bad speed");
				break;
			}
			PRINTF_APP("New config speed: %s\n", name);
			dSetFlag_ACK(flags);
		}
		else
		{
			PRINTF_APP("Config ignored: BAD SPEED\n");
			dSetFlag_NACK(flags);
		}
		CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, 0, 0);
	}
	else
	{

		PRINTF_APP("Addressing by MAC\n");
		PRINTF_APP("Set speed\r\n");
		if (CommLink_GetDataSize() != 13)
		{
			PRINTF_APP("%s bad CommLink_GetDataSize - should be 2\n", __FUNCTION__);
			return;
		}
		uint8_t *dataPointer = CommLink_GetPrtDataFrame();
		uint8_t flags = 0;

		AdiConf_t newConfig;
		Memory_TakeConfig(&newConfig);
		newConfig.RS485_speed = dataPointer[12];

		// COMPARE MAC
		if (memcmp(newConfig.MAC, dataPointer, 12) == 0)
		{
			// VALIDATE SPEED
			if (_check_speed(newConfig.RS485_speed))
			{
				Memory_SaveConfig(&newConfig);
				PRINTF_APP("New config send_id: %d\n", newConfig.sensor_ID);
				char name[20];
				memset(name, 0, 20);
				switch (newConfig.RS485_speed)
				{
				case speed_4800:
					sprintf(name, "speed_4800");
					break;
				case speed_7200:
					sprintf(name, "speed_7200");
					break;
				case speed_9600:
					sprintf(name, "speed_9600");
					break;
				case speed_14400:
					sprintf(name, "speed_14400");
					break;
				case speed_19200:
					sprintf(name, "speed_19200");
					break;
				case speed_28800:
					sprintf(name, "speed_28800");
					break;
				case speed_33600:
					sprintf(name, "speed_33600");
					break;
				case speed_38400:
					sprintf(name, "speed_38400");
					break;
				case speed_57600:
					sprintf(name, "speed_57600");
					break;
				case speed_115k:
					sprintf(name, "speed_115k");
					break;
				case speed_500k:
					sprintf(name, "speed_500k");
					break;
				case speed_1500k:
					sprintf(name, "speed_1500k");
					break;
				case speed_1000k:
					sprintf(name, "speed_1000k");
					break;
				default:
					sprintf(name, "bad speed");
					break;
				}
				PRINTF_APP("New config speed: %s\n", name);
				dSetFlag_ACK(flags);
			}
			else
			{
				PRINTF_APP("Config ignored: BAD SPEED\n");
				dSetFlag_NACK(flags);
			}
			CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, 0, 0);
		}
		else
		{
			PRINTF_APP("Config ignored: BAD MAC\n");
		}
	}
}

/******************************************************************************/

static void handler_reset(uint8_t cmd)
{
	if (!Sensor.addressing_by_MAC)
	{
		PRINTF_APP("Addressing by SensorID\n");
		PRINTF_APP("reset\r\n");
		uint8_t flags = 0;

		if (CommLink_GetDataSize() != 12)
		{
			PRINTF_APP("%s bad CommLink_GetDataSize - should be 12\n", __FUNCTION__);
			PRINTF_APP("ERROR");
			return;
		}

		dSetFlag_ACK(flags);
		CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, 0, 0);
		HAL_Delay(200);
		NVIC_SystemReset();
	}
	else
	{
		PRINTF_APP("Addressing by MAC\n");
		PRINTF("COMM_APP: reset\r\n");
		uint8_t flags = 0;

		if (CommLink_GetDataSize() != 12)
		{
			PRINTF_APP("%s bad CommLink_GetDataSize - should be 2\n", __FUNCTION__);
			return;
		}
		uint8_t *dataPointer = CommLink_GetPrtDataFrame();

		AdiConf_t Config;
		Memory_TakeConfig(&Config);

		if (memcmp(Config.MAC, dataPointer, 12) == 0)
		{
			dSetFlag_ACK(flags);
			CommLink_SendData(Sensor.configuration.sensor_ID, cmd, flags, 0, 0);
			HAL_Delay(200);
			NVIC_SystemReset();
		}
		else
		{
			PRINTF_APP("Config ignored: BAD MAC\n"); // dSetFlag_NACK(flags);
		}
	}
}

/******************************************************************************/

static void Comm_ProcessMessage(void)
{
	Sensor.addressing_by_MAC = false;
	Sensor.not_broadcast_mode = true;
	if ((CommLink_GetCommand() > 0x21 && CommLink_GetCommand() != 0x23) && Sensor.aqusition)
	{
		PRINTF_APP("COMM_APP: Sorry trwa akwizycja, komenda nieobs�ugiwana\r\n");
	}
	else
	{
		uint8_t cmd = CommLink_GetCommand();
		if (CommLink_GetAddr() == 0x00) // Adresowanie po MAC
		{
			if (cmd == getAddrMac && CommLink_GetAddr() == Sensor.configuration.sensor_ID) // Wysy�anie GetMac do ka�dego czujnika z ID=0
			{
				handler_getAddrMac(cmd);
			}
			else // adresowanie po MAC
			{
				Sensor.addressing_by_MAC = true;
				PRINTF_APP("COMM_APP: Adresowanie po MAC\r\n");

				for (uint32_t i = 0; i < sizeof(_handlers) / sizeof(_handlers[0]); ++i)
				{
					if (cmd == _handlers[i].cmd)
					{
						_handlers[i].func(_handlers[i].cmd);
						break;
					}
				}
			}
		}

		else if (CommLink_GetAddr() == 0xff) // Tryb Broadcast
		{
			Sensor.not_broadcast_mode = false;
			if (cmd == starAcq)
			{
				PRINTF_APP("COMM_APP: Polecenie rozg�oszeniowe StartACQ \r\n");
				handler_starAcq(cmd);
			}
			else if (cmd == stopAcq)
			{
				PRINTF_APP("COMM_APP: Polecenie rozg�oszeniowe stopAcq \r\n");
				handler_stopAcq(cmd);
			}
			else
			{
				PRINTF_APP("COMM_APP: Polecenie rozg�oszeniowe niedozwolona komenda w trybie broadcast \r\n");
			}
		}
		else if (CommLink_GetAddr() == Sensor.configuration.sensor_ID) // Adresowanie po ID
		{
			PRINTF_COMM("Dobry adres! \n");

			if ((cmd == reset) || (cmd == setSpeed)) // Reset i setSpeed nieobs�ugiwane po ID
			{
				PRINTF_APP("Komenda nie obs�ugiwana po ID\r\n");
			}
			else
			{
				for (int i = 0; i < sizeof(_handlers) / sizeof(_handlers[0]); ++i)
				{
					if (cmd == _handlers[i].cmd)
					{
						_handlers[i].func(_handlers[i].cmd);
						break;
					}
				}
			}
		}
		else // Inny adres czujnika ni� 0x00 oraz ID czujnik�w dost�pnych w sieci
		{
			PRINTF("COMM_APP: Nie nasz adres\r\n");
		}
	}
}

// static void Comm_ProcessMessage(void)
//
//{
//	Sensor.addressing_by_MAC = false;
//	Sensor.not_broadcast_mode = true;
//	if((CommLink_GetCommand() > 0x21) && Sensor.aqusition)
//	{
//		PRINTF_APP("COMM_APP: Sorry trwa akwizycja, komenda nieobs�ugiwana\r\n");
//	}
//	else
//	{
//		if(CommLink_GetAddr() == Sensor.configuration.sensor_ID)
//		{
//			uint8_t cmd = CommLink_GetCommand();
//			if(cmd == 0x2F && CommLink_GetAddr()!= 0x00)
//			{
//				PRINTF("Komenda nie obslugiwana po ID\r\n");
//			}
//			else if(cmd == 0x2F && CommLink_GetAddr()== 0x00)
//			{
//				Sensor.addressing_by_MAC=true;
//				PRINTF_APP("COMM_APP: Adresowanie po MAC\r\n");
//				handler_reset(cmd);
//			}
//			else if(cmd == 0x25 && CommLink_GetAddr()!= 0x00)
//			{
//				PRINTF("Komenda nie obslugiwana po ID");
//			}
//			else if(cmd == 0x25 && CommLink_GetAddr()== 0x00)
//			{
//				Sensor.addressing_by_MAC=true;
//				PRINTF_APP("COMM_APP: Adresowanie po MAC\r\n");
//				handler_setSpeed(cmd);
//			}
//			else if(cmd == 0x22 && CommLink_GetAddr()== 0x00)
//			{
//				Sensor.addressing_by_MAC=true;
//				PRINTF_APP("COMM_APP: Adresowanie po MAC\r\n");
//				handler_setID(cmd);
//			}
//			else
//			{
//				for(int i=0; i<sizeof(_handlers)/sizeof(_handlers[0]); ++i)
//				{
//					if(cmd == _handlers[i].cmd)
//					{
//						_handlers[i].func(_handlers[i].cmd);
//						break;
//					}
//				}
//			}
//
//		}
//		else if (CommLink_GetAddr() == 0x00) // Adresowanie po MAC
//		{
//			Sensor.addressing_by_MAC = true;
//			PRINTF_APP("COMM_APP: Adresowanie po MAC\r\n");
//			uint8_t cmd = CommLink_GetCommand();
//						for(int i=0; i<sizeof(_handlers)/sizeof(_handlers[0]); ++i)
//						{
//							if(cmd == _handlers[i].cmd)
//							{
//								_handlers[i].func(_handlers[i].cmd);
//								break;
//							}
//						}
//		}
//		else if (CommLink_GetAddr() == 0xff) // Adresowanie po MAC
//		{
//			Sensor.not_broadcast_mode = false;
//			uint8_t cmd = CommLink_GetCommand();
//			if (cmd == starAcq)
//			{
//				PRINTF_APP("COMM_APP: Polecenie rozg�oszeniowe StartACQ \r\n");
//				handler_starAcq(cmd);
//			}
//			else if(cmd == stopAcq)
//			{
//				PRINTF_APP("COMM_APP: Polecenie rozg�oszeniowe stopAcq \r\n");
//				handler_stopAcq(cmd);
//			}
//			else
//			{
//				PRINTF_APP("COMM_APP: Polecenie rozg�oszeniowe niedozwolona komenda w trybie broadcast \r\n");
//			}
//		}
//		else
//		{
//			PRINTF("COMM_APP: Nie nasz adres\r\n");
//		}
//	}
// }
static bool shouldStartFrameProcessing;
/******************************************************************************/
void Comm_Init(void)
{
	shouldStartFrameProcessing = false;
	CommLink_Init();
}
/******************************************************************************/
void Comm_Process(void)
{
	/*if (!shouldStartFrameProcessing)
	 {
	 return;
	 }
	 shouldStartFrameProcessing = false;*/
	if (FrameReady == CommLink_Process())
	{
		Comm_ProcessMessage();
	}
}
/******************************************************************************/
void Comm_StartFrameProcess(void)
{
	shouldStartFrameProcessing = true;
}

/******************************************************************************/

static void ota_body(uint8_t cmd)
{

	uint8_t *dataPointer = CommLink_GetPrtDataFrame();
	uint16_t data_size = CommLink_GetDataSize();

	// PRINTF_APP("ota_body %d\r\n",data_size);
	OTC_Body(dataPointer, data_size);
	// CommLink_SendData(Sensor.configuration.sensor_ID, cmd, 0, 0, 0);
}

void swap_t(uint8_t cmd)
{
	AdiConf_t newConfig;
	Memory_TakeConfig(&newConfig);
	switch (Protocol)
	{
	case SLS:
		Protocol = MODBUS;
		newConfig.RS485_speed = speed_9600;
		modbus_coils_table[1].coil_state = false;
		flushSLSBuffer();
		CommLink_SendData(Sensor.configuration.sensor_ID, cmd, 0, 0, 0);
		break;
	default:
	case MODBUS:
		Protocol = SLS;
		newConfig.RS485_speed = speed_1500k;
		modbusRTU_BufferFlush();
		break;
	}
	newConfig.protocol = Protocol;
	Memory_SaveConfig(&newConfig);
	PRINTF_APP("Protocol changed to: %s \n", enum2str(Protocol));
    Sensor.configuration.sensor_ID = newConfig.sensor_ID;
    memcpy(Sensor.configuration.MAC, newConfig.MAC, 12);
    Sensor.configuration.RS485_speed = newConfig.RS485_speed;
	ACQ_Init();
}

uint8_t *Comm_ModbusMath()
{

	//

#ifdef SENSOR_TYPE_LF
	if (Sensor.parameters.ScalarCount == 1)
	{
		Temp_SampleGet(respData);
	}
	// respLen = ACQ_GetSamples(respData + (sizeof(float) * Sensor.parameters.ScalarCount), 3 * samplesRequested) + sizeof(float) * Sensor.parameters.ScalarCount;
#elif defined SENSOR_TYPE_HF
	uint32_t samplesRequested = MAX_SAMPLES_AT_ONCE;
	ACQ_GetSamples(respData + (sizeof(float) * Sensor.parameters.ScalarCount), samplesRequested);

	if (Sensor.parameters.ScalarCount == 1)
	{
		Temp_SampleGet(respData);
	}
	// respLen = ACQ_GetSamples(respData+(sizeof(float)*Sensor.parameters.ScalarCount),samplesRequested) + sizeof(float)*Sensor.parameters.ScalarCount;
#endif

	ACQ_clearOverFlow();

	PRINTF_APP("COMM_APP: Get measurement\r\n");
	return respData;
}

uint32_t Comm_Modbus_MaxSampleAtOnce()
{
	return MAX_SAMPLES_AT_ONCE;
}
