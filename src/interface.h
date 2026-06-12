#include "config.h"
extern TaskHandle_t LED_TaskHandle;
extern TaskHandle_t beatMonitor_TaskHandle;
extern TaskHandle_t calculateBPM_TaskHandle;

extern SemaphoreHandle_t xMutex1;
extern SemaphoreHandle_t xMutex2;
extern SemaphoreHandle_t xMutex3;
extern SemaphoreHandle_t xMutex4;

extern uint8_t DIG_SEGS[11];
extern uint8_t digit1;
extern uint8_t digit2;
extern uint8_t digit3;
extern uint8_t digit4;

void Convert_BPM_to_7Seg(uint16_t bpm);

void LED_task(void * pvParameters);
void userInput_task(void * pvParameters);
extern void calculateBPM_task(void * pvParameters);  
extern void SevenSegmentDisplay_task(void * pvParameters);

