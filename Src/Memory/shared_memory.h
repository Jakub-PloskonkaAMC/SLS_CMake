#ifndef MEMORY_SHARED_MEMORY
#define MEMORY_SHARED_MEMORY

#include "conf.h"

#ifdef SENSOR_TYPE_HF

#include "../Estimates_Lib/rtwtypes.h"

#define DATA_BUFFER_LENGTH_SLS 67232
#define DATA_BUFFER_LENGTH_MODBUS 32100
#define BUFFER_SIZE sizeof(float)

typedef struct
{
	uint32_t start;
	uint32_t end;
	uint32_t size;
	bool overFlw;
} generalDataStorage;

typedef struct
{
	uint32_t start;
	uint32_t end;
	uint32_t size;
	bool overFlw;

	uint8_t data[BUFFER_SIZE * DATA_BUFFER_LENGTH_SLS];
} slsDataStorage_t;

typedef struct
{
	uint32_t start;
	uint32_t end;
	uint32_t size;
	bool overFlw;

	uint8_t data[BUFFER_SIZE * DATA_BUFFER_LENGTH_MODBUS];
	float x[8000];
	float b_x[4096];
	float fv[4096];
	float fv1[4096];
	float c_x[1024];
	float x_env[1024];
	float c_y[508];
	creal32_T X[4096];
	creal32_T shifted_spectrum[1024];
	creal32_T xCol[1024];
} modbusDataStorage_t;

typedef union
{
	generalDataStorage general;
	slsDataStorage_t sls;
	modbusDataStorage_t modbus;
} DataStorage_t;

DataStorage_t dataStorage;

#endif // SENSOR_TYPE_HF
#endif // MEMORY_SHARED_MEMORY
