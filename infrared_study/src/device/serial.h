/*
 * serial.h
 *
 *  Created on: 2013年8月26日
 *      Author: flx
 */

#ifndef __SERIAL_H__
#define __SERIAL_H__

extern int serial_open();
extern int serial_close();
extern int serial_read(unsigned char *buffer, int size);
extern int serial_write(const unsigned char *buffer, int data_len);
extern int restore_console();
extern void serial_log(const char *format, ...);

#endif /* __SERIAL_H__ */
