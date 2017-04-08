/*
 * log.h
 *
 *  Created on: 2013年8月26日
 *      Author: flx
 */

#ifndef LOG_H_
#define LOG_H_

#include <syslog.h>
#include "config.h"

#ifdef DEBUG_IN_JIKONG
#define to_log(fmt, args...) \
		syslog(LOG_INFO, fmt, ##args);
#else
#define to_log(fmt, args...)
#endif

extern void to_log_byte_array(const unsigned char *buffer, int data_len);

#endif /* LOG_H_ */
