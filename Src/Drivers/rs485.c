/*
 * rs485.c
 *
 *  Created on: 26.06.2019
 *      Author: mantoniak
 */

#include "rs485.h"
#include "debug.h"
#include "timers.h"
#include "cyclicBuffer.h"
#include "board.h"
#include "gpio.h"
#include <math.h>
#include "ModbusRTU.h"

UART_HandleTypeDef huart1;

volatile uint32_t tmp;
volatile bool _timerRunning;

#define NOP_WAIT_AMMOUNT 400

void RS485_RX_Sleep(uint16_t dataLength)
{
	float speed = 0;
	switch(Sensor.configuration.RS485_speed)
	{
		case speed_4800:
			speed = 4800.0;
			break;
		case speed_7200:
			speed = 7200.0;
			break;
		case speed_9600:
			speed = 9600.0;
			break;
		case speed_14400:
			speed = 14400.0;
			break;
		case speed_19200:
			speed = 19200.0;
			break;
		case speed_28800:
			speed = 28800.0;
			break;
		case speed_33600:
			speed = 33600.0;
			break;
		case speed_38400:
			speed = 38400.0;
			break;
		case speed_57600:
			speed = 57600.0;
			break;
		case speed_1500k:
			speed = 1500000.0;
			break;
		case speed_500k:
			speed = 500000.0;
			break;
		case speed_115k:
			speed = 115200.0;
			break;
		case speed_1000k:
			speed = 1000000.0;
			break;
		default:
			speed = 115200.0;
			break;
	}
	float timeInSecToSleep = (8.0*dataLength)/(speed);
	uint32_t miliSecsToSleep = (uint32_t)ceil(1000*timeInSecToSleep);
	TimerSleep_SetTime(miliSecsToSleep*2); // tu było 2*
	TimerSleep_Start();
	MX_UART1_RX_DeInit();
}

void RS485_RX_SleepDone(void)
{
	MX_UART1_RX_ReInit();
}

void MX_UART1_RX_DeInit(void)
{
	_LL_USART1_CR1_RE_BIT_DISABLE();
	_LL_USART1_CR1_RXNEIE_BIT_DISABLE();
}

void MX_UART1_RX_ReInit(void)
{
	_LL_USART1_CR1_RE_BIT_ENABLE();
	_LL_USART1_CR1_RXNEIE_BIT_ENABLE();
}

void MX_UART1_TX_DeInit(void)
{
	_LL_USART1_CR1_TE_BIT_DISABLE();
}

void MX_UART1_TX_ReInit(void)
{
	_LL_USART1_CR1_TE_BIT_ENABLE();
}


void RS_485_transmit_with_delay(char *data, int lenght, int delay)
{
	 HAL_GPIO_WritePin(USART__R_E_GPIO_Port, USART__R_E_Pin, GPIO_PIN_SET);
	 HAL_GPIO_WritePin(USART_DE_GPIO_Port, USART_DE_Pin, GPIO_PIN_SET);
	 HAL_UART_Transmit(&huart1, (uint8_t*)data, lenght, 1000);
	 HAL_Delay(delay);
}

/*
*******************************************************
 */


static uint8_t _rxByte;

void RS485_receive(void)
{
	HAL_GPIO_WritePin(USART__R_E_GPIO_Port, USART__R_E_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(USART_DE_GPIO_Port, USART_DE_Pin, GPIO_PIN_RESET);
	//HAL_UART_Receive(&huart1, value, 1, 100);
	HAL_UART_Receive_IT(&huart1,&_rxByte,1);
}

static uint8_t _buff[1024];
static struct CyclicBuffer _rxBuff = { 0, 0, 0, sizeof(_buff), _buff};

void flushSLSBuffer(void)
{
	CyclicBuffer_flush(&_rxBuff);
}

typedef enum
{
	TransmitDone,
	DataError,
	TransmitTimeout,
	UartBusy

}UartTransmitResult;

UartTransmitResult UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  if (huart->gState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return  DataError;
    }
    uint16_t TxXferSize = Size;
    uint32_t waitCounter = 0;
	while (TxXferSize > 0U)
	{
		TxXferSize--;
		while ((huart->Instance->SR & UART_FLAG_TXE) == RESET)
		{
			waitCounter++;
			if (waitCounter > 10000)
			{
				return TransmitTimeout;
			}
		}
		huart->Instance->DR = (*pData++ & (uint8_t) 0xFF);
		waitCounter = 0;
	}
	while ((huart->Instance->SR & UART_FLAG_TXE) == RESET)
	{
		//waitCounter++;
		//if (waitCounter > 10000)
		{
			return TransmitTimeout;
		}
	}
	return TransmitDone;
  }
  else
  {
    return UartBusy;
  }
}

uint16_t RS485_Send(const void *data, uint16_t len)
{

	if(data == 0 || len == 0)
		return 0;

	uint8_t *ptr = (uint8_t *)data;
	UART_Transmit(&huart1, ptr, len);
	//HAL_UART_Transmit(&huart1, ptr, len, 5000);
	//HAL_UART_Transmit_IT(&huart1,ptr,len);

	return len;
}

