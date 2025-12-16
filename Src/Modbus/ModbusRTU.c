/*
 * ModbusRTU.c
 *
 *  Created on: Jul 4, 2022
 *      Author: midec
 */

#include "ModbusRTU.h"
#include "conf.h"
#include "ModbusRTU_crc.h"
#include "ModbusRTU_RingBuffer.h"

static volatile uint8_t timer = 0;
uint8_t message_data[255];
uint8_t modbus_address = 0x01;

modbus_message_t mess;

volatile uint8_t data_buffer[256];
volatile uint8_t data_out[256];
volatile uint8_t send_index;

enum modbus_state
{
	STATE_IDLE,
	STATE_MESSAGE_RECEIVING,
	STATE_MESSAGE_RECEIVED,
	STATE_MESSAGE_SENDING
};

static volatile int state = STATE_IDLE;

modbus_func_descriptor desc[] = {
	{.function_code = MODBUS_FUNCTION_READ_COILS, .data_length = 4, .callback = (func_callback)modbusRTUFCallbacks_readCoils},
	{.function_code = MODBUS_FUNCTION_READ_HOLDING_REGISTERS, .data_length = 4, .callback = (func_callback)modbusRTUFCallbacks_readRegisters},
	{.function_code = MODBUS_FUNCTION_WRITE_MULTIPLE_REGISTER, .data_length = 6, .callback = (func_callback)modbusRTUFCallbacks_writeRegisters},
	{.function_code = MODBUS_FUNCTION_WRITE_SINGLE_COIL, .data_length = 4, .callback = (func_callback)modbusRTUFCallbacks_writeCoil}};

modbus_coil_table_entry_t modbus_coils_table[] =
	{
		{.coil_address = 0, .coil_state = false},				   // empty
		{.coil_address = modbusACQ, .coil_state = false},		   // acq on/off
		{.coil_address = modbusSwapProtocol, .coil_state = false}, // protocol switch
		{.coil_address = modbusReset, .coil_state = false},		   // reset
		{.coil_address = modbusResetConf, .coil_state = false},	   // reset conf
		{.coil_address = modbusACQRMS, .coil_state = false},	   // start RMS calculate
		{.coil_address = modbusACQZP, .coil_state = false},		   // start Zero peak calculate
		{.coil_address = modbusACQVRMS, .coil_state = false},	   // start VRMS calculate
		{.coil_address = modbusACQGE, .coil_state = false},		   // start gE calculate
		{.coil_address = modbusACQDataAble, .coil_state = false},  // Are new data available
		{.coil_address = modbusACQMaxMin, .coil_state = false}	   // start max min  calculate
};

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void _modbusRTU_portSetRx();
void _modbusRTU_portSetTx();
static uint8_t _modbusRTU_getFuncDescIndex(uint8_t code, uint8_t *index);
static uint8_t _modbusRTU_checkFuncCode(uint8_t code);
static uint8_t _modbusRTU_processMessage();

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void _modbusRTU_portSetRx()
{
	RS485_TransmitOff();
}

void _modbusRTU_portSetTx(void)
{
	RS485_TransmitOn();
}

static uint8_t _modbusRTU_getFuncDescIndex(uint8_t code, uint8_t *index)
{
	for (uint32_t i = 0; i < sizeof(desc) / sizeof(modbus_func_descriptor); i++)
	{
		if (desc[i].function_code == code)
		{
			*index = i;
			return MODBUS_STATUS_OK;
		}
	}
	return MODBUS_STATUS_ERROR;
}

static uint8_t _modbusRTU_checkFuncCode(uint8_t code)
{
	for (uint32_t i = 0; i < sizeof(desc) / sizeof(modbus_func_descriptor); i++)
	{
		if (desc[i].function_code == code)
			return MODBUS_STATUS_OK;
	}
	return MODBUS_STATUS_ERROR;
}

