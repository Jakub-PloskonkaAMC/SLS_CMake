/*
 * ModbusRTU_FunctionCallbacks.c
 *
 *  Created on: Jul 4, 2022
 *      Author: midec
 */

#include "ModbusRTU_FunctionCallbacks.h"
#include "comm_app.h"
#include "acquisition.h"
#include "ModbusMath/ModbusMath.h"
#include "ModbusMath/ModbusMath_MaxMin.h"
#include "temp.h"
#include "ModbusRTU.h"
#include "estimates_calc.h"

static modbus_register_holding_struct_t modbus_register_holding_struct;
extern modbus_coil_table_entry_t modbus_coils_table[];

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void _modbusRTUFCallbacks_registersStructureRead()
{
	AdiConf_t confAllTemp;
	Memory_TakeConfig(&confAllTemp);
	modbus_register_holding_struct.sensor_ID = confAllTemp.sensor_ID;
	memcpy((char *)&modbus_register_holding_struct.MAC, (const char *)&confAllTemp.MAC, MAC_ADDRESS_NUMBER_OF_BYTES);
	modbus_register_holding_struct.RS485_speed = confAllTemp.RS485_speed;
	memcpy((char *)&modbus_register_holding_struct.sensor_type, (const char *)&confAllTemp.sensor_type, 12);
	modbus_register_holding_struct.X_Calibration_A = confAllTemp.X_Calibration_A;
	modbus_register_holding_struct.X_Calibration_B = confAllTemp.X_Calibration_B;
	modbus_register_holding_struct.X_Calibration_Sensitivity = confAllTemp.X_Calibration_Sensitivity;
	modbus_register_holding_struct.Y_Calibration_A = confAllTemp.Y_Calibration_A;
	modbus_register_holding_struct.Y_Calibration_B = confAllTemp.Y_Calibration_B;
	modbus_register_holding_struct.Y_Calibration_Sensitivity = confAllTemp.Y_Calibration_Sensitivity;
	modbus_register_holding_struct.Z_Calibration_A = confAllTemp.Z_Calibration_A;
	modbus_register_holding_struct.Z_Calibration_B = confAllTemp.Z_Calibration_B;
	modbus_register_holding_struct.Z_Calibration_Sensitivity = confAllTemp.Z_Calibration_Sensitivity;

	Temp_SampleGet(modbus_register_holding_struct.temperature);

	calculated_estimates_t estimates = getEstimates();
	
#if defined SENSOR_TYPE_HF
	memcpy((char*)&modbus_register_holding_struct.PP, (char*)&estimates.PP, sizeof(float));
	memcpy((char*)&modbus_register_holding_struct.RMS, (char*)&estimates.RMS, sizeof(float));
	memcpy((char*)&modbus_register_holding_struct.VRMS, (char*)&estimates.VRMS, sizeof(float));
	memcpy((char*)&modbus_register_holding_struct.envZP, (char*)&estimates.envZP, sizeof(float));
	memcpy((char*)&modbus_register_holding_struct.envRMS, (char*)&estimates.envRMS, sizeof(float));

	modbus_register_holding_struct.unused[0] = 0;
	modbus_register_holding_struct.unused[1] = 0;
	modbus_register_holding_struct.unused[2] = 0;
	modbus_register_holding_struct.unused[3] = 0;

	modbus_register_holding_struct.is_HF = 1;
#elif defined SENSOR_TYPE_LF
	memcpy((char*)&modbus_register_holding_struct.PP, (char*)&estimates.x_PP, sizeof(float));
	memcpy((char*)&modbus_register_holding_struct.RMS, (char*)&estimates.x_RMS, sizeof(float));
	memcpy((char*)&modbus_register_holding_struct.VRMS, (char*)&estimates.x_VRMS, sizeof(float));

	memcpy((char*)&modbus_register_holding_struct.y_PP, (char*)&estimates.y_PP, sizeof(float));
	memcpy((char*)&modbus_register_holding_struct.y_RMS, (char*)&estimates.y_RMS, sizeof(float));
	memcpy((char*)&modbus_register_holding_struct.y_VRMS, (char*)&estimates.y_VRMS, sizeof(float));

	memcpy((char*)&modbus_register_holding_struct.z_PP, (char*)&estimates.z_PP, sizeof(float));
	memcpy((char*)&modbus_register_holding_struct.z_RMS, (char*)&estimates.z_RMS, sizeof(float));
	memcpy((char*)&modbus_register_holding_struct.z_VRMS, (char*)&estimates.z_VRMS, sizeof(float));

	modbus_register_holding_struct.is_HF = 0;
#endif // SENSOR_TYPE_HF

#ifdef DEBUG_ENABLED
	float valtest;
	memcpy(&valtest, &modbus_register_holding_struct.PP, sizeof(float));
	PRINTF_MODBUS("test PP:%f\n", valtest);
	memcpy(&valtest, &modbus_register_holding_struct.RMS, sizeof(float));
	PRINTF_MODBUS("test RMS:%f\n", valtest);
	memcpy(&valtest, &modbus_register_holding_struct.VRMS, sizeof(float));
	PRINTF_MODBUS("test VRMS:%f\n", valtest);
#if defined SENSOR_TYPE_HF
	memcpy(&valtest, &modbus_register_holding_struct.envZP, sizeof(float));
	PRINTF_MODBUS("test envZP:%f\n", valtest);
	memcpy(&valtest, &modbus_register_holding_struct.envRMS, sizeof(float));
	PRINTF_MODBUS("test envRMS:%f\n", valtest);
#endif // SENSOR_TYPE_HF
#endif // DEBUG_ENABLED

	modbus_register_holding_struct.software_version[0] = SOFTWARE_MAJOR;
	modbus_register_holding_struct.software_version[1] = SOFTWARE_MINOR;
	modbus_register_holding_struct.software_version[2] = SOFTWARE_PATCH;
	modbus_register_holding_struct.hardware_version[0] = HARDWARE_MAJOR;
	modbus_register_holding_struct.hardware_version[1] = HARDWARE_MINOR;
	modbus_register_holding_struct.hardware_version[2] = HARDWARE_REVISION;
}

