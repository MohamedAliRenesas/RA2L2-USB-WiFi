/* The USB Thread opens the USB peripheral as a CDC virtual COM port, reads user commands from the PC terminal, toggles
 * LEDs on request, forwards each command to the WiFi Thread through g_wifi_queue, waits on g_connect_sem only for the
 * connect command, and prints the menu and command results back to the terminal. LED states are kept in g_led_levels,
 * which the WiFi Thread reads through get_led_levels() to report them. USB events are received from usb_callback()
 * through g_usb_queue. The USB Thread is the only thread that writes to USB. The USB Thread also implements the WDT 
 * and refreshes it once per pass of its event loop, as well as every time it waits for the connection semaphore from the 
 * WiFi Thread. Failed FSP calls and connection timeouts go to error_handler(), which prints an error message and blinks 
 * the red LED without refreshing the WDT, so the WDT resets the MCU. */
#include <string.h>
#include "usb_thread.h"
#include "usb_wifi_interface.h"

/* Private function prototypes */
static void print_to_console(const char *p_data);
static void handle_usb_command(uint8_t user_input);
static void led_toggle(uint32_t led);
static void error_handler(void);

/* Blue, green, red LED pins */
static const bsp_io_port_pin_t g_led_pins[LED_COUNT] = {BSP_IO_PORT_02_PIN_04, BSP_IO_PORT_03_PIN_04,
		BSP_IO_PORT_03_PIN_03};
/* LED pin states: Bit 0 = Blue, Bit 1 = Green, Bit 2 = Red */
static uint8_t g_led_levels = 0;

/* USB descriptors defined in r_usb_pcdc_descriptor.c */
extern uint8_t g_apl_device[];
extern uint8_t g_apl_configuration[];
extern uint8_t g_apl_hs_configuration[];
extern uint8_t g_apl_qualifier_descriptor[];
extern uint8_t *g_apl_string_table[];

const usb_descriptor_t g_usb_descriptor = {g_apl_device, // Pointer to the device descriptor
		g_apl_configuration, // Pointer to the configuration descriptor for Full-Speed
		g_apl_hs_configuration, // Pointer to the configuration descriptor for Hi-Speed
		g_apl_qualifier_descriptor, // Pointer to the qualifier descriptor
		g_apl_string_table, // Pointer to the string descriptor table
		NUM_STRING_DESCRIPTOR};

/* USB menu message strings, stored in flash to reduce RAM usage. USBFS requires the buffers to be 2-byte aligned */
static const char g_msg_menu[] __attribute__((aligned(2))) = START_MESSAGE;
static const char g_msg_blue[] __attribute__((aligned(2))) = START_MESSAGE BLUE_LED;
static const char g_msg_green[] __attribute__((aligned(2))) = START_MESSAGE GREEN_LED;
static const char g_msg_red[] __attribute__((aligned(2))) = START_MESSAGE RED_LED;
static const char g_msg_err[] __attribute__((aligned(2))) = ERROR_MESSAGE;

static bool g_usb_tx_ready = false; // Readiness flag for USB transmit
static bool b_usb_attach = false; // Flag that indicates if USB is attached

