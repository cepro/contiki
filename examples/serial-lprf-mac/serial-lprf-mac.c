/*
 * Copyright (c) 2013 Clean Energy Prospector. All rights reserved.
 */

/**
 * \file
 *         Running 802.15.4 MAC on RF Soc. Application layer can control RF Soc
 *         MAC using Serial interface
 * \author
 *         Mahesh Sutariya <maheshsutariya@gmail.com>
 */

#include "contiki.h"
#include "dev/leds.h"
#include "dev/uart0.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "serial-lprf-mac.h"

#define DEBUG 1
#if DEBUG && GLOBAL_DEBUG_ON
#define PUTSTRING(...) putstring(__VA_ARGS__)
#define PUTDEC(...) putdec(__VA_ARGS__)
#else
#define PUTSTRING(...)
#endif

#if (DEVICE_MODE == RF_TESTING_MASTER)
  static struct etimer sendHelloEvent;
  static uint8_t count = 0;
  const rimeaddr_t rimeaddr_sender = { { 0x00, 0x12, 0x4b, 0x00, 0x01, 0x0e, 0x10, 0xb1 } };
  const rimeaddr_t rimeaddr_receiver = { { 0x00, 0x12, 0x4b, 0x00, 0x01, 0x0e, 0x05, 0x8a } };
#elif (DEVICE_MODE == RF_TESTING_SLAVE)
  static int32_t len;
  const rimeaddr_t rimeaddr_sender = { { 0x00, 0x12, 0x4b, 0x00, 0x01, 0x0e, 0x05, 0x8a } };
  const rimeaddr_t rimeaddr_receiver = { { 0x00, 0x12, 0x4b, 0x00, 0x01, 0x0e, 0x10, 0xb1 } };
#else // SERIAL_LPRF_MAC
	static SerialRecState_e state = SERIAL_REC_STATE_WAIT_FOR_START;
	static uint8_t pktLength = 0, dataBuffer[30], dataReceived;
	static int32_t len;
#endif
  int serial_lprf_mac_serial_input(unsigned char c);
/*---------------------------------------------------------------------------*/
PROCESS(serial_lprf_mac_process, "Serial LPRF MAC");
AUTOSTART_PROCESSES(&serial_lprf_mac_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(serial_lprf_mac_process, ev, data)
{
  PROCESS_BEGIN();

  PUTSTRING("Serial LPRF MAC\n");

  rimeaddr_set_node_addr(&rimeaddr_node_addr);
#if (DEVICE_MODE == RF_TESTING_MASTER)
  PUTSTRING("RF Testing Master\n");

  NETSTACK_RADIO.init();
  NETSTACK_RDC.init();
  NETSTACK_MAC.init();
  etimer_set(&sendHelloEvent, CLOCK_SECOND * 6);

  while(1) {
    PROCESS_WAIT_EVENT();

    if(ev == PROCESS_EVENT_TIMER) {
      PUTSTRING("Send Hello World ");
      PUTDEC(count++);
      PUTSTRING("\n");
      packetbuf_clear();
      packetbuf_copyfrom("Hello World", sizeof("Hello World"));
      packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, &rimeaddr_receiver);
      NETSTACK_RDC.send(NULL, NULL);
      leds_toggle(LEDS_GREEN);
      etimer_reset(&sendHelloEvent);
    }
  }
#elif (DEVICE_MODE == RF_TESTING_SLAVE)
  PUTSTRING("RF Testing Slave\n");

  NETSTACK_RADIO.init();
  NETSTACK_RDC.init();
  NETSTACK_MAC.init();
  while(1) {
    len = NETSTACK_RADIO.pending_packet();
    if(len) {
      packetbuf_clear();
      len = NETSTACK_RADIO.read(packetbuf_dataptr(), PACKETBUF_SIZE);
      if(len > 0) {
        packetbuf_set_datalen(len);
        NETSTACK_RDC.input();
      }
    }
  }
#else // SERIAL_LPRF_MAC
  uart0_set_input(serial_lprf_mac_serial_input);
	
	NETSTACK_RADIO.init();
	NETSTACK_RDC.init();
	NETSTACK_MAC.init();

	while(1) {
		if(state == SERIAL_REC_STATE_PACKET_COMPLETE)
		{
			switch(dataBuffer[0])
			{
				case 'S':
					PUTSTRING("Send packet\n");
					packetbuf_clear();
					packetbuf_copyfrom(&dataBuffer[9], pktLength - 8 - 1);
					packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, (rimeaddr_t*)&dataBuffer[1]);
					NETSTACK_RDC.send(NULL, NULL);
					leds_toggle(LEDS_GREEN);
					break;
				default:
					PUTSTRING("Wrong command received\n");
			}
			state = SERIAL_REC_STATE_WAIT_FOR_START;
		}
		
		len = NETSTACK_RADIO.pending_packet();
		if(len) {
			packetbuf_clear();
			len = NETSTACK_RADIO.read(packetbuf_dataptr(), PACKETBUF_SIZE);
			if(len > 0) {
				packetbuf_set_datalen(len);
				NETSTACK_RDC.input();
			}
		}
	}
#endif
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
#if (DEVICE_MODE == SERIAL_LPRF_MAC)
int
serial_lprf_mac_serial_input(unsigned char byteReceived)
{
  switch(state)
  {
		case SERIAL_REC_STATE_WAIT_FOR_START:
			if(byteReceived == SERIAL_LPRF_MAC_PACKET_START_BYTE)
			{
				state = SERIAL_REC_STATE_PACKET_START_RECEIVED;
				//PUTSTRING("Packet start received\n");
			}
			break;
			
		case SERIAL_REC_STATE_PACKET_START_RECEIVED:
			pktLength = byteReceived;
			dataReceived = 0;
			//PUTSTRING("Packet length received\n");
			state = SERIAL_REC_STATE_PACKET_LENGTH_RECEIVED;
			break;
			
		case SERIAL_REC_STATE_PACKET_LENGTH_RECEIVED:
			dataBuffer[dataReceived++] = byteReceived;
			if(dataReceived >= (pktLength - 1))
			{
				state = SERIAL_REC_STATE_PACKET_COMPLETE;
				//PUTSTRING("Packet complete\n");
			}
			break;
			
		case SERIAL_REC_STATE_PACKET_COMPLETE:
			// Wait till packet buffer is empty
			break;
			
		default:
			state = SERIAL_REC_STATE_WAIT_FOR_START;
			break;
  }
  return 0;
}
#endif
/*---------------------------------------------------------------------------*/
void
serial_lprf_mac_rf_input(void)
{
	uint8_t bufCnt, *bufPtr = packetbuf_dataptr();
#if (DEVICE_MODE == SERIAL_LPRF_MAC)
	uint8_t *addrPtr;
#endif
	
	PUTSTRING("Data Received : ");
	PUTSTRING(bufPtr);
	PUTSTRING("\n");
	leds_toggle(LEDS_RED);
#if (DEVICE_MODE == SERIAL_LPRF_MAC)
	putchar(SERIAL_LPRF_MAC_PACKET_START_BYTE);
	putchar(packetbuf_datalen() + 1 + 8);
	putchar('R');
	addrPtr = (uint8_t *)packetbuf_addr(PACKETBUF_ADDR_SENDER);
	for(bufCnt = 0; bufCnt < 8; bufCnt++)
		putchar( * (addrPtr  + bufCnt));
	for(bufCnt = 0; bufCnt < packetbuf_datalen(); bufCnt++)
		putchar( * (bufPtr + bufCnt));
#endif
}