#include "estimates_calc.h"

static calculated_estimates_t estimates;

calculated_estimates_t getEstimates(void)
{
	return estimates;
}

#if defined SENSOR_TYPE_LF
#include "../Estimates_Lib/FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s.h"
#define MODBUS_SAMPLES_AMOUNT 4000

void calculateEstimates(float* collectedData)
{
	calculated_estimates_t newEstimates;
	FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s(collectedData,
		&newEstimates.x_PP, &newEstimates.x_RMS, &newEstimates.x_VRMS);

	FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s(collectedData + MODBUS_SAMPLES_AMOUNT,
		&newEstimates.y_PP, &newEstimates.y_RMS, &newEstimates.y_VRMS);

	FD_PP_RMS_VRMS10_fs4000_nFFF4096_T1s(collectedData + (2 * MODBUS_SAMPLES_AMOUNT),
		&newEstimates.z_PP, &newEstimates.z_RMS, &newEstimates.z_VRMS);

	memcpy(&estimates, &newEstimates, sizeof(calculated_estimates_t));
}

#elif defined SENSOR_TYPE_HF
#include "../Estimates_Lib/FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s.h"

void calculateEstimates(float* collectedData)
{
	calculated_estimates_t newEstimates;
	FD_PP_RMS_VRMS10_envZP_envRMS_fs32000_nFFF4096_T1s(collectedData,
		&newEstimates.PP, &newEstimates.RMS, &newEstimates.VRMS, &newEstimates.envZP, &newEstimates.envRMS);
	memcpy(&estimates, &newEstimates, sizeof(calculated_estimates_t));
}

#endif // SENSOR_TYPE_HF
