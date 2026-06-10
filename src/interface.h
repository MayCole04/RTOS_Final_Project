#include "config.h"
extern TaskHandle_t LED_TaskHandle;
extern TaskHandle_t beatMonitor_TaskHandle;
extern TaskHandle_t calculateBPM_TaskHandle;

SemaphoreHandle_t xMutex1;
SemaphoreHandle_t xMutex2;
SemaphoreHandle_t xMutex3;
SemaphoreHandle_t xMutex4;

extern uint8_t DIG_SEGS[10];
extern uint8_t digit1;
extern uint8_t digit2;
extern uint8_t digit3;
extern uint8_t digit4;

void LED_task(void * pvParameters);
void userInput_task(void * pvParameters);
extern void calculateBPM_task(void * pvParameters);  
extern void SevenSegmentDisplay_task(void * pvParameters);

