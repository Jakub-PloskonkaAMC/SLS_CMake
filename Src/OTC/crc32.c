/*
 * crc32.c
 *
 *  Created on: 13 sie 2020
 *      Author: mantoniak
 */

#include <stdint.h>

static uint32_t table32[0x100];

static void GenerateTable(void)
{
  for (int i = 0; i < 0x100; i++)
  {
    uint32_t value = i;
    for (int j = 0; j < 8; j++)
    {
      if ((value & 1) == 1)
      {
        value = (value >> 1) ^ 0xEDB88320L;
      }
      else
      {
        value = value >> 1;
      }
    }
    table32[i] = value;
  }
}

static uint32_t midCrc;

static uint32_t _crc32(uint8_t* buffer, uint32_t length, uint32_t curVal)
{
  for (uint32_t i = 0; i < length; i++)
  {
    curVal = (curVal >> 8) ^ table32[buffer[i] ^ (curVal & 0xFF)];
  }
  return curVal;
}

void PrepareCrc32Calculation(void)
{
  midCrc = 0xFFFFFFFFL;
}

void PartCrc32Calculation(uint8_t* buffer, uint32_t length)
{
  midCrc = _crc32(buffer,length,midCrc);
}

uint32_t EndCrc32Calculation(void)
{
  return ~midCrc;
}

/*
static uint32_t software_B_CRC(void)
{
  uint8_t readedPage[0x1000];
  uint32_t softwareB_CRC = 0;
  uint16_t checkLen = 0x1000;
  PrepareCrc32Calculation();
  for (int i = 0; i < softwareInfoB.pagesAmmount; i++)
  {
    memset(readedPage,0,0x1000);
    PartCrc32Calculation(readedPage,checkLen);
  }
  softwareB_CRC = EndCrc32Calculation();
  return softwareB_CRC;
}*/
