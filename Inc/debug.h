/*
 * debug.h
 *
 *  Created on: 26.06.2019
 *      Author: mantoniak
 */

#ifndef DEBUG_H_
#define DEBUG_H_

//#define DEBUG_SINK_RS485

#include "conf.h"

#define LOG_APP 0
#define LOG_HW 1
#define LOG_FU 2
#define LOG_ERROR 3
#define LOG_MEMORY 4
#define LOG_TEST 5
#define LOG_0 6
#define LOG_MODBUS 7
#define LOG_COMM 8

char *enum2str(CurrentProtocol t);

#ifdef DEBUG_APP
#define PRINTF_APP(...) LOG_fprintf(LOG_APP, __VA_ARGS__)
#else
#define PRINTF_APP(...) \
    do                  \
    {                   \
    } while (0)
#endif

#ifdef DEBUG_HW
#define PRINTF_HW(...) LOG_fprintf(LOG_HW, __VA_ARGS__)
#else
#define PRINTF_HW(...) \
    do                 \
    {                  \
    } while (0)
#endif

#ifdef DEBUG_MEMORY
#define PRINTF_MEM(...) LOG_fprintf(LOG_MEMORY, __VA_ARGS__)
#else
#define PRINTF_MEM(...) \
    do                  \
    {                   \
    } while (0)
#endif

#ifdef DEBUG_TEST
#define PRINTF_TEST(...) LOG_fprintf(LOG_TEST, __VA_ARGS__)
#else
#define PRINTF_TEST(...) \
    do                   \
    {                    \
    } while (0);
#endif

#ifdef DEBUG_PRINTF
#define PRINTF(...) LOG_fprintf(LOG_0, __VA_ARGS__)
#else
#define PRINTF(...) \
    do              \
    {               \
    } while (0)
#endif

#ifdef DEBUG_MODBUS
#define PRINTF_MODBUS(...) LOG_fprintf(LOG_MODBUS, __VA_ARGS__)
#else
#define PRINTF_MODBUS(...) \
    do                     \
    {                      \
    } while (0)
#endif


#ifdef DEBUG_COMM
#define PRINTF_COMM(...) LOG_fprintf(LOG_COMM, __VA_ARGS__)
#else
#define PRINTF_COMM(...) \
    do                     \
    {                      \
    } while (0)
#endif

#ifdef DEBUG_ENABLED
#define LOG_Init() UART7_Init()
#else
#define LOG_Init() \
    do             \
    {              \
    } while (0)
#endif

void UART7_IRQHandler(void);
void UART7_Init(void);
void LOG_fprintf(uint8_t flags, const char *str, ...);

void DMA1_Stream1_IRQHandler(void);

#endif /* DEBUG_H_ */
