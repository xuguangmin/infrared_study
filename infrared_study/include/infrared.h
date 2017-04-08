/*
 * infrared.h
 *
 *  Created on: 2013年8月26日
 *      Author: flx
 */

#ifndef __INFRARED_H__
#define __INFRARED_H__

extern int infrared_open();
extern int infrared_close();
extern int infrared_write(const unsigned char *buffer, int data_len);
extern int infrared_read(unsigned char *buffer, int size);

#endif /* __INFRARED_H__ */
