#include "config.h"
extern TaskHandle_t LED_TaskHandle;
extern TaskHandle_t beatMonitor_TaskHandle;
extern TaskHandle_t calculateBPM_TaskHandle;
extern TaskHandle_t Display_TaskHandle;
extern TaskHandle_t colorChange_TaskHandle;
extern TaskHandle_t button_TaskHandle;
extern TaskHandle_t pot_TaskHandle;

extern SemaphoreHandle_t xMutex1;


extern const uint8_t DIG_SEGS[14];
/*
extern uint8_t digit1;
extern uint8_t digit2;
extern uint8_t digit3;
extern uint8_t digit4;
*/
extern uint32_t digits;


void Convert_BPM_to_7Seg(uint16_t bpm);

void LED_task(void * pvParameters);
void button_task(void * pvParameters);
extern void calculateBPM_task(void * pvParameters);  
extern void SevenSegmentDisplay_task(void * pvParameters);
void pot_task(void * pvParameters);

