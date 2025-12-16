/*
 * ModbusRTU_Defines.h
 *
 *  Created on: Jul 4, 2022
 *      Author: midec
 */

#ifndef MODBUS_MODBUSRTU_DEFINES_H_
#define MODBUS_MODBUSRTU_DEFINES_H_

#include "ModbusRTU_Config.h"

typedef struct
{
	uint8_t address;  /** Slave address */
	uint8_t function; /** Function code */
	uint8_t *data;	  /** Message data */
	uint8_t data_length;
	uint16_t error_check;
} modbus_message_t;

typedef struct
{
	uint8_t *data;
	int head;
	int tail;
	int count;
	int size;
} ring_buffer_t;

typedef struct
{
	uint16_t coil_address;
	bool coil_state;
} modbus_coil_table_entry_t;

typedef void (*func_callback)(modbus_message_t *);

typedef struct
{
	int function_code;
	int data_length;
	func_callback callback;
} modbus_func_descriptor;

enum modbus_coil_names
{
	modbusACQ = 0x00,
	modbusSwapProtocol = 0x01,
	modbusReset = 0x02,
	modbusResetConf = 0x03,
	modbusACQRMS = 0x04,
	modbusACQZP = 0x05,
	modbusACQVRMS = 0x06,
	modbusACQGE = 0x07,
	modbusACQDataAble = 0x08,
	modbusACQMaxMin = 0x09,
};

typedef struct __attribute__((__packed__)) modbus_register_holding_struct
{
	uint16_t sensor_ID;
	char MAC[MAC_ADDRESS_NUMBER_OF_BYTES];
	uint16_t RS485_speed;
	char sensor_type[12];
	uint32_t X_Calibration_A;
	uint32_t X_Calibration_B;
	uint32_t X_Calibration_Sensitivity;
	uint32_t Y_Calibration_A;
	uint32_t Y_Calibration_B;
	uint32_t Y_Calibration_Sensitivity;
	uint32_t Z_Calibration_A;
	uint32_t Z_Calibration_B;
	uint32_t Z_Calibration_Sensitivity;
	uint8_t temperature[4];
	uint32_t PP;
	uint32_t RMS;
	uint32_t VRMS;

#if defined SENSOR_TYPE_HF
	uint32_t envZP;
	uint32_t envRMS;
	uint32_t unused[4];
#elif defined SENSOR_TYPE_LF
	uint32_t y_PP;
	uint32_t y_RMS;
	uint32_t y_VRMS;
	uint32_t z_PP;
	uint32_t z_RMS;
	uint32_t z_VRMS;
#endif

	uint16_t is_HF;
	uint16_t software_version[3];
	uint16_t hardware_version[3];
} modbus_register_holding_struct_t;

/** Everything is ok. */
#define MODBUS_STATUS_OK 0

/** General error occurs. */
#define MODBUS_STATUS_ERROR 8


#define MODBUS_COIL_VALUE_TRUE 0xFF00
#define MODBUS_COIL_VALUE_FALSE 0x0000

/** Modbus implemented functions */
#define MODBUS_FUNCTION_READ_COILS 0x01

#define MODBUS_FUNCTION_READ_HOLDING_REGISTERS 0x03

#define MODBUS_FUNCTION_WRITE_SINGLE_COIL 0x05

#define MODBUS_FUNCTION_WRITE_MULTIPLE_REGISTER 0x10

#endif /* MODBUS_MODBUSRTU_DEFINES_H_ */
