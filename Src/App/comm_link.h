#ifndef COMMUNICATION_COMM_LINK_H_
#define COMMUNICATION_COMM_LINK_H_

#include <stdint.h>
#include <stdbool.h>



typedef enum
{
   FrameError,
   FrameNotReady,
   FrameReady,
   FrameBadCrc,
   CancelListen,
}Comm_FrameError_t;


void CommLink_Init(void);
bool CommLink_SendData(uint8_t addrSensor, uint8_t command, uint8_t flags, const void *data, uint16_t len);
bool CommLink_SendData_Mac(uint8_t addrSensor, uint8_t command, uint8_t flags, uint32_t Mac1,uint32_t Mac2,uint32_t Mac3);

Comm_FrameError_t CommLink_Process(void);

void *CommLink_GetPrtDataFrame(void);
uint16_t CommLink_GetDataSize(void);
uint8_t CommLink_GetAddr(void);
uint8_t CommLink_GetCommand(void);
uint8_t CommLink_GetFlags(void);

#endif /* COMMUNICATION_COMM_LINK_H_ */
