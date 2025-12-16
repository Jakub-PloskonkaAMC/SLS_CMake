/*
 * acquisition_lf.c
 *
 *  Created on: 11.07.2019
 *      Author: mantoniak
 */

#include "conf.h"

#ifdef SENSOR_TYPE_LF

#include "acquisition.h"
#include "timers.h"
#include "gpio.h"
#include "cyclicBuffer.h"
#include "adxl.h"
#include "board.h"
#include "debug.h"
#include "comm_app.h"
#include "rs485.h"

/*! ----------------------------------------------------------------------------
* Variable
------------------------------------------------------------------------------*/
#define CHANNEL_MAX_SAMPLES 10700
#define MODBUS_SAMPLES_AMOUNT 4000
#define CHANNELS_AMMOUNT 3

#define DATA_BUFFER_LENGTH CHANNELS_AMMOUNT * CHANNEL_MAX_SAMPLES

#define SAMPLE_SIZE sizeof(float)
#define ACCELEROMETER_SAMPLE_SIZE CHANNELS_AMMOUNT * 3

//#define TEST_SAMPLING

typedef struct
{
    uint32_t start;
    uint32_t end;
    uint32_t size;
    uint8_t data[SAMPLE_SIZE * DATA_BUFFER_LENGTH];
    bool overFlw;
} DataStorage_t;

static uint32_t _samplesCounter;
static DataStorage_t dataStorage;
static uint8_t dataResolution;

static float x_coefficientA;
static float x_coefficientB;
static float x_sensitivity;
static float y_coefficientA;
static float y_coefficientB;
static float y_sensitivity;
static float z_coefficientA;
static float z_coefficientB;
static float z_sensitivity;

extern char calculationsInterrupt;
extern float* collectedData;

/*! ----------------------------------------------------------------------------
* Functions
------------------------------------------------------------------------------*/



/******************************************************************************/
uint8_t ACQ_getRandom(void)
{
    //let's roll the dices
    return (uint8_t)((adxl_GetAxisData(DataX) >> 4));
}

