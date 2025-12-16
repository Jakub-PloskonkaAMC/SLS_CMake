/*
 * otc_core.h
 *
 *  Created on: 13 sie 2020
 *      Author: mantoniak
 */

#ifndef OTC_OTC_CORE_H_
#define OTC_OTC_CORE_H_

#include "conf.h"

typedef bool (*send_resp_func) (uint8_t addrSensor, uint8_t command, uint8_t flags, const void * const data, uint16_t len);

void OTC_Body(uint8_t* data, uint16_t length);
void OTC_RegisterResponseHandler(send_resp_func handler);

#endif /* OTC_OTC_CORE_H_ */