void _modbusRTUFCallbacks_registersStructureWrite()
{
	AdiConf_t confAllTemp;
	Memory_TakeConfig(&confAllTemp);
	confAllTemp.sensor_ID = modbus_register_holding_struct.sensor_ID;
	memcpy((char *)&confAllTemp.MAC, (const char *)&modbus_register_holding_struct.MAC,
		   MAC_ADDRESS_NUMBER_OF_BYTES);
	confAllTemp.RS485_speed = modbus_register_holding_struct.RS485_speed;
	memcpy((char *)&confAllTemp.sensor_type, (const char *)&modbus_register_holding_struct.sensor_type, 12);
	confAllTemp.X_Calibration_A = modbus_register_holding_struct.X_Calibration_A;
	confAllTemp.X_Calibration_B = modbus_register_holding_struct.X_Calibration_B;
	confAllTemp.X_Calibration_Sensitivity = modbus_register_holding_struct.X_Calibration_Sensitivity;
	confAllTemp.Y_Calibration_A = modbus_register_holding_struct.Y_Calibration_A;
	confAllTemp.Y_Calibration_B = modbus_register_holding_struct.Y_Calibration_B;
	confAllTemp.Y_Calibration_Sensitivity = modbus_register_holding_struct.Y_Calibration_Sensitivity;
	confAllTemp.Z_Calibration_A = modbus_register_holding_struct.Z_Calibration_A;
	confAllTemp.Z_Calibration_B = modbus_register_holding_struct.Z_Calibration_B;
	confAllTemp.Z_Calibration_Sensitivity = modbus_register_holding_struct.Z_Calibration_Sensitivity;

	Memory_SaveConfig(&confAllTemp);
}

int _modbusRTUFCallbacks_bitArrayToInt(bool arr[], int count)
{
	bool TempArr1[count];
	for (int j = 0; j < count; j++)
	{
		TempArr1[j] = arr[count - j - 1];
	}
	int ret = 0;
	int tmp;
	for (int i = 0; i < count; i++)
	{
		tmp = TempArr1[i];
		ret |= tmp << (count - i - 1);
	}
	return ret;
}