/******************************************************************************/
void ACQ_setOverFlow(void)
{
    Sensor.overflow = true;
    dataStorage.overFlw = false;
    dataStorage.start = 0;
    dataStorage.end = 0;
    _samplesCounter = 0;
    Sensor.aqusition=false;
    Sensor.sample_counter = 0;
#ifdef ABLE_TO_SCHEDULE_SENDING
    ResetScheduledSending();
#endif
}
/******************************************************************************/
void ACQ_Init(void)
{
    if (DATA_BUFFER_LENGTH % 3 != 0)
    {
        PRINTF_HW("DATA_BUFFER_LENGTH is not divided by 3\n");
        while (1) { ; }
    }

    if (Protocol == MODBUS)
    {
        dataResolution = 4;
//        Sensor.configuration.RS485_speed = speed_115k;
    }
    else
    {
        dataResolution = 3;
        Sensor.configuration.RS485_speed = speed_1500k;
    }
    RS485_Init(Sensor.configuration.RS485_speed);
    AdiConf_t Config;
	Memory_TakeConfig(&Config);
    memcpy(&x_coefficientA, &Config.X_Calibration_A, sizeof(float));
    memcpy(&x_coefficientB, &Config.X_Calibration_B, sizeof(float));
    memcpy(&x_sensitivity, &Config.X_Calibration_Sensitivity, sizeof(float));
    memcpy(&y_coefficientA, &Config.Y_Calibration_A, sizeof(float));
    memcpy(&y_coefficientB, &Config.Y_Calibration_B, sizeof(float));
    memcpy(&y_sensitivity, &Config.Y_Calibration_Sensitivity, sizeof(float));
    memcpy(&z_coefficientA, &Config.Z_Calibration_A, sizeof(float));
    memcpy(&z_coefficientB, &Config.Z_Calibration_B, sizeof(float));
    memcpy(&z_sensitivity, &Config.Z_Calibration_Sensitivity, sizeof(float));
//	Config.RS485_speed = Sensor.configuration.RS485_speed;
//	Config.protocol = SLS;
//	Memory_SaveConfig(&Config); ///!!!!!

    dataStorage.overFlw = false;
    dataStorage.start = 0;
    dataStorage.end = 0;
    _samplesCounter = 0;
    dataStorage.size = dataResolution * DATA_BUFFER_LENGTH;
    Sensor.sample_counter = 0;
    memset(dataStorage.data, 0, SAMPLE_SIZE * DATA_BUFFER_LENGTH);

    HAL_NVIC_SetPriority(EXTI3_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
}
/******************************************************************************/
void ACQ_DoJob(void)
{
    /*if (!Sensor.aqusition)
    {
        return;
    }*/
#ifndef TEST_SAMPLING
    uint8_t sample[ACCELEROMETER_SAMPLE_SIZE] = { 0 };
    adxl_GetAxisData_fast(sample);
    ACQ_SampleAppend(sample);
#else
    uint8_t tempData[CHANNELS_AMOUNT * (SAMPLE_SIZE - 1)] = { 0 };
    memcpy(tempData, &lastSampleX, 3);
    memcpy(tempData + 3, &lastSampleY, 3);
    memcpy(tempData + 6, &lastSampleZ, 3);
    ACQ_SampleAppend(tempData);
    lastSampleX += 10;
    lastSampleY += 10;
    lastSampleZ += 10;
    if (lastSampleX > 50000)
    {
        lastSampleX = 1;
    }
    if (lastSampleY > 50000)
    {
        lastSampleY = 2;
    }
    if (lastSampleZ > 50000)
    {
        lastSampleZ = 3;
    }
#endif
}
/******************************************************************************/
uint8_t ACQ_adxl_ST(void)
{
    return adxl_GetID();
}

/******************************************************************************/
void ACQ_Start(void)
{
    //CyclicBuffer_flush(&_acqBuff);
    dataStorage.overFlw = false;
    Sensor.aqusition = true;
    Sensor.overflow = false;
    dataStorage.start = 0;
    dataStorage.end = 0;
    Sensor.sample_counter = 0;
    _samplesCounter = 0;
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
    //HAL_TIM_Base_Start_IT(&htim7);
}

/******************************************************************************/
void ACQ_Stop(void)
{
    //dataStorage.start = 0;
    //dataStorage.end = 0;
    PRINTF_APP("========================\n");
    PRINTF_APP("CLEARING SAMPLE COUNTER\n");
    PRINTF_APP("========================\n");
    //Sensor.sample_counter = 0;
    Sensor.aqusition=false;
    //_samplesCounter = 0;
    HAL_NVIC_DisableIRQ(EXTI3_IRQn);
    //HAL_TIM_Base_Stop_IT(&htim7);
}

/******************************************************************************/
void ACQ_SampleAppend(uint8_t* sample)
{
    if (Protocol == MODBUS)
    {
        // Sample => 9 left alligned data bytes (3 for X, 3 for Y, 3 for Z), last three bytes of the sample are '0'

        uint8_t* AccPtr = NULL;
        int32_t xAccInt = 0, yAccInt = 0, zAccInt = 0;
        float v = 0;

        float scalling = 1 << ((1 >> 3) & 15); // 1
        float range = 1;
        float gain = 1;
        int resolution = 20;

        // -----------------------------------------------------------------------
        // ---------------------------------- X ----------------------------------
        // -----------------------------------------------------------------------
        AccPtr = (uint8_t*)(&xAccInt);
        AccPtr[0] = sample[0];
        AccPtr[1] = sample[1];
        AccPtr[2] = sample[2];
        AccPtr[3] = 0;

        // Extend sign
        if (xAccInt & 0x00800000)
        {
            xAccInt |= 0xff000000;
        }

        // Scale
        xAccInt *= scalling;

        // Calibrate
        v = (double)(xAccInt) * x_coefficientA + x_coefficientB;
        if (v > ((1 << 23) - 1))
        {
            v = ((1 << 23) - 1);
        }
        if (v < (-(1 << 23)))
        {
            v = (-(1 << 23));
        }

        // Cast back
        xAccInt = (int32_t)(v);

        // -----------------------------------------------------------------------
        // ---------------------------------- Y ----------------------------------
        // -----------------------------------------------------------------------
        AccPtr = (uint8_t*)(&yAccInt);
        AccPtr[0] = sample[3];
        AccPtr[1] = sample[4];
        AccPtr[2] = sample[5];
        AccPtr[3] = 0;

        if (yAccInt & 0x00800000)
        {
            yAccInt |= 0xff000000;
        }

        yAccInt *= scalling;

        v = (double)(yAccInt) * y_coefficientA + y_coefficientB;
        if (v > ((1 << 23) - 1))
        {
            v = ((1 << 23) - 1);
        }
        if (v < (-(1 << 23)))
        {
            v = (-(1 << 23));
        }

        yAccInt = (int32_t)(v);

        // -----------------------------------------------------------------------
        // ---------------------------------- Z ----------------------------------
        // -----------------------------------------------------------------------
        AccPtr = (uint8_t*)(&zAccInt);
        AccPtr[0] = sample[6];
        AccPtr[1] = sample[7];
        AccPtr[2] = sample[8];
        AccPtr[3] = 0;

        if (zAccInt & 0x00800000)
        {
            zAccInt |= 0xff000000;
        }

        zAccInt *= scalling;

        v = (double)(zAccInt) * z_coefficientA + z_coefficientB;
        if (v > ((1 << 23) - 1))
        {
            v = ((1 << 23) - 1);
        }
        if (v < (-(1 << 23)))
        {
            v = (-(1 << 23));
        }

        zAccInt = (int32_t)(v);

        // ------------------------------------------------------------------------
        // From document: T1025 ADI QuickStart v0.5
        // ------------------------------------------------------------------------
        
        // Since sensitivity may (hypothetically) differ from axis to axis, coeff is calculated without it
        float ns_coeff = range / gain / (float)(1 << (resolution - 1));

        // Change from Accelerometer's X, Y, Z to AVS 1003's X, Y, Z and scalle the values to [g]
        float xResultingFloat = (float)zAccInt * ns_coeff / x_sensitivity;
        float yResultingFloat = (float)xAccInt * ns_coeff / y_sensitivity * -1; // -1 -> Axis inversion to match AVS orientation
        float zResultingFloat = (float)yAccInt * ns_coeff / z_sensitivity;

        // Put the values in the dataStorage
        memcpy(dataStorage.data + (dataStorage.end / CHANNELS_AMMOUNT),                                                   &xResultingFloat, dataResolution);
        memcpy(dataStorage.data + (dataStorage.end / CHANNELS_AMMOUNT) + (MODBUS_SAMPLES_AMOUNT * dataResolution),        &yResultingFloat, dataResolution);
        memcpy(dataStorage.data + (dataStorage.end / CHANNELS_AMMOUNT) + (2 * MODBUS_SAMPLES_AMOUNT * dataResolution),    &zResultingFloat, dataResolution);
    }
    else
    {
        // Z Y X zmiana kolejnosci danych
        memcpy(dataStorage.data + dataStorage.end,                          sample + dataResolution,        dataResolution);
        memcpy(dataStorage.data + dataStorage.end + dataResolution,         sample,                         dataResolution);
        memcpy(dataStorage.data + dataStorage.end + (2 * dataResolution),   sample + (2 * dataResolution),  dataResolution);
    }

    dataStorage.end += (CHANNELS_AMMOUNT * dataResolution);
    _samplesCounter += CHANNELS_AMMOUNT;
    Sensor.sample_counter += CHANNELS_AMMOUNT;

    if (Protocol == MODBUS)
    {
        if (dataStorage.end == CHANNELS_AMMOUNT * MODBUS_SAMPLES_AMOUNT * dataResolution)
        {
            dataStorage.end = 0;
        }
    }
    else
    {
        if (dataStorage.end == DATA_BUFFER_LENGTH * dataResolution)
        {
            dataStorage.end = 0;
        }
    }

    if (Sensor.sample_counter == Sensor.number_of_samples)
    {
        PRINTF_TEST("ACQ done samples collected: %d\n",Sensor.sample_counter);
        if (Protocol == SLS)
        {
            Sensor.aqusition = false;
            ACQ_Stop();
        }
    }
    if (dataStorage.end == dataStorage.start)
    {
        PRINTF_TEST("Overflow occur XX\n");
        if (Protocol == MODBUS)
        {
            collectedData = (float*)dataStorage.data;
            calculationsInterrupt = 1;
        }
        ACQ_setOverFlow();
        //ACQ_Stop();
    }
}
/******************************************************************************/
uint32_t ACQ_CheckForAbleSamples(void)
{
    return _samplesCounter;
}
/******************************************************************************/
uint32_t ACQ_GetSamples(uint8_t* data, uint32_t samplesAmmount)
{
    uint32_t samplesToTake = samplesAmmount;
    PRINTF_TEST("Samples requested: %d\n",samplesToTake);
    PRINTF_TEST("Samples in buffers: %d\n",Sensor.sample_counter);
    if (samplesToTake > _samplesCounter)
    {
        samplesToTake = _samplesCounter;
        uint8_t reszta = samplesToTake % CHANNELS_AMMOUNT;
        if (reszta!=0)
        {
            samplesToTake-=reszta;
            PRINTF_TEST("Reszta nie 0...\n");
            ACQ_setOverFlow();
            ACQ_Stop();
            return 0;
        }
    }
    _samplesCounter -= samplesToTake;
    samplesToTake = dataResolution * samplesToTake; // teraz juz bytes to take
    PRINTF_TEST("Bytes to take: %d\n",samplesToTake);
    if (dataStorage.start + samplesToTake < dataStorage.size)
    {
        /*if (dataStorage.start + samplesToTake > dataStorage.end && Sensor.aqusition)
        {
            PRINTF_APP("Jakas dziwna sytuacja...\n");
            ACQ_setOverFlow();
            ACQ_Stop();
            ACQ_Init();
            return 0;
        }*/
        memcpy(data,dataStorage.data+dataStorage.start,samplesToTake);
        PRINTF_TEST("Samples form %d to %d taken\n",dataStorage.start/3,(dataStorage.start+samplesToTake)/3);
        PRINTF_TEST("Bytes form %d to %d taken\n",dataStorage.start,dataStorage.start+samplesToTake);
        dataStorage.start += samplesToTake;
        if (dataStorage.start == 3*DATA_BUFFER_LENGTH)
        {
            dataStorage.start = 0;
        }
        else if (dataStorage.start > 3*DATA_BUFFER_LENGTH)
        {
            PRINTF_TEST("Jakas dziwna sytuacja...1\n");
            ACQ_setOverFlow();
            ACQ_Stop();
        }
        return samplesToTake;
    }

    // KOPIOWANIE NA DWA RAZY
    uint16_t firstPart = dataStorage.size - dataStorage.start;
    uint16_t secondPart = samplesToTake - firstPart;

    //FIRST COPYING
    memcpy(data, dataStorage.data + dataStorage.start, firstPart);
    dataStorage.start += firstPart;
    if (dataStorage.start == dataResolution * DATA_BUFFER_LENGTH)
    {
        dataStorage.start = 0;
    }
    else
    {
        PRINTF_TEST("Jakas dziwna sytuacja...2\n");
        ACQ_setOverFlow();
        ACQ_Stop();
        return 0;
    }
    //SECOND COPYING
    if (dataStorage.start + secondPart > dataStorage.end && Sensor.aqusition)
    {
        PRINTF_TEST("Jakas dziwna sytuacja...3\n");
        ACQ_setOverFlow();
        ACQ_Stop();
        return 0;
    }
    memcpy(data + firstPart, dataStorage.data, secondPart);
    dataStorage.start += secondPart;
    PRINTF_TEST("Samples second: %d to %d taken\n", 0, secondPart);
    PRINTF_TEST("Samples first: %d to %d taken\n", dataStorage.start, dataStorage.size);
    return samplesToTake;
}
/******************************************************************************/
void ACQ_clearOverFlow(void)
{
    Sensor.overflow = false;
}

#ifdef TEST_SAMPLING
int lastSampleX = 1;
int lastSampleY = 2;
int lastSampleZ = 3;
#endif

#endif
