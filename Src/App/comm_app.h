#ifndef COMMUNICATION_COMM_APP_H_
#define COMMUNICATION_COMM_APP_H_

#include <stdint.h>
#include <stdbool.h>
#include "conf.h"

enum command_type{
	starAcq=0x01,
	stopAcq=0x02,
	getData=0x11,
	getAddrMac=0x12,
	/*getTemp=0x13,*/
	swap = 0x19,
	getStatus=0x21,
	setID=0x22,
	getConfig=0x23,
	setConfig=0x24,
	setSpeed=0x25,
	calibration=0x26,
	reset=0x2F,
	ota=0x31
};

typedef struct __attribute__((packed))
{
	uint8_t MAC[12];
	char sensor_type[12];
	uint8_t RS485_speed;
	uint32_t DataBufferSize;
	uint16_t SampleRate;
	uint8_t BitResolution;
	uint8_t Vcoding;
	uint8_t ScalarCount;
	uint8_t AxisCount;
	uint32_t X_Calibration_A;
	uint32_t X_Calibration_B;
	uint32_t X_Calibration_Gain;
	uint32_t X_Calibration_Range;
	uint32_t X_Calibration_Sensitivity;
#ifdef SENSOR_TYPE_LF
	uint32_t Y_Calibration_A;
	uint32_t Y_Calibration_B;
	uint32_t Y_Calibration_Gain;
	uint32_t Y_Calibration_Range;
	uint32_t Y_Calibration_Sensitivity;
	uint32_t Z_Calibration_A;
	uint32_t Z_Calibration_B;
	uint32_t Z_Calibration_Gain;
	uint32_t Z_Calibration_Range;
	uint32_t Z_Calibration_Sensitivity;
#endif
} CommConfig_t;

void Comm_Init(void);
void Comm_Process(void);
void Comm_StartFrameProcess(void);
bool Comm_WmBusSendData(const void *data, uint8_t len);
bool Comm_ServiceSendData(const void *data, uint8_t len);
void Comm_StartFrameProcess(void);
void CheckScheduledSending();
void ResetScheduledSending(void);
void swap_t(uint8_t cmd);
uint8_t* Comm_ModbusMath();
uint32_t Comm_Modbus_MaxSampleAtOnce();

#endif /* COMMUNICATION_COMM_APP_H_ */
