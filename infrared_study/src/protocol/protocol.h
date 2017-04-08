#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "protocol_define.h"

/* protocol data unit */
typedef struct __PDU
{
#define PROTO_DATA_LEN_MAX     4096

	unsigned char cmd;
	unsigned char data[PROTO_DATA_LEN_MAX];
	int           data_len;
	unsigned char checksum1;
	unsigned char checksum2;
	int           b_enable;
}PDU;


extern int parse_packet(const unsigned char *in_buffer, int data_len, PDU *lpPdu);
extern unsigned create_protocol_packet(unsigned char cmd, const unsigned char *data, unsigned int data_len,
                                       unsigned char *out_buffer, unsigned int buffer_size);

#endif  /* __PROTOCOL_H__ */
