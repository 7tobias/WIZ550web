/*
 * freebus.h
 *
 *  Created on: 11.03.2015
 *      Author: tw
 */

#ifndef FREEBUS_H_
#define FREEBUS_H_



#define _FREEBUS_DEBUG_



#define FREEBUS_SERVER_PORT      	3671	      ///< DHCP server port number

#include "Ethernet/socket.h"


void FREEBUS_init(uint8_t s);

void send_FREEBUS_Test(void);
void send_FREEBUS(uint8_t *message, uint8_t len);

void FREEBUS_run(void);

#endif /* FREEBUS_H_ */