uint8_t _modbusRTUFCallbacks_getCoilByte(uint16_t byte_address, uint16_t byte_amount)
{
	uint8_t ret_data = 0;
	bool ret_dataTable[8] =
		{
			0};

	for (uint8_t i = 0; i < byte_amount; i++)
	{
		ret_dataTable[i] = modbus_coils_table[i + byte_address].coil_state;
	}

	ret_data = _modbusRTUFCallbacks_bitArrayToInt(ret_dataTable, 8);

	return ret_data;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

/////////////////////// COILS   ///////////////////////////////////////////////////

void modbusRTUFCallbacks_readCoils(modbus_message_t *mess)
{
	PRINTF_MODBUS("MODBUS read coils callback\n");
	modbus_message_t response;
	uint8_t response_data[30];
	response.address = mess->address;
	response.data = response_data;
	response.data_length = 0;
	response.function = mess->function;
	uint16_t coil_add = (mess->data[0] << 8) + mess->data[1];
	uint16_t coil_nmb = (mess->data[2] << 8) + mess->data[3];

	bool coil_avbl = 0;

	if (coil_add <= 8)
	{
		coil_avbl = true;
	}

	if (coil_avbl)
	{
		if (coil_nmb <= 8)
		{
			response_data[0] = ((coil_nmb - 1) / 8) + 1;
			response_data[1] = _modbusRTUFCallbacks_getCoilByte(coil_add, coil_nmb);
			response.data_length = 2;
			PRINTF_MODBUS("MODBUS read coils start:%d, amount:%d value:%d/n", coil_add, coil_nmb, response_data[1]);
		}
		modbusRTU_sendMessage(&response);
	}
}

void modbusRTUFCallbacks_writeCoil(modbus_message_t *mess)
{
	PRINTF_MODBUS("MODBUS write single coil callback\n");
	if (mess->data_length == 4)
	{
		uint16_t coil = (mess->data[0] << 8) + mess->data[1];
		uint16_t data = (mess->data[2] << 8) + mess->data[3];
		bool dataBoll;
		if (data == MODBUS_COIL_VALUE_FALSE)
		{
			dataBoll = false;
		}
		else if (data == MODBUS_COIL_VALUE_TRUE)
		{
			dataBoll = true;
		}
		else
		{
			PRINTF_MODBUS("ERROR: wrong coil value...");
			return;
		}

		switch (coil)
		{
		case modbusACQ:
			if (dataBoll == true)
			{
				PRINTF_MODBUS("Modbus coil ACQ true");
				modbus_coils_table[coil].coil_state = true;
				ModbusMath_ACQStart_Write(true);
				ACQ_Start();
			}
			else if (dataBoll == false)
			{
				PRINTF_MODBUS("Modbus coil ACQ false");
				modbus_coils_table[coil].coil_state = false;
				ModbusMath_ACQStart_Write(false);
				ACQ_Stop();
			}
			else
				PRINTF_MODBUS("ERROR: illegal coil value MA...");
			break;

		case modbusSwapProtocol:
			if (dataBoll == true)
			{
				PRINTF_MODBUS("Modbus coil SwapProtocol true");
				modbus_coils_table[coil].coil_state = true;
				modbusRTU_sendMessage(mess);
				swap_t(coil);
			}
			else
				PRINTF_MODBUS("ERROR: illegal coil value MSP...");
			break;

		case modbusReset:
			if (dataBoll == true)
			{
				modbus_coils_table[coil].coil_state = true;
				PRINTF_MODBUS("Modbus coil Reset device true");
				modbusRTU_sendMessage(mess);
				HAL_Delay(200);
				HAL_NVIC_SystemReset();
			}
			else
			{
				PRINTF_MODBUS("ERROR: illegal coil value MR...");
			}
			break;

		case modbusResetConf:
			if (dataBoll == true)
			{
				PRINTF_MODBUS("Modbus coil reset conf true");
				modbus_coils_table[coil].coil_state = true;
				AdiConf_t confAllTemp;
				Memory_TakeConfig(&confAllTemp);
				Memory_MakeDefaultConfig(&confAllTemp);
				Memory_SaveConfig(&confAllTemp);
			}
			else
				PRINTF_MODBUS("ERROR: illegal coil value MSP...");

			break;

		case modbusACQMaxMin:
			if (dataBoll == true)
			{
				PRINTF_MODBUS("Modbus coil ACQMaxMin true");
				modbus_coils_table[coil].coil_state = true;
				ModbusMath_MaxMin_Start();
				Comm_ModbusMath();
			}
			else if (dataBoll == false)
			{
				PRINTF_MODBUS("Modbus coil ACQMaxMin false");
				modbus_coils_table[coil].coil_state = false;
				ModbusMath_MaxMin_Stop();
				//
			}
			else
			{
				PRINTF_MODBUS("ERROR: illegal coil value MR...");
			}
			break;

		default:
			PRINTF_MODBUS("Command not found. A wrong modbus coil has been chosen...");
			return;
		}
	}

	modbusRTU_sendMessage(mess);
}

///////////////////////////////////////////////////////////////////////////////////

/////////////////////// REGISTERS  ////////////////////////////////////////////////

void modbusRTUFCallbacks_readRegisters(modbus_message_t *mess)
{
	PRINTF_MODBUS("MODBUS read MULTIPLE register callback\n");
	modbus_message_t response;
	uint8_t response_data[252];
	response.address = mess->address;
	response.data = response_data;
	response.function = mess->function;
	response.data_length = 0;
	uint16_t reg_add = ((mess->data[0] << 8) + mess->data[1]) * 2;
	uint16_t reg_nmb = ((mess->data[2] << 8) + mess->data[3]) * 2;

	uint8_t response_dataBuffTemp[252] = {0};
	uint16_t response_dataTemp = 0;

	PRINTF_MODBUS("modbusRTUFCallbacks_readRegisters regAdd:%d  regNmb:%d\n", reg_add, reg_nmb);

	response.data[0] = reg_nmb;

	_modbusRTUFCallbacks_registersStructureRead();

	// Bez odwracania bajtow (LOW first)
	// memcpy((char *)&response.data[1], (const char *)&modbus_register_holding_struct + reg_add, reg_nmb);

	// Odwrocenie bajtow (HIGH first)
	memcpy((char *)&response_dataBuffTemp[0], (const char *)&modbus_register_holding_struct + reg_add, reg_nmb);
	for (int i = 0; i < reg_nmb; i += 2)
	{
		memcpy((char *)&response_dataTemp, (const char *)&response_dataBuffTemp[i], 2);
		response_dataTemp = __builtin_bswap16(response_dataTemp);
		memcpy((char *)&response.data[1 + i], (const char *)&response_dataTemp, 2);
	}

	response.data_length = (reg_nmb + 1);

	PRINTF_MODBUS("modbusRTUFCallbacks_readRegisters\n");

	response.error_check = 0;
	PRINTF_MODBUS("Holding registers request OK\n");

	modbusRTU_sendMessage(&response);
}

void modbusRTUFCallbacks_writeRegisters(modbus_message_t *mess)
{
	PRINTF_MODBUS("MODBUS write MULTIPLE register callback\n");
	uint8_t recive_data[252];
	modbus_message_t response;
	uint8_t response_data[252];
	uint8_t response_dataBuffTemp[252] = {0};
	uint16_t response_dataTemp = 0;
	response.address = mess->address;
	response.data = response_data;
	response.function = mess->function;
	response.data_length = 0;

	memcpy((char *)recive_data, (const char *)mess->data, sizeof(recive_data));

	uint16_t startAddress = (mess->data[0] << 8) + mess->data[1];
	uint16_t quantityOfRegisters = ((mess->data[2] << 8) + mess->data[3]);
	uint16_t byteCount = (mess->data[4]);

	PRINTF_MODBUS("Modbus write multiple registers Start address:%d, quantity:%d, byteCount:%d", startAddress, quantityOfRegisters, byteCount);

	_modbusRTUFCallbacks_registersStructureRead();

	// Bez odwracania bajtow (LOW first)
	// memcpy((char *)&modbus_register_holding_struct + startAddress, (const char *)&recive_data[5], quantityOfRegisters*2);

	// Odwrocenie bajtow (HIGH first)
	memcpy((char *)&response_dataBuffTemp[0], (const char *)&recive_data[5], quantityOfRegisters * 2);
	for (int i = 0; i < quantityOfRegisters * 2; i += 2)
	{
		memcpy((char *)&response_dataTemp, (const char *)&response_dataBuffTemp[i], 2);
		response_dataTemp = __builtin_bswap16(response_dataTemp);
		memcpy((char *)&modbus_register_holding_struct + i + startAddress * 2, (const char *)&response_dataTemp, 2);
	}

	_modbusRTUFCallbacks_registersStructureWrite();

	uint16_t startAddressBE = __builtin_bswap16(startAddress);
	memcpy((char *)&response.data[0], (const char *)&startAddressBE, 2);
	uint16_t quantityOfRegistersBE = __builtin_bswap16(quantityOfRegisters);
	memcpy((char *)&response.data[2], (const char *)&quantityOfRegistersBE, 2);

	response.data_length = 4;
	modbusRTU_sendMessage(&response);

	return;
}