/* USB Thread entry function */
void usb_thread_entry(void *pvParameters) {
	FSP_PARAMETER_NOT_USED(pvParameters);
	fsp_err_t err = FSP_SUCCESS;
	usb_event_info_t event_info; // USB event instance
	usb_status_t event = USB_STATUS_POWERED;
	uint8_t buf[READ_BUF_SIZE] = {0}; // Buffer that stores data read over USB
	static usb_pcdc_linecoding_t line_coding; // Stores virtual UART settings
	usb_event_info_t *p_mess; // Pointer to USB event info

	err = R_WDT_Open(&g_wdt0_ctrl, &g_wdt0_cfg); // Initialize WDT
	if (err != FSP_SUCCESS) {
		error_handler();
	}

	err = R_WDT_Refresh(&g_wdt0_ctrl); // Start WDT (Register start mode)
	if (err != FSP_SUCCESS) {
		error_handler();
	}

	err = R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg); // Open USB peripheral
	if (err != FSP_SUCCESS) {
		error_handler();
	}

	err = R_LPM_Open(&g_lpm_ctrl, &g_lpm_cfg); // Initialize Software Standby mode
	if (err != FSP_SUCCESS) {
		error_handler();
	}

	while (1) {
		R_WDT_Refresh(&g_wdt0_ctrl); // Refresh WDT
		/* Wait for the next USB event from usb_callback(). The timeout keeps the WDT refreshed while USB is idle */
		if (pdPASS != xQueueReceive(g_usb_queue, (void*) &p_mess, pdMS_TO_TICKS(USB_EVENT_WAIT_MS))) {
			continue;
		}
		event_info = *p_mess;
		event = event_info.event;

		/* Process USB event */
		switch (event) {
			case USB_STATUS_CONFIGURED: // USB has been configured and confirmed to be attached
				err = R_USB_Read(&g_basic0_ctrl, buf, READ_BUF_SIZE, USB_CLASS_PCDC);
				if (err != FSP_SUCCESS) {
					error_handler();
				}
				g_usb_tx_ready = true; // USB is ready for writing
				b_usb_attach = true; // USB interface is connected
				break;

			case USB_STATUS_READ_COMPLETE: // A USB read transaction has completed
				if (b_usb_attach) { // Ensure USB connection remains established
					uint8_t key = buf[0]; // Copy before next read overwrites buf
					err = R_USB_Read(&g_basic0_ctrl, buf, READ_BUF_SIZE, USB_CLASS_PCDC); // Start next USB read
					if (err != FSP_SUCCESS) {
						error_handler();
					}
					/* Check readiness of USB write */
					if ((g_usb_tx_ready) && (event_info.data_size > 0)) {
						handle_usb_command(key); // Handle user input
					}
				}
				break;

			case USB_STATUS_REQUEST: // Receive Class Request
				/* Check for the specific CDC class request IDs */
				if ((event_info.setup.request_type & USB_BREQUEST) == USB_PCDC_SET_LINE_CODING) {
					err = R_USB_PeriControlDataGet(&g_basic0_ctrl, (uint8_t*) &line_coding, LINE_CODING_LENGTH);
					if (err != FSP_SUCCESS) {
						error_handler();
					}
				}
				else if ((event_info.setup.request_type & USB_BREQUEST) == USB_PCDC_GET_LINE_CODING) {
					err = R_USB_PeriControlDataSet(&g_basic0_ctrl, (uint8_t*) &line_coding, LINE_CODING_LENGTH);
					if (err != FSP_SUCCESS) {
						error_handler();
					}
				}
				else if ((event_info.setup.request_type & USB_BREQUEST) == USB_PCDC_SET_CONTROL_LINE_STATE) {
					err = R_USB_PeriControlStatusSet(&g_basic0_ctrl, USB_SETUP_STATUS_ACK);
					if (err != FSP_SUCCESS) {
						error_handler();
					}
				}
				break;

			case USB_STATUS_WRITE_COMPLETE:
				/* USB peripheral is ready for new write transactions */
				g_usb_tx_ready = true;
				break;

			case USB_STATUS_DETACH:
				/* Handle USB detach event */
				b_usb_attach = false;
				memset(buf, 0, sizeof(buf));
				break;

			case USB_STATUS_SUSPEND:
				/* Handle USB suspension event */
				b_usb_attach = false;
				memset(buf, 0, sizeof(buf));
				err = R_LPM_LowPowerModeEnter(&g_lpm_ctrl); // Returns after USB resume wakes MCU
				if (err != FSP_SUCCESS) {
					error_handler();
				}
				break;

			case USB_STATUS_RESUME: // USB resumed from suspend
				b_usb_attach = true;
				break;

			default:
				break;
		}
	}
}

