/* The WiFi Thread toggles LEDs, initializes the DA16200, joins a WiFi access point, opens the on-chip HTTP client,
 * and sends the state of all three on-board LEDs to a status server through HTTP GET. These actions are executed based
 * on the command received from the USB Thread through g_wifi_queue. */
#include "wifi_config.h"
#include "wifi_thread.h"
#include "usb_wifi_interface.h"

#define LED_COUNT (3U) // Number of on-board LEDs

static void led_toggle(uint32_t led);

/* Blue, green, red LED pins */
static const bsp_io_port_pin_t g_led_pins[LED_COUNT] = {BSP_IO_PORT_02_PIN_04, BSP_IO_PORT_03_PIN_04,
		BSP_IO_PORT_03_PIN_03};
/* LED pin states */
static bsp_io_level_t g_led_levels[LED_COUNT] = {BSP_IO_LEVEL_LOW, BSP_IO_LEVEL_LOW, BSP_IO_LEVEL_LOW};

/* WiFi Thread entry function */
void wifi_thread_entry(void *pvParameters) {
	FSP_PARAMETER_NOT_USED(pvParameters);
	uint8_t command = 0;

	while (1) {
		/* Blocks on reception of command g_wifi_queue from USB Thread */
		if (pdPASS == xQueueReceive(g_wifi_queue, &command, portMAX_DELAY)) {
			switch (command) {
				/* Toggle desired LED */
				case WIFI_APP_CMD_LED_BLUE:
				case WIFI_APP_CMD_LED_GREEN:
				case WIFI_APP_CMD_LED_RED:
					led_toggle((uint32_t) (command - WIFI_APP_CMD_LED_BLUE));
					xSemaphoreGive(g_led_sem); // Tell USB Thread the LED has been toggled

					/* Update the server with all three LED states */
					(void) wifi_send_led_status(g_led_levels[0], g_led_levels[1], g_led_levels[2]);
					break;

				case WIFI_APP_CMD_CONNECT:
					/* Initialize DA16200, open the HTTP client, and send the current LED states */
					wifi_connect(g_led_levels[0], g_led_levels[1], g_led_levels[2]);
					xSemaphoreGive(g_led_sem); // WiFi result text is ready
					break;

				default:
					xSemaphoreGive(g_led_sem); // Ensure USB thread continues
					break;
			}
		}
	}
}

/* Toggles LED based on user input from USB Thread */
static void led_toggle(uint32_t led) {
	g_led_levels[led] = (BSP_IO_LEVEL_LOW == g_led_levels[led]) ? BSP_IO_LEVEL_HIGH : BSP_IO_LEVEL_LOW;

	R_BSP_PinAccessEnable();
	R_BSP_PinWrite(g_led_pins[led], g_led_levels[led]); // Toggle LED
	R_BSP_PinAccessDisable();
}
