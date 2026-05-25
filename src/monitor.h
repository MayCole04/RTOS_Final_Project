#ifndef MONITOR_H
#define MONITOR_H

#include "config.h"
#include "soc/soc.h"
#include <freertos/queue.h>

#define threshhold_voltage 1
bool IRAM_ATTR heartbeat_timer_callback(void *args);
void signalInput_task(void * pvParameters);
void beatMonitor_task(void * pvParameters );
void colorChange_task(void * pvParameters);
void calculateBPM_task(void * pvParameters);
#endif