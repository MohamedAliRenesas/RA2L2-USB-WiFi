/* generated common source file - do not edit */
#include "common_data.h"
sci_uart_instance_ctrl_t     g_uart0_ctrl;

            baud_setting_t               g_uart0_baud_setting =
            {
                /* Baud rate calculated with 0.160% error. */ .semr_baudrate_bits_b.abcse = 0, .semr_baudrate_bits_b.abcs = 0, .semr_baudrate_bits_b.bgdm = 1, .cks = 0, .brr = 12, .mddr = (uint8_t) 256, .semr_baudrate_bits_b.brme = false
            };

            /** UART extended configuration for UARTonSCI HAL driver */
            const sci_uart_extended_cfg_t g_uart0_cfg_extend =
            {
                .clock                = SCI_UART_CLOCK_INT,
                .rx_edge_start          = SCI_UART_START_BIT_FALLING_EDGE,
                .noise_cancel         = SCI_UART_NOISE_CANCELLATION_DISABLE,
                .rx_fifo_trigger        = SCI_UART_RX_FIFO_TRIGGER_MAX,
                .p_baud_setting         = &g_uart0_baud_setting,
                .flow_control           = SCI_UART_FLOW_CONTROL_RTS,
                #if 0xFF != 0xFF
                .flow_control_pin       = BSP_IO_PORT_FF_PIN_0xFF,
                #else
                .flow_control_pin       = (bsp_io_port_pin_t) UINT16_MAX,
                #endif
                .rs485_setting = {
                    .enable = SCI_UART_RS485_DISABLE,
                    .polarity = SCI_UART_RS485_DE_POLARITY_HIGH,
                #if 0xFF != 0xFF
                    .de_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
                #else
                    .de_control_pin       = (bsp_io_port_pin_t) UINT16_MAX,
                #endif
                },
                .irda_setting = {
                    .ircr_bits_b.ire = 0,
                    .ircr_bits_b.irrxinv = 0,
                    .ircr_bits_b.irtxinv = 0,
                },
            };

            /** UART interface configuration */
            const uart_cfg_t g_uart0_cfg =
            {
                .channel             = 0,
                .data_bits           = UART_DATA_BITS_8,
                .parity              = UART_PARITY_OFF,
                .stop_bits           = UART_STOP_BITS_1,
                .p_callback          = rm_at_transport_da16xxx_uart_callback,
                .p_context           = NULL,
                .p_extend            = &g_uart0_cfg_extend,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                .p_transfer_tx       = NULL,
#else
                .p_transfer_tx       = &RA_NOT_DEFINED,
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                .p_transfer_rx       = NULL,
#else
                .p_transfer_rx       = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
                .rxi_ipl             = (2),
                .txi_ipl             = (2),
                .tei_ipl             = (2),
                .eri_ipl             = (2),
#if defined(VECTOR_NUMBER_SCI0_RXI)
                .rxi_irq             = VECTOR_NUMBER_SCI0_RXI,
#else
                .rxi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI0_TXI)
                .txi_irq             = VECTOR_NUMBER_SCI0_TXI,
#else
                .txi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI0_TEI)
                .tei_irq             = VECTOR_NUMBER_SCI0_TEI,
#else
                .tei_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI0_ERI)
                .eri_irq             = VECTOR_NUMBER_SCI0_ERI,
#else
                .eri_irq             = FSP_INVALID_VECTOR,
#endif
            };

/* Instance structure to use this module. */
const uart_instance_t g_uart0 =
{
    .p_ctrl        = &g_uart0_ctrl,
    .p_cfg         = &g_uart0_cfg,
    .p_api         = &g_uart_on_sci
};
const char *g_at_transport_da16xxx_uart_cmd_baud = "115200";
        at_transport_da16xxx_instance_ctrl_t g_at_transport_da16xxx_ctrl;
        const at_transport_da16xxx_extended_cfg_t g_at_transport_da16xxx_cfg_extend =
        {
        #define RA_NOT_DEFINED (1)
        .num_uarts = 1,
        .reset_pin = AT_TRANSPORT_DA16XXX_CFG_RESET_PIN,
        .uart_instances =
        {

        #if (RA_NOT_DEFINED == g_uart0)
            NULL,
        #else
            &g_uart0,
        #endif

        NULL,

        #undef RA_NOT_DEFINED
        }
        };

        at_transport_da16xxx_cfg_t g_at_transport_da16xxx_cfg =
        {
        .p_extend = &g_at_transport_da16xxx_cfg_extend,
        .p_callback         = rm_wifi_da16xxx_callback,
        };

        const at_transport_da16xxx_instance_t g_at_transport_da16xxx =
        {
            .p_ctrl = &g_at_transport_da16xxx_ctrl, .p_cfg = &g_at_transport_da16xxx_cfg,
            .p_api  = &g_at_transport_da16xxx_on_uart,
        };
/** WiFi DA16XXX configuration */
            const wifi_da16xxx_cfg_t     g_wifi_da16xxx_cfg =
            {
                .p_transport_instance        = &g_at_transport_da16xxx,
                .num_sockets = WIFI_DA16XXX_CFG_NUM_CREATEABLE_SOCKETS,
                .country_code = (uint8_t*)"US",
                .sntp_server_ip = (uint8_t*)"0.0.0.0",
                .sntp_utc_offset_in_hours = 0,
            };
ioport_instance_ctrl_t g_ioport_ctrl;
const ioport_instance_t g_ioport =
        {
            .p_api = &g_ioport_on_ioport,
            .p_ctrl = &g_ioport_ctrl,
            .p_cfg = &g_bsp_pin_cfg,
        };

QueueHandle_t g_usb_queue;
                #if 1
                StaticQueue_t g_usb_queue_memory;
                uint8_t g_usb_queue_queue_memory[4 * 16];
                #endif
                void rtos_startup_err_callback(void * p_instance, void * p_data);
QueueHandle_t g_wifi_queue;
                #if 1
                StaticQueue_t g_wifi_queue_memory;
                uint8_t g_wifi_queue_queue_memory[1 * 8];
                #endif
                void rtos_startup_err_callback(void * p_instance, void * p_data);
SemaphoreHandle_t g_led_sem;
                #if 1
                StaticSemaphore_t g_led_sem_memory;
                #endif
                void rtos_startup_err_callback(void * p_instance, void * p_data);
void g_common_init(void) {
g_usb_queue =
                #if 1
                xQueueCreateStatic(
                #else
                xQueueCreate(
                #endif
                16,
                4
                #if 1
                , &g_usb_queue_queue_memory[0],
                &g_usb_queue_memory
                #endif
                );
                if (NULL == g_usb_queue) {
                rtos_startup_err_callback(g_usb_queue, 0);
                }
g_wifi_queue =
                #if 1
                xQueueCreateStatic(
                #else
                xQueueCreate(
                #endif
                8,
                1
                #if 1
                , &g_wifi_queue_queue_memory[0],
                &g_wifi_queue_memory
                #endif
                );
                if (NULL == g_wifi_queue) {
                rtos_startup_err_callback(g_wifi_queue, 0);
                }
g_led_sem =
                #if 1
                xSemaphoreCreateBinaryStatic(&g_led_sem_memory);
                #else
                xSemaphoreCreateBinary();
                #endif
                if (NULL == g_led_sem) {
                rtos_startup_err_callback(g_led_sem, 0);
                }
}
