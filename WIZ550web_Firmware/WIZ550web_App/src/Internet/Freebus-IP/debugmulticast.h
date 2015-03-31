/*
 * debugmulticast.h
 *
 *  Created on: 15.03.2015
 *      Author: tw
 */

#ifndef DEBUGMULTICAST_H_
#define DEBUGMULTICAST_H_

#define DEBUG_MULTICAST_PORT      	1234	      ///< DHCP server port number

// comment to disable debug messages via multicasts
#define _DEBUG_MULTICAST_

#include <stdint.h>

void DEBUG_MULTICAST_init(uint8_t s);

void DEBUGMULTICAST_run(void);

void send_debugmulticast_data(const char *buffer);
void send_debugmulticast_buf(char *buffer, uint16_t len);

#endif /* DEBUGMULTICAST_H_ */
