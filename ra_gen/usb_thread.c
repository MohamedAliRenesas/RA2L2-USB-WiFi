/* generated thread source file - do not edit */
#include "usb_thread.h"

#if 1
                static StaticTask_t usb_thread_memory;
                #if defined(__ARMCC_VERSION)           /* AC6 compiler */
                static uint8_t usb_thread_stack[1024] BSP_PLACE_IN_SECTION(BSP_UNINIT_SECTION_PREFIX ".stack.thread") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
                #else
                static uint8_t usb_thread_stack[1024] BSP_PLACE_IN_SECTION(BSP_UNINIT_SECTION_PREFIX ".stack.usb_thread") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
                #endif
                #endif
                TaskHandle_t usb_thread;
                void usb_thread_create(void);
                static void usb_thread_func(void * pvParameters);
                void rtos_startup_err_callback(void * p_instance, void * p_data);
                void rtos_startup_common_init(void);
#if BSP_TZ_NONSECURE_BUILD
 #if defined(BSP_CFG_CLOCKS_SECURE) && BSP_CFG_CLOCKS_SECURE
  #error "The LPM module requires CGC registers to be non-secure when it is used in a TrustZone Non-secure project."
 #endif
#endif

lpm_instance_ctrl_t g_lpm_ctrl;

const lpm_cfg_t g_lpm_cfg =
{
    .low_power_mode     = LPM_MODE_STANDBY,
    .standby_wake_sources       = LPM_STANDBY_WAKE_SOURCE_USBFS |  (lpm_standby_wake_source_t) 0,
#if BSP_FEATURE_ICU_HAS_WUPEN2
    .standby_wake_sources_2     =  (lpm_standby_wake_source_2_t) 0,
#endif
#if BSP_FEATURE_LPM_HAS_SNOOZE
    .snooze_cancel_sources      = LPM_SNOOZE_CANCEL_SOURCE_NONE,
    .snooze_request_source      = (lpm_snooze_request_t) LPM_SNOOZE_REQUEST_RXD0_FALLING,
#if BSP_FEATURE_LPM_SNZEDCR_MASK > 0
    .snooze_end_sources         =  (lpm_snooze_end_t) 0,
#endif
    .dtc_state_in_snooze        = LPM_SNOOZE_DTC_DISABLE,
#endif
#if BSP_FEATURE_LPM_HAS_SBYCR_OPE
    .output_port_enable         = 0,
#endif
#if BSP_FEATURE_LPM_HAS_DEEP_STANDBY
    .io_port_state                = 0,
    .power_supply_state           = 0,
    .deep_standby_cancel_source   =  (lpm_deep_standby_cancel_source_t) 0,
    .deep_standby_cancel_edge     =  (lpm_deep_standby_cancel_edge_t) 0,
#if BSP_FEATURE_LPM_HAS_DPSBYCR_DCSSMODE
    .deep_standby_soft_start_mode = 0,
#endif
#endif
#if BSP_FEATURE_LPM_HAS_PDRAMSCR
    .ram_retention_cfg.ram_retention = (uint16_t) ( 0),
    .ram_retention_cfg.tcm_retention = false,
#endif
#if BSP_FEATURE_LPM_HAS_DPSBYCR_SRKEEP
    .ram_retention_cfg.standby_ram_retention = false,
#endif
#if BSP_FEATURE_LPM_HAS_LDO_SKEEP
    .ldo_standby_cfg.pll1_ldo = false,
    .ldo_standby_cfg.pll2_ldo = false,
    .ldo_standby_cfg.hoco_ldo = false,
#endif
#if BSP_FEATURE_LPM_HAS_FLASH_MODE_SELECT
    .lpm_flash_mode_select = false,
#endif
#if BSP_FEATURE_LPM_HAS_HOCO_STARTUP_SPEED_MODE
    .lpm_hoco_startup_speed = false,
#endif
#if BSP_FEATURE_LPM_HAS_STANDBY_SOSC_SELECT
    .lpm_standby_sosc = false,
#endif
#if BSP_FEATURE_LPM_HAS_DSLPWUPIRQEN0
    .deep_sleep_wake_source0 = (lpm_deep_sleep_wake_source_t) ( 0),
#endif
#if BSP_FEATURE_LPM_HAS_DSLPWUPIRQEN1
    .deep_sleep_wake_source1 = (lpm_deep_sleep_wake_source_t) ( 0),
#endif
#if BSP_FEATURE_LPM_HAS_DSLPWUPIRQEN2
    .deep_sleep_wake_source2 = (lpm_deep_sleep_wake_source_t) ( 0),
#endif
    .p_extend           = NULL,
};

const lpm_instance_t g_lpm =
{
    .p_api = &g_lpm_on_lpm,
    .p_ctrl = &g_lpm_ctrl,
    .p_cfg = &g_lpm_cfg
};
usb_instance_ctrl_t g_basic0_ctrl;

