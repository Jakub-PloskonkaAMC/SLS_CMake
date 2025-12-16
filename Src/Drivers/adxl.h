#ifndef ADXL_H_
#define ADXL_H_

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "spi.h"

#define ADXL_ID_ADDRESS 0x00
#define ADXL_TEMP2_ADDRESS 0x06
#define ADXL_TEMP1_ADDRESS 0x07
#define ADXL_XDATA3_ADDRESS 0x08
#define ADXL_XDATA2_ADDRESS 0x09
#define ADXL_XDATA1_ADDRESS 0x0A
#define ADXL_YDATA3_ADDRESS 0x0B
#define ADXL_YDATA2_ADDRESS 0x0C
#define ADXL_YDATA1_ADDRESS 0x0D
#define ADXL_ZDATA3_ADDRESS 0x0E
#define ADXL_ZDATA2_ADDRESS 0x0F
#define ADXL_ZDATA1_ADDRESS 0x10

typedef enum
{
   DataZ,
   DataY,
   DataX,
}ADXL_AXIS;

uint8_t adxl_GetID(void);
uint8_t adxl_GetMST(void);
uint8_t adxl_GetPARTID(void);
void adxl_Init(void);
void adxl_Stop(void);
void blinkDiode(int ticTime);
uint8_t adxl_read(uint8_t address);
void adxl_read_fast(uint8_t address, uint8_t* data_rec);
int U2_to_dec(uint16_t u2Numb);
uint32_t adxl_GetAxisData(ADXL_AXIS Ax);
void adxl_GetAxisData_fast(uint8_t* data);
void adxl_GetTemp(uint8_t * result);


#endif /* ADXL_H_ */
