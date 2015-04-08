/*
 * avrhandler.c
 *
 *  Created on: 22.03.2015
 *      Author: tw
 */


#define _AVRHANDLER_DEBUG_

#include "uartHandler.h"

#include "Ethernet/socket.h"


#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef _AVRHANDLER_DEBUG_
	#include "debugmulticast.h"
#endif

static uint8_t state=0;
static int8_t avrstate=-1; // unknown
static int8_t timer=0;
static int8_t watchdog=5;
static int8_t avrready=1;
static int8_t cmdtosend=0;

#define AVRANSWERBUFFERMAX 20
static uint8_t avranswerbuffer[AVRANSWERBUFFERMAX+1];
static uint8_t avranswerbufferlength=0;

void avr_init(void)
{
	// nothing to initialize
}

uint8_t getavrstate(void)
{
	return(avrstate);
}

void avr_time_handler(void){

	if (timer>0) timer--;
	if (watchdog>0) watchdog--;

}


void avr_powerup(void){
	cmdtosend=11;
#if defined _DEBUG_MULTICAST_
	send_debugmulticast_data("avrhandler: Powering up avr!");
#endif

}
void avr_powerdown(void){
	cmdtosend=10;
#if defined _DEBUG_MULTICAST_
	send_debugmulticast_data("avrhandler: Shutting down!");
#endif
}



void avr_run(void)
{
	uint8_t recv_char;

	if (watchdog==0) {
		UART_write("\n\r", 2);
#if defined _DEBUG_MULTICAST_
	send_debugmulticast_data("avrhandler: sending NL to avr");
#endif
		watchdog=1;

	}

	if ((avrready == 1) && (timer<=0)) {
		switch (cmdtosend) {
		case 0:
			UART_write("PWR?\n\r", 6);
			if (avrstate==1) cmdtosend=1; else cmdtosend=0;
			break;
		case 1:
			UART_write("ERR?\n\r", 6);
			if (avrstate==1) cmdtosend=2; else cmdtosend=0;
			break;
		case 2:
			UART_write("SOURCE?\n\r", 9);
			cmdtosend=0;
		break;
		case 10:
			UART_write("PWR OFF\n\r", 9);
			cmdtosend=0;
		break;
		case 11:
			UART_write("PWR ON\n\r", 8);
			cmdtosend=0;
			break;
		}
		avrready=0;
		watchdog=60;
		timer=5;
#if defined _DEBUG_MULTICAST_
	//send_debugmulticast_data("avrhandler: sending command to avr");
#endif

	}

	if(UART_read(&recv_char, 1) > 0) {
#if defined _DEBUG_MULTICAST_
	//send_debugmulticast_buf(&recv_char, 1);
#endif


		switch (recv_char) {
		case 0x0D:
			send_debugmulticast_buf(&avranswerbuffer, avranswerbufferlength);
			if (strncmp(avranswerbuffer,"PWR=00",avranswerbufferlength) == 0) {
				if (avrstate!=0) {
#if defined _DEBUG_MULTICAST_
					send_debugmulticast_data("avrstate now off (0)");
#endif
					avrstate=0;
				}
			}
			if (strncmp(avranswerbuffer,"PWR=01",avranswerbufferlength) == 0) {
				if (avrstate!=1) {
#if defined _DEBUG_MULTICAST_
					send_debugmulticast_data("avrstate now on (1)");
#endif
					avrstate=1;
				}
			}
			if (strncmp(avranswerbuffer,"PWR=02",avranswerbufferlength) == 0) {
				if (avrstate!=2) {
#if defined _DEBUG_MULTICAST_
					send_debugmulticast_data("avrstate now warming up (2)");
#endif
					avrstate=2;

				}
			}
			if (strncmp(avranswerbuffer,"PWR=03",avranswerbufferlength) == 0) {
				if (avrstate!=3) {
#if defined _DEBUG_MULTICAST_
					send_debugmulticast_data("avrstate now cooling down (3)");
#endif
					avrstate=3;

				}
			}



#if defined _DEBUG_MULTICAST_
			send_debugmulticast_buf(avranswerbuffer,avranswerbufferlength);
#endif


			avranswerbufferlength=0;
			break;
		case ':':
			watchdog = 120; avrready=1;
#if defined _DEBUG_MULTICAST_
//	send_debugmulticast_data("avrhandler: Prompt");
#endif

			break;
		default:
			if (avranswerbufferlength<AVRANSWERBUFFERMAX) {
				avranswerbuffer[avranswerbufferlength++]=recv_char;
			}
#if defined _DEBUG_MULTICAST_
	//send_debugmulticast_data("avrhandler: received char");
#endif

			break;

		}

	}

}

