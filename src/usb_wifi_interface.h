/*
 * usb_wifi_interface.h
 *
 *  Created on: Oct 6, 2026
 *      Author: a5151432
 */

#ifndef USB_WIFI_INTERFACE_H_
#define USB_WIFI_INTERFACE_H_

/* Commands the USB thread posts to g_wifi_queue. The WiFi thread gives g_led_sem when it has finished each one. */
#define WIFI_APP_CMD_LED_BLUE     ('0')
#define WIFI_APP_CMD_LED_GREEN    ('1')
#define WIFI_APP_CMD_LED_RED      ('2')
#define WIFI_APP_CMD_CONNECT      ('3')

#define START_MESSAGE "Welcome to the RA2L2 USB WiFi Project \r\n"\
					  "0: Toggle Blue LED\r\n"\
					  "1: Toggle Green LED\r\n"\
					  "2: Toggle Red LED\r\n"\
					  "3: Connect to WiFi (enables LED reporting)\r\n\r\n"

#define BLUE_LED "Toggled Blue LED \r\n\r\n"
#define GREEN_LED "Toggled Green LED \r\n\r\n"
#define RED_LED "Toggled Red LED \r\n\r\n"

#define LINE_CODING_LENGTH (0x07U)
#define READ_BUF_SIZE (8U)
#define NUM_STRING_DESCRIPTOR (7U)

/* Returns WiFi result for the USB Thread to print */
char * wifi_app_result_get(void);

#endif /* USB_WIFI_INTERFACE_H_ */
