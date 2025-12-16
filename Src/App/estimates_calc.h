#ifndef ESTIMATES_CALC
#define ESTIMATES_CALC

#include "conf.h"

#if defined SENSOR_TYPE_LF

/**
 * x, y, z - Axis
 * PP - Acceleration Peak-to-Peak estimate value
 * RMS - Acceleration Root Mean Square estimate value
 * VRMS - Velocity Root Mean Square estimate value
 */
typedef struct
{
	float x_PP;
	float x_RMS;
	float x_VRMS;
	float y_PP;
	float y_RMS;
	float y_VRMS;
	float z_PP;
	float z_RMS;
	float z_VRMS;
} calculated_estimates_t;

#elif defined SENSOR_TYPE_HF

/**
 * PP - Acceleration Peak-to-Peak estimate value
 * RMS - Acceleration Root Mean Square estimate value
 * VRMS - Velocity Root Mean Square estimate value
 * envZP - Envelope Zero Peak estimate value
 * envelopeRMS - Envelope Root Mean Square estimate value
 */
typedef struct
{
	float PP;
	float RMS;
	float VRMS;
	float envZP;
	float envRMS;
} calculated_estimates_t;

#endif // SENSOR_TYPE_HF

/**
 * @brief Calculate the values of PP, RMS and VRMS (plus envelope ZP and envelope RMS [HF]), and put them in the "estimates" structure
 * @param Pointer to the array of 32000 [HF] or 4000 [LF] floats, used to calculate the estimates value
 */
void calculateEstimates(float* collectedData);

/**
 * @brief Obtain the latest values of the calculated estimates
 * @return Structure with the calculated estimates
 */
calculated_estimates_t getEstimates(void);

#endif
