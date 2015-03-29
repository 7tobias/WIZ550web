/*
 * beamerhandler.h
 *
 *  Created on: 15.03.2015
 *      Author: tw
 */

#ifndef BEAMERHANDLER_H_
#define BEAMERHANDLER_H_

#include "debugmulticast.h"
#include "Ethernet/socket.h"

void beamer_init(void);
void beamer_run(void);
void beamer_time_handler(void);

void beamer_powerup(void);
void beamer_powerdown(void);

uint8_t getbeamerstate(void);

#endif /* BEAMERHANDLER_H_ */