static int message_index_rtu = 0;
static uint8_t _modbusRTU_processMessage()
{
	uint8_t message_completed = 0;
	mess.data = (uint8_t *)message_data;
//	memset(message_data, 0, 255);
	static uint16_t wCRCWord = 0xFFFF;
	static uint8_t nTemp;
	char c;
	static uint8_t index = 0;
	static char broadcast_id_mode = 0;
	static bool frameInProgress = false;
	static int ringBufferTail = -1;

	while (modbusRTURingBuffer_GetChar((ring_buffer_t *)&data_ring_buffer, &c) == MODBUS_STATUS_OK)
	{
//		PRINTF_MODBUS("Byte processed: %X, idx %d\n", c, message_index_rtu);
		// return MODBUS_STATUS_ERROR;

		switch (message_index_rtu)
		{
		case 0:
			if (c == modbus_address)
			{
				wCRCWord = 0xFFFF;
				mess.address = c;
				message_index_rtu++;
				ModbusRTUCRC(&nTemp, &wCRCWord, c);
				frameInProgress = true;
				broadcast_id_mode = 0;
				ringBufferTail = data_ring_buffer.tail;
			}
			else if (c == 0xFF)
			{
				wCRCWord = 0xFFFF;
				mess.address = c;
				message_index_rtu++;
				ModbusRTUCRC(&nTemp, &wCRCWord, c);

				broadcast_id_mode = 1;
				frameInProgress = true;
				ringBufferTail = data_ring_buffer.tail;
			}
			else
			{
//				PRINTF_MODBUS("Wrong addres: %d  proper address: %d.\n\r", c, modbus_address);
				message_index_rtu = 0;
				broadcast_id_mode = 0;
				modbusRTURingBuffer_Clear((ring_buffer_t *)&data_ring_buffer);
				RS485_Clear();
				state = STATE_IDLE;
				timer = 0;
				frameInProgress = false;
				return MODBUS_STATUS_ERROR;
			}

			break;

		case 1:
			mess.function = c;
			if (_modbusRTU_checkFuncCode(c) == MODBUS_STATUS_OK)
			{
				if (_modbusRTU_getFuncDescIndex(mess.function, &index) == MODBUS_STATUS_OK)
				{
					// if (modbusRTURingBuffer_GetLen((ring_buffer_t *)&data_ring_buffer) < desc[index].data_length + 2)
					// {
					// 	PRINTF_MODBUS("WRONG_LEN");
					// 	return MODBUS_STATUS_ERROR;
					// }
					// else
					{
						ModbusRTUCRC(&nTemp, &wCRCWord, c);
						mess.data_length = desc[index].data_length;
						message_index_rtu++;
						frameInProgress = true;
					}
				}
				else
				{
					frameInProgress = false;
				}
			}
			else
			{
				modbusRTURingBuffer_GoBack((ring_buffer_t *)&data_ring_buffer, ringBufferTail + 1);  //Maybe the address is good
//				message_index_rtu = 0;
//				modbusRTURingBuffer_Clear((ring_buffer_t *)&data_ring_buffer);
				mess.function += 0x80;
				mess.data[0] = 1;
				PRINTF_MODBUS("MODBUS_BAD_FUNC %d\n", c);
				frameInProgress = false;

				return MODBUS_STATUS_ERROR;
			}
			break;

		default:
			if (message_index_rtu == 6 && mess.function == 0x10)
			{
				desc[index].data_length = c + 5;
				frameInProgress = true;
			}

			if (_modbusRTU_getFuncDescIndex(mess.function, &index) == MODBUS_STATUS_OK)
			{
				if (message_index_rtu - 2 < desc[index].data_length)
				{
					ModbusRTUCRC(&nTemp, &wCRCWord, c);
					mess.data[message_index_rtu - 2] = c;
					message_index_rtu++;
					frameInProgress = true;
				}
				else
				{
					if (message_index_rtu == 2 + desc[index].data_length)
					{
						mess.error_check = c;
						message_index_rtu++;
						frameInProgress = true;
					}
					else if (message_index_rtu == 3 + desc[index].data_length)
					{
						mess.error_check += (c << 8);
						message_completed = 1;
						message_index_rtu++;
						desc[2].data_length = 6;
						frameInProgress = true;
					}
					else
					{
						frameInProgress = false;
					}
				}
			}
			else
			{
				frameInProgress = false;
			}

			break;
		}
	}
	if (message_completed)
	{
		if (wCRCWord != mess.error_check)
		{
			PRINTF_MODBUS("BAD CRC: Calc: 0x%04X  ModbusRecCRC: 0x%04X\n", wCRCWord, mess.error_check);
			frameInProgress = false;
			modbusRTURingBuffer_GoBack((ring_buffer_t *)&data_ring_buffer, ringBufferTail + 2); //skip bad address and function code
		}
		else if (broadcast_id_mode == 1)
		{
			PRINTF_MODBUS("PERFORMING_ID_BROADCAST\n");
			broadcast_id_mode = 0;
			uint16_t reg_add = ((mess.data[0] << 8) + mess.data[1]) * 2;
			uint16_t reg_nmb = ((mess.data[2] << 8) + mess.data[3]) * 2;

			if (mess.address == 0xFF && reg_add == 0 && reg_nmb == 2)
			{
				uint32_t delay = rand() % 1000;
				HAL_Delay(delay);
				desc[index].callback(&mess);
				frameInProgress = false;
			}
		}
		else
		{
			PRINTF_MODBUS("MODBUS_MESSAGE_COMPLETED\n");
			if (_modbusRTU_getFuncDescIndex(mess.function, &index) == MODBUS_STATUS_OK)
			{
				ringBufferTail = -1;
				desc[index].callback(&mess);
				frameInProgress = false;
			}
		}
	}
	if (state == STATE_MESSAGE_RECEIVED)
		state = STATE_IDLE;
	if (!frameInProgress)
	{
		broadcast_id_mode = 0;
		message_index_rtu = 0;
		wCRCWord = 0xFFFF;
	}
	return MODBUS_STATUS_OK;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void modbusRTU_loop(void)
{
	_modbusRTU_processMessage();
}

void modbusRTU_init(uint8_t address)
{
	data_ring_buffer.data = (uint8_t *)data_buffer;
	data_ring_buffer.head = 0;
	data_ring_buffer.tail = 0;
	data_ring_buffer.count = 0;
	data_ring_buffer.size = 256;

	// log_fprintf(UART_STREAM, "Init modbus with address: %d\n\r", address);

	modbus_address = address;
	_modbusRTU_portSetRx();
}

uint8_t modbusRTU_sendMessage(modbus_message_t *message)
{
	uint8_t nTemp;
	uint16_t wCRCWord = 0xFFFF;

	memset((void *)data_out, 0x00, sizeof(data_out));
	send_index = 0;

	data_out[send_index] = message->address;
	send_index++;
	ModbusRTUCRC(&nTemp, &wCRCWord, message->address);

	data_out[send_index] = message->function;
	send_index++;
	ModbusRTUCRC(&nTemp, &wCRCWord, message->function);

	PRINTF_MODBUS("Sending the frame. Data length: %d\n", message->data_length);

	if (message->function & 0x80) // error occurs
	{
		data_out[send_index] = message->data[0];
		send_index++;
		ModbusRTUCRC(&nTemp, &wCRCWord, message->data[0]);
	}
	else
	{
		for (int i = 0; i < message->data_length; i++)
		{
			data_out[send_index] = message->data[i];
			send_index++;
			ModbusRTUCRC(&nTemp, &wCRCWord, message->data[i]);
		}
	}
	data_out[send_index] = (uint8_t)wCRCWord;
	send_index++;
	data_out[send_index] = (uint8_t)(wCRCWord >> 8);
	send_index++;

	_modbusRTU_portSetTx();
	HAL_Delay(5);
	RS485_Send((void *)data_out, send_index);
	HAL_Delay(5);
	_modbusRTU_portSetRx();

	send_index = 0;
	return MODBUS_STATUS_OK;
}

void modbusRTU_getChar(char c)
{
	// PRINTF_MODBUS("MODBUS char %02X",c);
	switch (state)
	{
	case STATE_IDLE:
		if (timer == 0)
		{
			modbusRTURingBuffer_PutChar((ring_buffer_t *)&data_ring_buffer, c);
			timer = 7;
			state = STATE_MESSAGE_RECEIVING;
		}
		else
		{
			modbusRTURingBuffer_Clear((ring_buffer_t *)&data_ring_buffer);
			timer = 0;
		}
		break;

	case STATE_MESSAGE_RECEIVING:
		if (timer > 0)
		{
			modbusRTURingBuffer_PutChar((ring_buffer_t *)&data_ring_buffer, c);
			timer = 7;
		}
		else if (timer == 0)
		{
			timer = 4;
			state = STATE_IDLE;
		}

		break;

	case STATE_MESSAGE_RECEIVED:

		break;
	}
}

void modbusRTU_BufferFlush()
{
	message_index_rtu = 0;
	modbusRTURingBuffer_Clear((ring_buffer_t *)&data_ring_buffer);
}
