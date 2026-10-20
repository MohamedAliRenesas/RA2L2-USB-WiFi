/* generated thread header file - do not edit */
#ifndef WIFI_THREAD_H_
#define WIFI_THREAD_H_
#include "bsp_api.h"
                #include "FreeRTOS.h"
                #include "task.h"
                #include "semphr.h"
                #include "hal_data.h"
                #ifdef __cplusplus
                extern "C" void wifi_thread_entry(void * pvParameters);
                #else
                extern void wifi_thread_entry(void * pvParameters);
                #endif
#include "rm_http_onchip_da16xxx.h"
FSP_HEADER
extern const http_onchip_da16xxx_cfg_t          g_http_onchip_da16xxx_cfg;
FSP_FOOTER
#endif /* WIFI_THREAD_H_ */
