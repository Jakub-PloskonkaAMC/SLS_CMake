#include "spi.h"
#include "conf.h"
#include "gpio.h"
#include "ad7768.h"
#include "acquisition_hf.h"

SPI_HandleTypeDef hspi1;
extern uint8_t sample[3];

void SPI1_Init(void)
{
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
#ifdef SENSOR_TYPE_HF
	hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;//4
#else
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;//4
#endif
	hspi1.Init.NSS = SPI_NSS_SOFT;

	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		Error_Handler();
	}
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(spiHandle->Instance==SPI1)
	{
		__HAL_RCC_SPI1_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**SPI1 GPIO Configuration
    	PA5     ------> SPI1_SCK
    	PA6     ------> SPI1_MISO
    	PA7     ------> SPI1_MOSI
		 */
		GPIO_InitStruct.Pin = SCLK_ADXL_Pin|MISO_ADXL_Pin|MOSI_ADXL_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		HAL_NVIC_SetPriority(SPI1_IRQn, 1, 1);
		HAL_NVIC_EnableIRQ(SPI1_IRQn);
	}
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{
	if(spiHandle->Instance==SPI1)
	{
		__HAL_RCC_SPI1_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOA, SCLK_ADXL_Pin|MISO_ADXL_Pin|MOSI_ADXL_Pin);
	}
} 


void SPI1_IRQHandler(void)
{
	HAL_SPI_IRQHandler(&hspi1);
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	uint8_t data[3];
	ad7768_set_cs();
	data[0] = sample[2];
	data[1] = sample[1];
	data[2] = sample[0];
	ACQ_SampleAppend(data);
	LED_green_Off();
}

static void SPI_2linesRxISR_8BIT_Lightsome(struct __SPI_HandleTypeDef *hspi);
static void SPI_2linesTxISR_8BIT_Lightsome(struct __SPI_HandleTypeDef *hspi);

HAL_StatusTypeDef HAL_SPI_TransmitReceive_IT_Lightsome(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
{
  uint32_t             tmp_mode;
  HAL_SPI_StateTypeDef tmp_state;
  HAL_StatusTypeDef    errorcode = HAL_OK;


  /* Process locked */
  __HAL_LOCK(hspi);

  /* Init temporary variables */
  tmp_state           = hspi->State;
  tmp_mode            = hspi->Init.Mode;

  if (!((tmp_state == HAL_SPI_STATE_READY) || \
        ((tmp_mode == SPI_MODE_MASTER) && (hspi->Init.Direction == SPI_DIRECTION_2LINES) && (tmp_state == HAL_SPI_STATE_BUSY_RX))))
  {
    errorcode = HAL_BUSY;
    goto error;
  }

  if ((pTxData == NULL) || (pRxData == NULL) || (Size == 0U))
  {
    errorcode = HAL_ERROR;
    goto error;
  }

  /* Don't overwrite in case of HAL_SPI_STATE_BUSY_RX */
  if (hspi->State != HAL_SPI_STATE_BUSY_RX)
  {
    hspi->State = HAL_SPI_STATE_BUSY_TX_RX;
  }

  /* Set the transaction information */
  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi->pTxBuffPtr  = (uint8_t *)pTxData;
  hspi->TxXferSize  = Size;
  hspi->TxXferCount = Size;
  hspi->pRxBuffPtr  = (uint8_t *)pRxData;
  hspi->RxXferSize  = Size;
  hspi->RxXferCount = Size;


  hspi->RxISR     = SPI_2linesRxISR_8BIT_Lightsome;
  hspi->TxISR     = SPI_2linesTxISR_8BIT_Lightsome;


  /* Enable TXE, RXNE and ERR interrupt */
  __HAL_SPI_ENABLE_IT(hspi, (SPI_IT_TXE | SPI_IT_RXNE | SPI_IT_ERR));

  /* Check if the SPI is already enabled */
  if ((hspi->Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE)
  {
    /* Enable SPI peripheral */
    __HAL_SPI_ENABLE(hspi);
  }

error :
  /* Process Unlocked */
  __HAL_UNLOCK(hspi);
  return errorcode;
}

static void SPI_CloseRxTx_ISR_Lightsome(SPI_HandleTypeDef *hspi)
{
    __IO uint32_t count = 1000000;
    /* Disable ERR interrupt */
    __HAL_SPI_DISABLE_IT(hspi, SPI_IT_ERR);

    /* Wait until TXE flag is set */
    do
    {
        if (count == 0U)
        {
            SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
            break;
        }
        count--;
    }
    while ((hspi->Instance->SR & SPI_FLAG_TXE) == RESET);



    /* Clear overrun flag in 2 Lines communication mode because received is not read */
    if (hspi->Init.Direction == SPI_DIRECTION_2LINES)
    {
        __HAL_SPI_CLEAR_OVRFLAG(hspi);
    }


    if (hspi->ErrorCode == HAL_SPI_ERROR_NONE)
    {
        if (hspi->State == HAL_SPI_STATE_BUSY_RX)
        {
            hspi->State = HAL_SPI_STATE_READY;
            /* Call user Rx complete callback */

            HAL_SPI_RxCpltCallback(hspi);

        }
        else
        {
            hspi->State = HAL_SPI_STATE_READY;
            /* Call user TxRx complete callback */

            HAL_SPI_TxRxCpltCallback(hspi);

        }
    }
    else
    {
        hspi->State = HAL_SPI_STATE_READY;
        /* Call user error callback */

        HAL_SPI_ErrorCallback(hspi);

    }

}

static void SPI_2linesRxISR_8BIT_Lightsome(struct __SPI_HandleTypeDef *hspi)
{
  /* Receive data in 8bit mode */
  *hspi->pRxBuffPtr = *((__IO uint8_t *)&hspi->Instance->DR);
  hspi->pRxBuffPtr++;
  hspi->RxXferCount--;

  /* Check end of the reception */
  if (hspi->RxXferCount == 0U)
  {
    /* Disable RXNE  and ERR interrupt */
    __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_RXNE | SPI_IT_ERR));

    if (hspi->TxXferCount == 0U)
    {
    	SPI_CloseRxTx_ISR_Lightsome(hspi);
    }
  }
}



static void SPI_2linesTxISR_8BIT_Lightsome(struct __SPI_HandleTypeDef *hspi)
{
  *(__IO uint8_t *)&hspi->Instance->DR = (*hspi->pTxBuffPtr);
  hspi->pTxBuffPtr++;
  hspi->TxXferCount--;

  /* Check the end of the transmission */
  if (hspi->TxXferCount == 0U)
  {
    /* Disable TXE interrupt */
    __HAL_SPI_DISABLE_IT(hspi, SPI_IT_TXE);

    if (hspi->RxXferCount == 0U)
    {
    	SPI_CloseRxTx_ISR_Lightsome(hspi);
    }
  }
}