void RS485_TransmitOn(void)
{
	//HAL_UART_MspDeInit_UART7();
	MX_UART1_RX_DeInit();
	HAL_GPIO_WritePin(USART__R_E_GPIO_Port, USART__R_E_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(USART_DE_GPIO_Port, USART_DE_Pin, GPIO_PIN_SET);
	MX_UART1_TX_ReInit();

	for(int i = 0; i < NOP_WAIT_AMMOUNT; i++)
	{
		__asm("nop");
	}
	//__disable_irq();
}

void RS485_TransmitOff(void)
{
	//HAL_UART_MspDeInit_UART7();
	for(int i = 0; i < NOP_WAIT_AMMOUNT; i++)
	{
		__asm("nop");
	}
	MX_UART1_TX_DeInit();
	HAL_GPIO_WritePin(USART__R_E_GPIO_Port, USART__R_E_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(USART_DE_GPIO_Port, USART_DE_Pin, GPIO_PIN_RESET);
	for(uint16_t i=0; i<100; ++i)
	{
		uint32_t tmp;
		UNUSED(tmp);
		tmp = USART1->SR;
		tmp = USART1->DR;
	}

	MX_UART1_RX_ReInit();
}

void RS485_Clear(void)
{
	CyclicBuffer_flush(&_rxBuff);
}

bool RS485_GetByte(uint8_t *ch)
{
   if(CyclicBuffer_isNotEmpty(&_rxBuff))
   {
      *ch = CyclicBuffer_get(&_rxBuff);
      return true;
   }
   return false;
}

uint16_t RS485_BuferCount(void)
{
   return CyclicBuffer_avaiableSize(&_rxBuff);
}


void DataTimerDone(void)
{
	_timerRunning = false;
}

void FAST_UART_RX_Start(void)
{
	HAL_GPIO_WritePin(USART__R_E_GPIO_Port, USART__R_E_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(USART_DE_GPIO_Port, USART_DE_Pin, GPIO_PIN_RESET);


    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
}


static void FAST_UART_Receive_IT(UART_HandleTypeDef *huart)
{
	static uint32_t lastTime = 0;
    __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);
    __HAL_UART_DISABLE_IT(huart, UART_IT_PE);
    __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);
    __HAL_UART_CLEAR_OREFLAG(&huart1);

  	//tmp = huart1.Instance->SR;
  	tmp = huart1.Instance->DR;
	//CyclicBuffer_append(&_rxBuff, tmp);

	switch(Protocol)
	{
	case SLS:
		CyclicBuffer_append(&_rxBuff, tmp);
		break;
	case MODBUS:
		if (HAL_GetTick() - lastTime >= 5)
		{
			modbusRTU_BufferFlush();
		}
	    modbusRTU_getChar(tmp);
	    lastTime = HAL_GetTick();
		break;
	default:
		break;
	}

    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);

}



void USART1_IRQHandler(void)
{

	FAST_UART_Receive_IT(&huart1);

}


static void FastUartMspInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
}

static void RS485_SetConfig(UART_HandleTypeDef *huart)
{
  uint32_t tmpreg;

  MODIFY_REG(huart->Instance->CR2, USART_CR2_STOP, huart->Init.StopBits);

  tmpreg = (uint32_t)huart->Init.WordLength | huart->Init.Parity | huart->Init.Mode | huart->Init.OverSampling;
  MODIFY_REG(huart->Instance->CR1,
             (uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE | USART_CR1_OVER8),
             tmpreg);

  MODIFY_REG(huart->Instance->CR3, (USART_CR3_RTSE | USART_CR3_CTSE), huart->Init.HwFlowCtl);

  if (huart->Init.OverSampling == UART_OVERSAMPLING_8)
  {
    if ((huart->Instance == USART1) || (huart->Instance == USART6))
    {
      huart->Instance->BRR = UART_BRR_SAMPLING8(HAL_RCC_GetPCLK2Freq(), huart->Init.BaudRate);
    }
    else
    {
      huart->Instance->BRR = UART_BRR_SAMPLING8(HAL_RCC_GetPCLK1Freq(), huart->Init.BaudRate);
    }
  }
  else
  {
    if ((huart->Instance == USART1) || (huart->Instance == USART6))
    {
      huart->Instance->BRR = UART_BRR_SAMPLING16(HAL_RCC_GetPCLK2Freq(), huart->Init.BaudRate);
    }
    else
    {
      huart->Instance->BRR = UART_BRR_SAMPLING16(HAL_RCC_GetPCLK1Freq(), huart->Init.BaudRate);
    }
  }
}

void RS485_Init(ADI_rs485_speed spd)
{

	HAL_GPIO_WritePin(USART__R_E_GPIO_Port, USART__R_E_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(USART_DE_GPIO_Port, USART_DE_Pin, GPIO_PIN_RESET);
  _timerRunning = false;
  huart1.Instance = USART1;

  huart1.Init.BaudRate = Get_Speed(spd);
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;

  if (huart1.gState == HAL_UART_STATE_RESET)
  {
	huart1.Lock = HAL_UNLOCKED;
    FastUartMspInit();
  }

  huart1.gState = HAL_UART_STATE_BUSY;

  __HAL_UART_DISABLE(&huart1);

  RS485_SetConfig(&huart1);

  CLEAR_BIT(huart1.Instance->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
  CLEAR_BIT(huart1.Instance->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

  __HAL_UART_ENABLE(&huart1);

  huart1.ErrorCode = HAL_UART_ERROR_NONE;
  huart1.gState = HAL_UART_STATE_READY;
  huart1.RxState = HAL_UART_STATE_READY;
}
