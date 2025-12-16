/*
 * MacAddress.c
 *
 *  Created on: 26.06.2019
 *      Author: kgil
 */
#include <stdint.h>
#include <debug.h>
#include "memory.h"
/**
 * The STM32 factory-programmed UUID memory.
 * Three values of 32 bits each starting at this address
 * Use like this: STM32_UUID[0], STM32_UUID[1], STM32_UUID[2]
 */
#define STM32_UUID ((uint32_t *)0x1FFF7A10)

char* GetMACAddress(void)
{

	AdiConf_t currentConf;
	Memory_TakeConfig(&currentConf);
	static char MAC_Table[12];

	memcpy(MAC_Table,currentConf.MAC,12);
	return MAC_Table;
}

uint8_t GetMACByte(uint8_t i)
{

	static char MAC_Table[12];


	MAC_Table[0] =  (char)(STM32_UUID[1]);
	MAC_Table[1] =  (char)(STM32_UUID[1]>>8);
	MAC_Table[2] =  (char)(STM32_UUID[1]>>16);
	MAC_Table[3] =  (char)(STM32_UUID[1]>>24);
	MAC_Table[4] =  (char)(STM32_UUID[2]);
	MAC_Table[5] =  (char)(STM32_UUID[2]>>8);
	MAC_Table[6] =  (char)(STM32_UUID[2]>>16);
	MAC_Table[7] =  (char)(STM32_UUID[2]>>24);
	MAC_Table[8] =  (char)(STM32_UUID[3]);
	MAC_Table[9] =  (char)(STM32_UUID[3]>>8);
	MAC_Table[10] =  (char)(STM32_UUID[3]>>16);
	MAC_Table[11] =  (char)(STM32_UUID[3]>>24);

	//	 PRINTF("%d \t  %d \t %x\n",i,&STM32_UUID[1],STM32_UUID[1]);
	//	 PRINTF("%d \t  %d \t %x\n",i,&STM32_UUID[2],STM32_UUID[2]);
	//	 PRINTF("%d \t  %d \t %x\n",i,&STM32_UUID[3],STM32_UUID[3]);
	//	 for(int j=0;j<12;j++)
	//	 {
	//		 PRINTF("%x",foo_char[j]);
	//	 }
	//PRINTF("%lf",MAC_Table);
	return MAC_Table[i];
}


