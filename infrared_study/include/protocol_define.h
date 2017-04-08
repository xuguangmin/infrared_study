/*
 * protocol_define.h
 *
 *  Created on: 2013-05-13
 *      Author: flx
 */

#ifndef __PROTOCOL_DEFINE_H__
#define __PROTOCOL_DEFINE_H__

/* protocol command set */
#define PCS_INVALID                     0x00
#define PCS_IRDA_INITIALIZE             0x01
#define PCS_START_STUDY                 0x02
#define PCS_END_STUDY                   0x03
#define PCS_TEST_IRDA_DATA              0x04
#define PCS_IRDA_CLOSE                  0x05

/* protocol command set extend */
#define PCS_EX_OK 		                0x00
#define PCS_EX_ERR		                0x01

#define PROTOCOL_DATA_LEN_MAX           1024

#endif /* __PROTOCOL_DEFINE_H__ */
