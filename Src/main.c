#include "main.h"
#include "debug.h"
#include "rs485.h"
#include "timers.h"
#include "board.h"
#include "gpio.h"
#include "comm_app.h"
#include "comm_link.h"
#include "spi.h"
#include "acquisition.h"
#include "memory.h"
#include "temp.h"
#include "ModbusRTU.h"
#include "ad7768.h"
#include "adxl.h"
#include "estimates_calc.h"

CurrentProtocol Protocol = SLS;

float* collectedData = NULL;
char calculationsInterrupt = 0;

int main(void)
{
    SCB->VTOR = 0x8020000;
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();
    LOG_Init();
    ADC_Temp_Init();
    TIMER_Sleep_Init();
    TIMER_AcqSample_Init();
    TIMER_TempSample_Init();
    PRINTF_HW("\n\n");
    PRINTF_HW("Software version: %02d:%02d:%02d\n", SOFTWARE_MAJOR, SOFTWARE_MINOR, SOFTWARE_PATCH);
    PRINTF_HW("Hardware version: %02d:%02d:%02d\n", HARDWARE_MAJOR, HARDWARE_MINOR, HARDWARE_REVISION);

#if defined SENSOR_TYPE_HF
    PRINTF_HW("Sensor type: HF\n\n\n");
#elif defined SENSOR_TYPE_LF
    PRINTF_HW("Sensor type: LF\n\n\n");
#endif

    SPI1_Init();
    Memory_Init();
#ifdef SENSOR_TYPE_HF
    ad7768_Init();
#elif defined SENSOR_TYPE_LF
    adxl_Init();
#endif

    AdiConf_t fromMemoryConfig;

    Memory_TakeConfig(&fromMemoryConfig);
    Memory_ShowConfig(&fromMemoryConfig);

    Sensor.configuration.sensor_ID = fromMemoryConfig.sensor_ID;
    memcpy(Sensor.configuration.MAC, fromMemoryConfig.MAC, 12);
    Sensor.configuration.RS485_speed = fromMemoryConfig.RS485_speed;
    Protocol = fromMemoryConfig.protocol;

    ACQ_Init();

    PRINTF_APP("Odczytano id po zapisie\r\n");
    Memory_ShowConfig(&fromMemoryConfig);

    RS485_Init(Sensor.configuration.RS485_speed);
    FAST_UART_RX_Start();
    CommLink_Init();
    PRINTF_HW("Init ok\n");

    TimerTemperature_Start();

    modbusRTU_init(fromMemoryConfig.sensor_ID);
    PRINTF_HW("Init ok\n");
    PRINTF_APP("Protocol : %s \n", enum2str(Protocol));

    ACQ_Start();

    while (1)
    {
        LED_red_Toggle();
        switch (Protocol)
        {
        case SLS:
            Comm_Process();
            break;

        case MODBUS:
            modbusRTU_loop();
            if (calculationsInterrupt == 1)
            {
                calculateEstimates(collectedData);
                calculationsInterrupt = 0;
            }
            break;

        default:
            break;
        }
    }
}
