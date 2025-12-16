
#ifndef ACQUISITION_H_
#define ACQUISITION_H_

#include "conf.h"

#ifdef SENSOR_TYPE_LF
#include "acquisition_lf.h"
#elif defined SENSOR_TYPE_HF
#include "acquisition_hf.h"
#else
#error  Zdefinuij czujnik
#endif


#endif /* ACQUISITION_H_ */
