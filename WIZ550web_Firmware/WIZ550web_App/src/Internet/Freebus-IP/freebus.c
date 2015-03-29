/*
 * freebus.c
 *
 *  Created on: 11.03.2015
 *      Author: tw
 */


#include "freebus.h"
#include "Ethernet/socket.h"
#include "gpioHandler.h"
#include "beamerhandler.h"

#include <stdio.h>
#include <string.h>


#ifdef _FREEBUS_DEBUG_
   #include <stdio.h>
#endif



uint8_t FREEBUS_SOCKET;                      // Socket number for Freebus

uint8_t portstatus_old[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

uint8_t  freebuscmd_beameron[]= {0x06,0x10,0x05,0x30,0x00,0x11,0x29,0x00,0xbc,0xd0,0x11,0x03,0x19,0x00,0x01,0x00,0x81};
uint8_t  freebuscmd_beameroff[]={0x06,0x10,0x05,0x30,0x00,0x11,0x29,0x00,0xbc,0xd0,0x11,0x03,0x19,0x00,0x01,0x00,0x80};

uint8_t  freebusstatus_beameron[]= {0x06,0x10,0x05,0x30,0x00,0x11,0x29,0x00,0xbc,0xb0,0x11,0x03,0x19,0x00,0x01,0x00,0x81};
uint8_t  freebusstatus_beameroff[]={0x06,0x10,0x05,0x30,0x00,0x11,0x29,0x00,0xbc,0xb0,0x11,0x03,0x19,0x00,0x01,0x00,0x80};

uint8_t bbeamerstate=-1,bnewbeamerstate;


void FREEBUS_run(void)
{
	uint8 portpin,portstatus_new;

	uint16_t len;
	uint8_t svr_addr[6] = {224,0,23,12};
	uint16_t  svr_port = FREEBUS_SERVER_PORT;

	uint8_t  buffer[128];

	for(portpin=0; portpin<15; portpin++){ // walk through Port D and get current states
		portstatus_new = get_IO_Status(portpin);
		if (portstatus_new!=portstatus_old[portpin]) { // if state changed

			if (portstatus_new) {
#ifdef _FREEBUS_DEBUG_
	printf("Freebus - Port D%d rising edge \r\n",portpin);
#endif
				if (portpin==0) send_FREEBUS_Test();

			} else {
#ifdef _FREEBUS_DEBUG_
	printf("Freebus - Port D%d falling edge \r\n",portpin);
#endif
			}

			portstatus_old[portpin] = portstatus_new;
		}
	}


	bnewbeamerstate = getbeamerstate();
	if (bnewbeamerstate!=bbeamerstate){
		bbeamerstate=bnewbeamerstate;
		if (bbeamerstate==0) send_FREEBUS(freebusstatus_beameroff, sizeof(freebusstatus_beameroff));
		if (bbeamerstate==1) send_FREEBUS(freebusstatus_beameron, sizeof(freebusstatus_beameron));
	}


	 if((len = getSn_RX_RSR(FREEBUS_SOCKET)) > 0)
	   {
	   	len = recvfrom(FREEBUS_SOCKET, buffer, sizeof(buffer) , svr_addr, &svr_port);
	   #ifdef _FREEBUS_DEBUG_
	   	  printf("FREEBUS message from %d.%d.%d.%d:%d %d received: ",svr_addr[0],svr_addr[1],svr_addr[2], svr_addr[3],svr_port, len);
	      //printf("FREEBUS message : ");
	      for (portpin=0; portpin<len; portpin++){
	    	  printf("%02x", buffer[portpin]);
	      }
	      printf(" \r\n");
	   #endif

	      if((len == sizeof(freebuscmd_beameron) && memcmp(freebuscmd_beameron, buffer,len) == 0))
	    	  beamer_powerup();
	      if((len == sizeof(freebuscmd_beameroff) && memcmp(freebuscmd_beameroff, buffer,len) == 0))
	      	  beamer_powerdown();


	   }
	   else return;

}


/* SEND Freebus packet */
void send_FREEBUS_Test(void)
{
	uint8_t dipr[4] = {224,0,23,12};

// '0610053000112900bcb011fb0003010081\0'   unten
//	0610053000112900bcd011030100010081      oben

	uint8_t  freebusbuffer[128]= //	0610053000112900bcd011030100010081 oben
	{ 0x06, 0x10, 0x05, 0x30, 0x00, 0x11, 0x29, 0x00, 0xbc, 0xd0, 0x11, 0x03, 0x01, 0x00, 0x01, 0x00, 0x81};

	uint16_t packetsize = 17;



#ifdef _FREEBUS_DEBUG_
	printf("> Send FREEBUS Test - IP: %d.%d.%d.%d:%d Socket: %d \r\n",dipr[0],dipr[1],dipr[2],dipr[3],FREEBUS_SERVER_PORT,FREEBUS_SOCKET);
#endif

	sendto(FREEBUS_SOCKET, freebusbuffer, packetsize, dipr, FREEBUS_SERVER_PORT);
}

void send_FREEBUS(uint8_t *message, uint8_t len)
{
	uint8_t dipr[4] = {224,0,23,12};


#ifdef _FREEBUS_DEBUG_
	printf("> Send FREEBUS Message - IP: %d.%d.%d.%d:%d Socket: %d \r\n",dipr[0],dipr[1],dipr[2],dipr[3],FREEBUS_SERVER_PORT,FREEBUS_SOCKET);
#endif

	sendto(FREEBUS_SOCKET, message, len, dipr, FREEBUS_SERVER_PORT);
}

void FREEBUS_init(uint8_t s)
{
	uint8_t dhar[6]={0x01,0x00,0x5e,0x00,0x17,0x0c}; // 01:00:5e:00:17:0c

	uint8_t dipr[4]={224,0,23,12};

	FREEBUS_SOCKET = s; // Socket for freebus

	do {
		setSn_CR(s, Sn_CR_CLOSE);

		setSn_MR(s,0x02 | 0x80);
		setSn_PORT(s, FREEBUS_SERVER_PORT);
		setSn_DPORT(s, FREEBUS_SERVER_PORT);
		setSn_DHAR(s, dhar);
		setSn_DIPR(s, dipr);

		setSn_CR(s,Sn_CR_OPEN);
	} while (getSn_SR(s) != SOCK_UDP);

#ifdef _FREEBUS_DEBUG_
	printf("Freebus Init \r\n");
#endif


}

