#include "config.h"
extern TaskHandle_t LED_TaskHandle;
extern TaskHandle_t    beatMonitor_TaskHandle;
void LED_task(void * pvParameters);
void userInput_task(void * pvParameters);