/*
 * Copyright (c) 2013 Clean Energy Prospector. All rights reserved.
 */

/**
 * \file
 *         serial-lprf-mac project configuration
 * \author
 *         Mahesh Sutariya <maheshsutariya@gmail.com>
 */

#ifndef __PROJECT_CONF_H__
#define __PROJECT_CONF_H__


#define RF_TESTING_MASTER						1
#define RF_TESTING_SLAVE						2
#define SERIAL_LPRF_MAC							3
#define DEVICE_MODE									SERIAL_LPRF_MAC

#define GLOBAL_DEBUG_ON            1 /* To overide debug enabled by individual file */

#define UIP_CONF_IPV6              0
#define BUTTON_SENSOR_CONF_ON      0
#define ADC_SENSOR_CONF_ON         0
#define VIZTOOL_CONF_ON            0
#define RIMEADDR_CONF_SIZE         8
#define UART0_CONF_ENABLE						1
#define UART0_CONF_WITH_INPUT				1
#define UART1_CONF_ENABLE						1
#define UART1_CONF_WITH_INPUT				1
#define STARTUP_CONF_VERBOSE				0

#define NETSTACK_CONF_RDC          sicslowmac_driver

#endif /* PROJECT_CONF_H_ */
