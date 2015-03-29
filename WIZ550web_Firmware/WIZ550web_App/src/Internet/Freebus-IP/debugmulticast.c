/*
 * debugmulticast.c
 *
 *  Created on: 15.03.2015
 *      Author: tw
 */


#include "debugmulticast.h"
#include "Ethernet/socket.h"
//#include "gpioHandler.h"
#include "debugmulticast.h"

uint8_t DEBUG_MULTICAST_SOCKET;                      // Socket number for DEBUG Multicast





void DEBUGMULTICAST_run(void)
{
	uint8_t i,n;

	uint16_t len;
	uint8_t svr_addr[6] = {224,0,23,181};  // 224.0.23.181 - unassigned multicast address
	uint16_t  svr_port = DEBUG_MULTICAST_PORT;

	uint8_t  buffer[128];





	 if((len = getSn_RX_RSR(DEBUG_MULTICAST_SOCKET)) > 0)
	   {
	   	len = recvfrom(DEBUG_MULTICAST_SOCKET, buffer, sizeof(buffer) , svr_addr, &svr_port);


	   }
	   else return;

}


/* SEND Debug Multicast packet */
void send_debugmulticast_data(const char *buffer)
{
	uint8_t dipr[4] = {224,0,23,181};

	sendto(DEBUG_MULTICAST_SOCKET, buffer, strlen(buffer), dipr, DEBUG_MULTICAST_PORT);
}
void send_debugmulticast_buf(char *buffer, uint16_t len)
{
	uint8_t dipr[4] = {224,0,23,181};

	sendto(DEBUG_MULTICAST_SOCKET, buffer, len, dipr, DEBUG_MULTICAST_PORT);
}

void DEBUG_MULTICAST_init(uint8_t s)
{
	uint8_t dhar[6]={0x01,0x00,0x5e,0x00,0x17,0xb5}; // 01:00:5e:00:17:b5

	uint8_t dipr[4]={224,0,23,181};

	DEBUG_MULTICAST_SOCKET = s; // Socket for debug via multicast

	do {
		setSn_CR(s, Sn_CR_CLOSE);

		setSn_MR(s,0x02 | 0x80);
		setSn_PORT(s, DEBUG_MULTICAST_PORT);
		setSn_DPORT(s, DEBUG_MULTICAST_PORT);
		setSn_DHAR(s, dhar);
		setSn_DIPR(s, dipr);

		setSn_CR(s,Sn_CR_OPEN);
	} while (getSn_SR(s) != SOCK_UDP);


}
