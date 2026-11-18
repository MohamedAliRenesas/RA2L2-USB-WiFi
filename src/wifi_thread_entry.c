/* The WiFi Thread initializes the DA16200, joins a WiFi access point, opens the on-chip HTTP client, and sends the
 * state of all three on-board LEDs to a status server through HTTP GET. These actions are executed based on the command
 * received from the USB Thread through g_wifi_queue. The USB Thread toggles the LEDs; this thread reads their states
 * through get_led_levels() and gives g_connect_sem only when a connect command has finished. If an LED report fails
 * after the HTTP client has been opened, this thread disconnects and calls wifi_connect() to rejoin the access point. */
#include "wifi_config.h"
#include "wifi_thread.h"
#include "usb_wifi_interface.h"

/* WiFi Thread entry function */
void wifi_thread_entry(void *pvParameters) {
	FSP_PARAMETER_NOT_USED(pvParameters);
	uint8_t command = 0;
	uint8_t leds = 0;

	while (1) {
		/* Blocks on reception of command g_wifi_queue from USB Thread */
		if (pdPASS == xQueueReceive(g_wifi_queue, &command, portMAX_DELAY)) {
			switch (command) {
				/* LED commands: the USB Thread has already toggled the LED */
				case WIFI_APP_CMD_LED_BLUE:
				case WIFI_APP_CMD_LED_GREEN:
				case WIFI_APP_CMD_LED_RED:
					leds = get_led_levels(); // Get current LED states
					/* Update the server with all three LED states */
					fsp_err_t err = wifi_send_led_status((bsp_io_level_t)(leds & 1U), (bsp_io_level_t)((leds >> 1U) & 1U), (bsp_io_level_t)((leds >> 2U) & 1U));
					if((FSP_SUCCESS != err) && (FSP_ERR_NOT_OPEN != err)) {
						(void) WIFI_Disconnect(); // Clear cached IP
						wifi_connect((bsp_io_level_t)(leds & 1U), (bsp_io_level_t)((leds >> 1U) & 1U), (bsp_io_level_t)((leds >> 2U) & 1U)); // Reconnect
					}
					break;

				case WIFI_APP_CMD_CONNECT:
					leds = get_led_levels(); // Get current LED states
					/* Initialize DA16200, open the HTTP client, and send the current LED states */
					wifi_connect((bsp_io_level_t)(leds & 1U), (bsp_io_level_t)((leds >> 1U) & 1U), (bsp_io_level_t)((leds >> 2U) & 1U));
					xSemaphoreGive(g_connect_sem); // WiFi connection is complete and result text is ready
					break;

				default:
					break;
			}
		}
	}
}
