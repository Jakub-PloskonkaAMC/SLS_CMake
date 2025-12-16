/*
 * debug.c
 *
 *  Created on: 26.06.2019
 *      Author: mantoniak
 */

#include "debug.h"

#ifdef DEBUG_ENABLED

#include <stdarg.h>
#include <stdio.h>
#include "gpio.h"

#ifdef DEBUG_SINK_RS485

#include "rs485.h"
#endif

#define DEBUG_DATA_MAX_LENGTH 128

#define LOG_QUEUE_SIZE 8
#define LOG_MSG_MAX_LEN DEBUG_DATA_MAX_LENGTH

typedef struct {
    char data[LOG_MSG_MAX_LEN];
    uint16_t length;
} LogMessage_t;

static LogMessage_t logQueue[LOG_QUEUE_SIZE];
static volatile uint8_t logHead = 0;
static volatile uint8_t logTail = 0;
static volatile bool logTransmitting = false;

UART_HandleTypeDef huart7;
DMA_HandleTypeDef hdma_uart7_tx;

char tempBuf[DEBUG_DATA_MAX_LENGTH];

static const char *log_sources[] =
	{
		"APP",
		"HW",
		"FAST UART",
		"ERROR",
		"MEM",
		"TEST",
		"LOG",
		"MODBUS",
		"COMM"};

char *enum2str(CurrentProtocol t)
{
	switch (t)
	{
	case SLS:
		return "SLS";
	case MODBUS:
		return "MODBUS";
	default:
		return "ERROR";
	}
	return NULL;

}

static void UART7_StartNextLog(void)
{
    if (logHead == logTail) {
        logTransmitting = false;
        return;
    }

#ifdef DEBUG_SINK_RS485
    _modbusRTU_portSetTx();
#endif

    logTransmitting = true;
    HAL_UART_Transmit_DMA(&huart7, (uint8_t*)logQueue[logTail].data, logQueue[logTail].length);

}

void LOG_fprintf(uint8_t flags, const char *str, ...)
{
    uint8_t nextHead = (logHead + 1) % LOG_QUEUE_SIZE;

    if (nextHead == logTail) {
        return;
    }

    va_list args;
    va_start(args, str);
    memset(logQueue[logHead].data, 0, LOG_MSG_MAX_LEN);

    switch (flags)
    {
    case LOG_APP:    snprintf(logQueue[logHead].data, LOG_MSG_MAX_LEN, "%s: ", log_sources[0]); break;
    case LOG_HW:     snprintf(logQueue[logHead].data, LOG_MSG_MAX_LEN, "%s: ", log_sources[1]); break;
    case LOG_FU:     snprintf(logQueue[logHead].data, LOG_MSG_MAX_LEN, "%s: ", log_sources[2]); break;
    case LOG_ERROR:  snprintf(logQueue[logHead].data, LOG_MSG_MAX_LEN, "%s: ", log_sources[3]); break;
    case LOG_MEMORY: snprintf(logQueue[logHead].data, LOG_MSG_MAX_LEN, "%s: ", log_sources[4]); break;
    case LOG_TEST:   snprintf(logQueue[logHead].data, LOG_MSG_MAX_LEN, "%s: ", log_sources[5]); break;
    case LOG_0:      snprintf(logQueue[logHead].data, LOG_MSG_MAX_LEN, "%s: ", log_sources[6]); break;
    case LOG_MODBUS: snprintf(logQueue[logHead].data, LOG_MSG_MAX_LEN, "%s: ", log_sources[7]); break;
    case LOG_COMM:   snprintf(logQueue[logHead].data, LOG_MSG_MAX_LEN, "%s: ", log_sources[8]); break;
    }

    vsnprintf(logQueue[logHead].data + strlen(logQueue[logHead].data),LOG_MSG_MAX_LEN - strlen(logQueue[logHead].data),str, args);
    va_end(args);

    logQueue[logHead].length = strlen(logQueue[logHead].data);
    logHead = nextHead;


    if (!logTransmitting) {
        UART7_StartNextLog();
    }
}

void UART7_Init(void)
{
    huart7.Instance = UART7;
    huart7.Init.BaudRate = 1500000;
    huart7.Init.WordLength = UART_WORDLENGTH_8B;
    huart7.Init.StopBits = UART_STOPBITS_1;
    huart7.Init.Parity = UART_PARITY_NONE;
    huart7.Init.Mode = UART_MODE_TX_RX;
    huart7.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart7.Init.OverSampling = UART_OVERSAMPLING_16;

    __HAL_RCC_DMA1_CLK_ENABLE();

    //DMA_Init
    hdma_uart7_tx.Instance = DMA1_Stream1;
    hdma_uart7_tx.Init.Channel = DMA_CHANNEL_5;
    hdma_uart7_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_uart7_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart7_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart7_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart7_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart7_tx.Init.Mode = DMA_NORMAL;
    hdma_uart7_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_uart7_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

    HAL_DMA_Init(&hdma_uart7_tx);

    __HAL_LINKDMA(&huart7, hdmatx, hdma_uart7_tx);

    HAL_UART_Init(&huart7);

    HAL_NVIC_SetPriority(UART7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(UART7_IRQn);

    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == UART7)
    {
#ifdef DEBUG_SINK_RS485
        _modbusRTU_portSetRx();
#endif

        logTail = (logTail + 1) % LOG_QUEUE_SIZE;
        UART7_StartNextLog();
    }
}

void UART7_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart7);
}

void DMA1_Stream1_IRQHandler(void)
{

	HAL_DMA_IRQHandler(&hdma_uart7_tx);
}

#else

#endif
