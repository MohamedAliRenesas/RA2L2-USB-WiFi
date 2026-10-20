/*
 * wifi_config.c
 *
 *  Created on: Oct 7, 2026
 *      Author: a5151432
 */
#include <stdio.h>
#include <string.h>
#include "wifi_config.h"
#include "wifi_thread.h"
#include "usb_wifi_interface.h"

/* Local function prototypes */
static WIFIReturnCode_t wifi_config(void);
static void network_info(fsp_err_t report_err);

/* Static global variables */
static char g_request_buffer[HTTP_REQ_BUF_SIZE]; // HTTP Request buffer
static char g_response_buffer[HTTP_RESP_BUF_SIZE] __attribute__((aligned(2))); // USBFS requires 2-byte alignment
static http_onchip_da16xxx_buffer_t g_user_buf = {.p_request_buffer = g_request_buffer,
		.req_length = sizeof(g_request_buffer), .p_response_buffer = g_response_buffer,
		.resp_length = sizeof(g_response_buffer) - 1U, // Account for NULL terminator
};
static http_onchip_da16xxx_instance_ctrl_t g_rm_http_onchip_da16xxx_instance; // DA16200 on-chip HTTP client instance
static bool g_wifi_on = false; // WiFi connection flag
static bool g_http_open = false; // HTTP connection flag

/* Returns WiFi result for the USB Thread to print */
char* wifi_app_result_get(void) {
	return g_response_buffer;
}

/* Establishes a WiFi connection, opens the HTTP client, and send the current LED states */
void wifi_connect(bsp_io_level_t blue, bsp_io_level_t green, bsp_io_level_t red) {
	/* Turn on WiFi */
	if (!g_wifi_on) {
		if (eWiFiSuccess != WIFI_On()) {
			snprintf(g_response_buffer, sizeof(g_response_buffer), "\r\nWi-Fi module failed to turn on\r\n");

			return;
		}

		g_wifi_on = true; // Set WiFi flag
	}

	/* Setup Access Point connection parameters */
	if (eWiFiSuccess != WIFI_IsConnected(NULL)) {
		if (eWiFiSuccess != wifi_config()) {
			snprintf(g_response_buffer, sizeof(g_response_buffer),
					"\r\nWi-Fi module failed to connect to \"%s\"\r\n", WIFI_SSID);

			return;
		}
	}

	/* Initialize the DA16XXX on-chip HTTP client */
	if (!g_http_open) {
		fsp_err_t http_err = RM_HTTP_DA16XXX_Open(&g_rm_http_onchip_da16xxx_instance, &g_http_onchip_da16xxx_cfg);
		if (FSP_SUCCESS != http_err) {
			snprintf(g_response_buffer, sizeof(g_response_buffer),
					"\r\nRM_HTTP_DA16XXX_Open API failed (err %d)\r\n", (int) http_err);
			return;
		}

		g_http_open = true; // Set HTTP flag
	}

	/* Send LED status to HTTP server */
	fsp_err_t report_err = wifi_send_led_status(blue, green, red);
	network_info(report_err);
}

/* Gets the IP configuration and prints it out */
static void network_info(fsp_err_t report_err) {
	WIFIIPConfiguration_t ip_config;
	uint32_t ip = 0U;

	memset(&ip_config, 0, sizeof(ip_config));
	if (eWiFiSuccess == WIFI_GetIPInfo(&ip_config)) {
		ip = ip_config.xIPAddress.ulAddress[0];
	}

	/* Store WiFi result in g_response_buffer to be printed by USB Thread */
	int len = snprintf(g_response_buffer, sizeof(g_response_buffer),
			"\r\nWi-Fi connected to \"%s\", IP %u.%u.%u.%u\r\n", WIFI_SSID, (unsigned) (ip & 0xFFU),
			(unsigned) ((ip >> 8) & 0xFFU), (unsigned) ((ip >> 16) & 0xFFU), (unsigned) ((ip >> 24) & 0xFFU));

	if ((len > 0) && ((size_t) len < sizeof(g_response_buffer))) {
		if (FSP_SUCCESS == report_err) {
			snprintf(&g_response_buffer[len], sizeof(g_response_buffer) - (size_t) len,
					"LED reporting to " STATUS_SERVER " enabled\r\n");
		}
		else {
			snprintf(&g_response_buffer[len], sizeof(g_response_buffer) - (size_t) len,
					"LED reporting enabled, but " STATUS_SERVER " did not answer (err %d)\r\n", (int) report_err);
		}
	}
}

/* Connects to the WiFi SSID */
static WIFIReturnCode_t wifi_config(void) {
	WIFINetworkParams_t net_params;

	/* Copy WiFI SSID, password, and security parameters */
	memset(&net_params, 0, sizeof(net_params));
	memcpy(net_params.ucSSID, WIFI_SSID, sizeof(WIFI_SSID));
	net_params.ucSSIDLength = (uint8_t) (sizeof(WIFI_SSID) - 1U);
	net_params.xSecurity = WIFI_SECURITY;
	memcpy(net_params.xPassword.xWPA.cPassphrase, WIFI_PASSWORD, sizeof(WIFI_PASSWORD));
	net_params.xPassword.xWPA.ucLength = (uint8_t) (sizeof(WIFI_PASSWORD) - 1U);

	/* Connect to the Access Point (AT+WFJAP, then DHCP) */
	return WIFI_ConnectAP(&net_params);
}

/* Updates HTTP server with current LED status */
fsp_err_t wifi_send_led_status(bsp_io_level_t blue, bsp_io_level_t green, bsp_io_level_t red) {
	if (!g_http_open) {
		return FSP_ERR_NOT_OPEN;
	}

	/* url string holds server endpoint and LED status of all three LEDs that are processed by HTTP server */
	char url[sizeof(STATUS_SERVER) + sizeof(STATUS_PATH_FORMAT)];
	snprintf(url, sizeof(url), STATUS_SERVER STATUS_PATH_FORMAT, (int) blue, (int) green, (int) red);

	/* Build HTTP request */
	http_onchip_da16xxx_request_t http_request = {.p_http_endpoint = url, .p_request_body = NULL,
			.method = HTTP_ONCHIP_DA16XXX_GET, .length = 0U, };

	g_response_buffer[sizeof(g_response_buffer) - 1U] = '\0';

	/* Send the HTTP request with the configured buffers */
	return RM_HTTP_DA16XXX_Send(&g_rm_http_onchip_da16xxx_instance, &http_request, &g_user_buf);
}
