#include "monitor.h"
#include <freertos/queue.h>


TaskHandle_t calculateBPM_TaskHandle = NULL;
 bool IRAM_ATTR heartbeat_timer_callback(void *args) {
    BaseType_t high_task_awoken = pdFALSE;
    uint16_t adc_raw = adc1_get_raw(signalInput_channel);
    if (sample_queue != NULL) 
            xQueueSendFromISR(sample_queue, &adc_raw, &high_task_awoken);
        else {
            printf("Error: Queue handle is NULL!\n");
        }    
    return high_task_awoken == pdTRUE; 
}

void beatMonitor_task(void * pvParameters )
{
    uint16_t beat_miss_count = 0;
    vTaskDelay(pdMS_TO_TICKS(500));
    for(;;)
    {
        uint16_t current_adc = 0;
        if (sample_queue != NULL) 
            xQueueReceive(sample_queue, &current_adc, portMAX_DELAY);
        else 
            printf("Error: Queue handle is NULL!\n");
        
        float voltage = current_adc * 2.45 / 4095;

        if(voltage < threshhold_voltage)
            beat_miss_count++;
        else
        {
            xTaskNotify(LED_TaskHandle, 1, eNoAction);
            xTaskNotifyGive(calculateBPM_TaskHandle);
            beat_miss_count = 0;
            printf("Heartbeat Successfully Read. sample:%d voltage: %f\n", beat_miss_count+1, voltage);   
        }

        if(beat_miss_count == 250)
            printf("Voltage: %f\n", voltage);   
        if(beat_miss_count == 65535)
        {
            beat_miss_count = 0;
            printf("Error: beat_miss_count overflow\n");
        }
    }
}
 QueueHandle_t         bpm_queue;

void calculateBPM_task(void * pvParameters){
    bpm_queue = xQueueCreate(queue_size, sizeof(uint8_t)); 
    while(bpm_queue == NULL)
      printf("sample queue is NULL\n");
    uint8_t bpm =0;
    for (;;){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
   
    
    ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(2));
    xQueueSend(bpm_queue,&bpm, portMAX_DELAY);
    }
}

void colorChange_task(void * pvParameters){
int x;
}