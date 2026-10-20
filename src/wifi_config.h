/*
 * wifi_config.h
 *
 *  Created on: Oct 7, 2026
 *      Author: a5151432
 */
#ifndef WIFI_CONFIG_H_
#define WIFI_CONFIG_H_

#include "wifi_thread.h"

/* User settings */
#define WIFI_SSID                   "USER_SSID" // Declare WiFi SSID
#define WIFI_PASSWORD               "USER_PASSWORD" // Declare WiFi password
#define WIFI_SECURITY               (eWiFiSecurityWPA2) // Declare WiFi security type

#define WIFI_CONNECT_ATTEMPTS       (2U) // Number of WiFi connection attempts
#define HTTP_REQ_BUF_SIZE           (128U) //Holds "AT+NWHTCH=<url>,get\r"
#define HTTP_RESP_BUF_SIZE          (256U) // Raw DA16200 reply; afterwards reused for the text sent to the terminal

#define STATUS_SERVER  "http://XXX.XXX.X.XX:8000" // Declare HTTP server pathway
#define STATUS_PATH_FORMAT "/update?blue=%d&green=%d&red=%d" // Status path format to be displayed on server

/* Establishes a WiFi connection, opens the HTTP client, and send the current LED states */
void wifi_connect(bsp_io_level_t blue, bsp_io_level_t green, bsp_io_level_t red);

/* Updates HTTP server with current LED status */
fsp_err_t wifi_send_led_status(bsp_io_level_t blue, bsp_io_level_t green, bsp_io_level_t red);

#endif /* WIFI_CONFIG_H_ */
