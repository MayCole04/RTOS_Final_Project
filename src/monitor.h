#ifndef MONITOR_H
#define MONITOR_H

#include "config.h"
#include "soc/soc.h"
#include <freertos/queue.h>
#include "interface.h"

#define threshhold_voltage 1
#define measure_time 15
#define base_lowBPM 60
#define base_highBPM 79

extern TaskHandle_t calculateBPM_TaskHandle;
extern TaskHandle_t colorChange_TaskHandle;
bool IRAM_ATTR heartbeat_timer_callback(void *args);
void beatMonitor_task(void * pvParameters );            
void colorChange_task(void * pvParameters);             
void calculateBPM_task(void * pvParameters);            
#endif