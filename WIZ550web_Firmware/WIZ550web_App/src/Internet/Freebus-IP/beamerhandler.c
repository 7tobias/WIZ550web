/*
 * beamerhandler.c
 *
 *  Created on: 15.03.2015
 *      Author: tw
 */


#include "uartHandler.h"
#include "Ethernet/socket.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "debugmulticast.h"


static uint8_t state=0;
static int8_t beamerstate=-1; // unknown
static int8_t timer=0;
static int8_t watchdog=5;
static int8_t beamerready=1;
static int8_t cmdtosend=0;

#define BEAMERANSWERBUFFERMAX 10
static uint8_t beameranswerbuffer[BEAMERANSWERBUFFERMAX+1];
static uint8_t beameranswerbufferlength=0;


void beamer_init(void)
{
	// nothing to initialize
}


uint8_t getbeamerstate(void)
{
	return(beamerstate);
}

void beamer_time_handler(void){

	if (timer>0) timer--;
	if (watchdog>0) watchdog--;

}


void beamer_powerup(void){
	cmdtosend=11;
#if defined _DEBUG_MULTICAST_
	send_debugmulticast_data("Beamerhandler: Powering up Beamer!");
#endif

}
void beamer_powerdown(void){
	cmdtosend=10;
#if defined _DEBUG_MULTICAST_
	send_debugmulticast_data("Beamerhandler: Shutting down!");
#endif
}



void beamer_run(void)
{
	uint8_t recv_char;

	if (watchdog==0) {
		UART_write("\n\r", 2);
#if defined _DEBUG_MULTICAST_
	send_debugmulticast_data("Beamerhandler: sending NL to beamer");
#endif
		watchdog=1;

	}

	if ((beamerready == 1) && (timer<=0)) {
		switch (cmdtosend) {
		case 0:
			UART_write("PWR?\n\r", 6);
			if (beamerstate==1) cmdtosend=1; else cmdtosend=0;
			break;
		case 1:
			UART_write("ERR?\n\r", 6);
			if (beamerstate==1) cmdtosend=2; else cmdtosend=0;
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
		beamerready=0;
		watchdog=60;
		timer=5;
#if defined _DEBUG_MULTICAST_
	//send_debugmulticast_data("Beamerhandler: sending command to beamer");
#endif

	}

	if(UART_read(&recv_char, 1) > 0) {
#if defined _DEBUG_MULTICAST_
	//send_debugmulticast_buf(&recv_char, 1);
#endif


		switch (recv_char) {
		case 0x0D:
			if (strncmp(beameranswerbuffer,"PWR=00",beameranswerbufferlength) == 0) {
				if (beamerstate!=0) {
#if defined _DEBUG_MULTICAST_
					send_debugmulticast_data("Beamerstate now off (0)");
#endif
					beamerstate=0;
				}
			}
			if (strncmp(beameranswerbuffer,"PWR=01",beameranswerbufferlength) == 0) {
				if (beamerstate!=1) {
#if defined _DEBUG_MULTICAST_
					send_debugmulticast_data("Beamerstate now on (1)");
#endif
					beamerstate=1;
				}
			}
			if (strncmp(beameranswerbuffer,"PWR=02",beameranswerbufferlength) == 0) {
				if (beamerstate!=2) {
#if defined _DEBUG_MULTICAST_
					send_debugmulticast_data("Beamerstate now warming up (2)");
#endif
					beamerstate=2;

				}
			}
			if (strncmp(beameranswerbuffer,"PWR=03",beameranswerbufferlength) == 0) {
				if (beamerstate!=3) {
#if defined _DEBUG_MULTICAST_
					send_debugmulticast_data("Beamerstate now cooling down (3)");
#endif
					beamerstate=3;

				}
			}



#if defined _DEBUG_MULTICAST_
			send_debugmulticast_buf(beameranswerbuffer,beameranswerbufferlength);
#endif


			beameranswerbufferlength=0;
			break;
		case ':':
			watchdog = 120; beamerready=1;
#if defined _DEBUG_MULTICAST_
//	send_debugmulticast_data("Beamerhandler: Prompt");
#endif

			break;
		default:
			if (beameranswerbufferlength<BEAMERANSWERBUFFERMAX) {
				beameranswerbuffer[beameranswerbufferlength++]=recv_char;
			}
#if defined _DEBUG_MULTICAST_
	//send_debugmulticast_data("Beamerhandler: received char");
#endif

			break;

		}

	}

}


