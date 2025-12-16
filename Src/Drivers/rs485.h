/*
 * rs485.h
 *
 *  Created on: 26.06.2019
 *      Author: mantoniak
 */

#ifndef DRIVERS_RS485_H_
#define DRIVERS_RS485_H_

#include "conf.h"
#include "memory.h"

#define RE_BIT_MASK 	0x0004
#define TE_BIT_MASK 	0x0008
#define RXNEIE_BIT_MASK 0x0020

void RS485_RX_Sleep(uint16_t dataLength);
void RS485_Clear(void);
void RS485_RX_SleepDone(void);

#define _LL_USART1_CR1_RE_BIT_DISABLE() 			(USART1->CR1 &= ~((uint32_t)RE_BIT_MASK))
#define _LL_USART1_CR1_TE_BIT_DISABLE() 			(USART1->CR1 &= ~((uint32_t)TE_BIT_MASK))
#define _LL_USART1_CR1_RXNEIE_BIT_DISABLE()  	(USART1->CR1 &= ~((uint32_t)RXNEIE_BIT_MASK))

#define _LL_USART1_CR1_RE_BIT_ENABLE() 		(USART1->CR1 |= ((uint32_t)RE_BIT_MASK))
#define _LL_USART1_CR1_TE_BIT_ENABLE() 		(USART1->CR1 |= ((uint32_t)TE_BIT_MASK))
#define _LL_USART1_CR1_RXNEIE_BIT_ENABLE()  	(USART1->CR1 |= ((uint32_t)RXNEIE_BIT_MASK))

void MX_UART1_RX_DeInit(void);
void MX_UART1_RX_ReInit(void);

void MX_UART1_TX_DeInit(void);
void MX_UART1_TX_ReInit(void);

void DataTimerDone(void);
void USART1_IRQHandler(void);
void FAST_UART_RX_Start(void);

void RS485_receive(void);
void RS_485_transmit_with_delay(char *data, int lenght, int delay);

/*
 * ***************************************************
 */


void RS485_Init(ADI_rs485_speed spd);
uint16_t RS485_Send(const void *data, uint16_t len);
bool RS485_GetByte(uint8_t *ch);
uint16_t RS485_BuferCount(void);
void RS485_TransmitOn(void);
void RS485_TransmitOff(void);
void flushSLSBuffer(void);

#endif /* DRIVERS_RS485_H_ */
