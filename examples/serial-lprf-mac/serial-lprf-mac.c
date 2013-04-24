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
#include "net/netstack.h"
#include "net/packetbuf.h"

#define DEBUG 1
#if DEBUG && GLOBAL_DEBUG_ON
#define PUTSTRING(...) putstring(__VA_ARGS__)
#define PUTDEC(...) putdec(__VA_ARGS__)
#else
#define PUTSTRING(...)
#endif

#ifdef RF_TESTING_MASTER
  static struct etimer sendHelloEvent;
  static uint8_t count = 0;
#endif
#ifdef RF_TESTING_SLAVE
  static int32_t len;
#endif

/*---------------------------------------------------------------------------*/
PROCESS(serial_lprf_mac_process, "Serial LPRF MAC");
AUTOSTART_PROCESSES(&serial_lprf_mac_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(serial_lprf_mac_process, ev, data)
{
  PROCESS_BEGIN();

  PUTSTRING("Serial LPRF MAC\n");

#ifdef RF_TESTING_MASTER
  PUTSTRING("RF Testing Master\n");

  NETSTACK_RADIO.init();
  NETSTACK_RDC.init();
  NETSTACK_MAC.init();
  etimer_set(&sendHelloEvent, CLOCK_SECOND * 2);

  while(1) {
    PROCESS_WAIT_EVENT();

    if(ev == PROCESS_EVENT_TIMER) {
      PUTSTRING("Send Hello World ");
      PUTDEC(count++);
      PUTSTRING("\n");
      packetbuf_clear();
      packetbuf_copyfrom("Hello World", sizeof("Hello World"));
      packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, &rimeaddr_null);
      NETSTACK_RDC.send(NULL, NULL);
      leds_toggle(LEDS_GREEN);
      etimer_reset(&sendHelloEvent);
    }
  }
#endif
#ifdef RF_TESTING_SLAVE
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
#endif
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