/* Processes user input received from USB console */
static void handle_usb_command(uint8_t user_input) {
	const char *p_message = g_msg_menu; // Points to the string containing the menu start message

	if (!g_usb_tx_ready) { // Ignore key if previous message is still being sent
		return;
	}

	/* User requests connecting to WiFi */
	if (WIFI_APP_CMD_CONNECT == user_input) {
		if (pdPASS == xQueueSend(g_wifi_queue, &user_input, 0)) { // Post command to WiFi Thread
			uint32_t waited_ms = 0;

			R_WDT_Refresh(&g_wdt0_ctrl); // Start the connect wait with a full WDT period
			/* Keep refreshing the WDT while the WiFi Thread connects */
			while (pdTRUE != xSemaphoreTake(g_connect_sem, pdMS_TO_TICKS(WDT_REFRESH_MS))) {
				R_WDT_Refresh(&g_wdt0_ctrl); // Refresh WDT
				waited_ms += WDT_REFRESH_MS; // Increment wait counter

				/* Check if maximum timeout has been exceeded */
				if (waited_ms >= MAX_CONNECT_TIMEOUT_MS) {
					error_handler();
				}
			}
			print_to_console(wifi_app_result_get()); // Print result of attempted WiFi connection
		}
		return;
	}

	/* User requested LED toggle */
	if ((user_input >= WIFI_APP_CMD_LED_BLUE) && (user_input <= WIFI_APP_CMD_LED_RED)) {
		led_toggle((uint32_t) (user_input - WIFI_APP_CMD_LED_BLUE)); // Toggle the requested LED
		xQueueSend(g_wifi_queue, &user_input, 0); // Post command to WiFi Thread

		/* Update message pointer to point to message corresponding to requested LED */
		switch (user_input) {
			case WIFI_APP_CMD_LED_BLUE:
				p_message = g_msg_blue;
				break;
			case WIFI_APP_CMD_LED_GREEN:
				p_message = g_msg_green;
				break;
			case WIFI_APP_CMD_LED_RED:
				p_message = g_msg_red;
				break;
			default:
				break;
		}
	}
	print_to_console(p_message); // Print menu message
}

/* Print string to console over USB */
static void print_to_console(const char *p_data) {
	fsp_err_t err = FSP_SUCCESS;
	uint32_t len = ((uint32_t) strlen(p_data)); // Length of string

	/* Clear ready flag for USB write in progress */
	g_usb_tx_ready = false;
	err = R_USB_Write(&g_basic0_ctrl, (uint8_t*) p_data, len, USB_CLASS_PCDC); // Write string to USB
	if (err != FSP_SUCCESS) {
		g_usb_tx_ready = true; // Nothing was sent, so drop the message and set the ready flag
	}
}

/* USB callback */
void usb_callback(usb_event_info_t *p_api_event, usb_hdl_t cur_task, usb_onoff_t usb_state) {
	(void) usb_state;
	(void) cur_task;
	/* Send USB event to USB Thread */
	xQueueSend(g_usb_queue, (const void*) &p_api_event, (TickType_t) 0);
}

/* Toggles the requested LED and records its new level in g_led_levels */
static void led_toggle(uint32_t led) {
	g_led_levels ^= (uint8_t) (1U << led);

	R_BSP_PinAccessEnable();
	R_BSP_PinWrite(g_led_pins[led], (bsp_io_level_t) ((g_led_levels >> led) & 1U));
	R_BSP_PinAccessDisable();
}

/* Returns the current levels of all LEDs */
uint8_t get_led_levels(void) {
	return g_led_levels;
}

/* Prints an error message over USB and indefinitely blinks the red LED until the WDT resets the MCU */
static void error_handler(void) {
	print_to_console(g_msg_err); // Print error message
	R_BSP_PinAccessEnable();

	/* Blink red LED every half second */
	while (1) {
		R_BSP_PinWrite(BSP_IO_PORT_03_PIN_03, BSP_IO_LEVEL_HIGH);
		R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);
		R_BSP_PinWrite(BSP_IO_PORT_03_PIN_03, BSP_IO_LEVEL_LOW);
		R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);
	}
}
