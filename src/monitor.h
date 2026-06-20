#ifndef MONITOR_H
#define MONITOR_H

#include "config.h"
#include "soc/soc.h"
#include <freertos/queue.h>
#include "interface.h"

#define threshhold_voltage 1.9
#define measure_time 15
#define base_lowBPM 55
#define base_highBPM 85
extern QueueHandle_t   bpm_queue;
extern TaskHandle_t calculateBPM_TaskHandle;
bool IRAM_ATTR heartbeat_timer_callback(void *args);
//void signalInput_task(void * pvParameters);             //
void beatMonitor_task(void * pvParameters );            
void colorChange_task(void * pvParameters);             
void calculateBPM_task(void * pvParameters);            
#endif