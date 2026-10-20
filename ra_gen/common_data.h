/* generated common header file - do not edit */
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "FreeRTOS.h"
                #include "queue.h"
#include "FreeRTOS.h"
                #include "semphr.h"
#include "r_sci_uart.h"
            #include "r_uart_api.h"
#include "rm_at_transport_da16xxx_uart.h"
                    #include "rm_at_transport_da16xxx.h"
#include "rm_wifi_da16xxx.h"
#include "r_ioport.h"
#include "bsp_pin_cfg.h"
FSP_HEADER
/** UART on SCI Instance. */
            extern const uart_instance_t      g_uart0;

            /** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
            extern sci_uart_instance_ctrl_t     g_uart0_ctrl;
            extern const uart_cfg_t g_uart0_cfg;
            extern const sci_uart_extended_cfg_t g_uart0_cfg_extend;

            #ifndef rm_at_transport_da16xxx_uart_callback
            void rm_at_transport_da16xxx_uart_callback(uart_callback_args_t * p_args);
            #endif
extern const char *                           g_at_transport_da16xxx_uart_cmd_baud;
                   extern at_transport_da16xxx_instance_ctrl_t g_at_transport_da16xxx_ctrl;
                   extern at_transport_da16xxx_cfg_t  g_at_transport_da16xxx_cfg;
                   extern const at_transport_da16xxx_instance_t g_at_transport_da16xxx;
extern const wifi_da16xxx_cfg_t          g_wifi_da16xxx_cfg;
#define IOPORT_CFG_NAME g_bsp_pin_cfg
#define IOPORT_CFG_OPEN R_IOPORT_Open
#define IOPORT_CFG_CTRL g_ioport_ctrl

/* IOPORT Instance */
extern const ioport_instance_t g_ioport;

/* IOPORT control structure. */
extern ioport_instance_ctrl_t g_ioport_ctrl;
extern QueueHandle_t g_usb_queue;
extern QueueHandle_t g_wifi_queue;
extern SemaphoreHandle_t g_led_sem;
void g_common_init(void);
FSP_FOOTER
#endif /* COMMON_DATA_H_ */
