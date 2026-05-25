#include "monitor.h"
#include <freertos/queue.h>



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

        if(voltage < 1)
            beat_miss_count++;
        else
        {
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


void calculateBPM_task(void * pvParameters){

}

void colorChange_task(void * pvParameters){
    
}