#if !defined(g_usb_descriptor)
extern usb_descriptor_t g_usb_descriptor;
#endif
#define RA_NOT_DEFINED (1)
            const usb_cfg_t g_basic0_cfg =
            {
                .usb_mode  = USB_MODE_PERI,
                .usb_speed = USB_SPEED_FS,
                .module_number = 0,
                .type = USB_CLASS_PCDC,
#if defined(g_usb_descriptor)
                .p_usb_reg = g_usb_descriptor,
#else
                .p_usb_reg = &g_usb_descriptor,
#endif
                .usb_complience_cb = NULL,
#if defined(VECTOR_NUMBER_USBFS_INT)
                .irq       = VECTOR_NUMBER_USBFS_INT,
#else
                .irq       = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_USBFS_RESUME)
                .irq_r     = VECTOR_NUMBER_USBFS_RESUME,
#else
                .irq_r     = FSP_INVALID_VECTOR,
#endif
                .hsirq     = FSP_INVALID_VECTOR,
                .hsirq_d0  = FSP_INVALID_VECTOR,
                .hsirq_d1  = FSP_INVALID_VECTOR,
                .ipl       = (2),
                .ipl_r     = (2),
                .ipl_d0    = (2),
                .ipl_d1    = (2),
                .hsipl     = BSP_IRQ_DISABLED,
                .hsipl_d0  = BSP_IRQ_DISABLED,
                .hsipl_d1  = BSP_IRQ_DISABLED,
#if (BSP_CFG_RTOS == 0) && defined(USB_CFG_HMSC_USE)
                .p_usb_apl_callback = NULL,
#else
                .p_usb_apl_callback = usb_callback,
#endif
#if defined(NULL)
                .p_context = NULL,
#else
                .p_context = (void *) &NULL,
#endif
#if defined(VECTOR_NUMBER_USBFS_FIFO_0)
                .irq_d0    = VECTOR_NUMBER_USBFS_FIFO_0,
#else
                .irq_d0    = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_USBFS_FIFO_1)
                .irq_d1    = VECTOR_NUMBER_USBFS_FIFO_1,
#else
                .irq_d1    = FSP_INVALID_VECTOR,
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
#else
                .p_transfer_tx = &RA_NOT_DEFINED,
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
#else
                .p_transfer_rx = &RA_NOT_DEFINED,
#endif
            };
#undef RA_NOT_DEFINED

/* Instance structure to use this module. */
const usb_instance_t g_basic0 =
{
    .p_ctrl        = &g_basic0_ctrl,
    .p_cfg         = &g_basic0_cfg,
    .p_api         = &g_usb_on_usb,
};


extern uint32_t g_fsp_common_thread_count;

                const rm_freertos_port_parameters_t usb_thread_parameters =
                {
                    .p_context = (void *) NULL,
                };

                void usb_thread_create (void)
                {
                    /* Increment count so we will know the number of threads created in the RA Configuration editor. */
                    g_fsp_common_thread_count++;

                    /* Initialize each kernel object. */
                    

                    #if 1
                    usb_thread = xTaskCreateStatic(
                    #else
                    BaseType_t usb_thread_create_err = xTaskCreate(
                    #endif
                        usb_thread_func,
                        (const char *)"USB Thread",
                        1024/4, // In words, not bytes
                        (void *) &usb_thread_parameters, //pvParameters
                        2,
                        #if 1
                        (StackType_t *)&usb_thread_stack,
                        (StaticTask_t *)&usb_thread_memory
                        #else
                        & usb_thread
                        #endif
                    );

                    #if 1
                    if (NULL == usb_thread)
                    {
                        rtos_startup_err_callback(usb_thread, 0);
                    }
                    #else
                    if (pdPASS != usb_thread_create_err)
                    {
                        rtos_startup_err_callback(usb_thread, 0);
                    }
                    #endif
                }
                static void usb_thread_func (void * pvParameters)
                {
                    /* Initialize common components */
                    rtos_startup_common_init();

                    /* Initialize each module instance. */
                    

                    #if (1 == BSP_TZ_NONSECURE_BUILD) && (1 == 1)
                    /* When FreeRTOS is used in a non-secure TrustZone application, portALLOCATE_SECURE_CONTEXT must be called prior
                     * to calling any non-secure callable function in a thread. The parameter is unused in the FSP implementation.
                     * If no slots are available then configASSERT() will be called from vPortSVCHandler_C(). If this occurs, the
                     * application will need to either increase the value of the "Process Stack Slots" Property in the rm_tz_context
                     * module in the secure project or decrease the number of threads in the non-secure project that are allocating
                     * a secure context. Users can control which threads allocate a secure context via the Properties tab when
                     * selecting each thread. Note that the idle thread in FreeRTOS requires a secure context so the application
                     * will need at least 1 secure context even if no user threads make secure calls. */
                     portALLOCATE_SECURE_CONTEXT(0);
                    #endif

                    /* Enter user code for this thread. Pass task handle. */
                    usb_thread_entry(pvParameters);
                }
