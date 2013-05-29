/*
 * Copyright (c) 2013 Clean Energy Prospector. All rights reserved.
 */

/**
 * \file
 *         Header file for serial LPRF MAC
 * \author
 *         Mahesh Sutariya <maheshsutariya@gmail.com>
 */

#ifndef __SERIAL_LPRF_MAC__
#define __SERIAL_LPRF_MAC__

typedef enum
{
	SERIAL_REC_STATE_WAIT_FOR_START = 0,
	SERIAL_REC_STATE_PACKET_START_RECEIVED,
	SERIAL_REC_STATE_PACKET_LENGTH_RECEIVED,
	SERIAL_REC_STATE_PACKET_COMPLETE,
	SERIAL_REC_STATE_MAX
}SerialRecState_e;

#define SERIAL_LPRF_MAC_PACKET_START_BYTE				0x3C

void serial_lprf_mac_rf_input(void);

#endif /* __SERIAL_LPRF_MAC__ */