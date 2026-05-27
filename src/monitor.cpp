#include "monitor.h"
#include <freertos/queue.h>

/*
; this ISR collects the raw ADC value periodically set by frequency that is user defined in config.h
*/
TaskHandle_t calculateBPM_TaskHandle = NULL;
 bool IRAM_ATTR heartbeat_timer_callback(void *args) {
    BaseType_t high_task_awoken = pdFALSE;                  //Dont switch to higher priority task when interrupt starts
    uint16_t adc_raw = adc1_get_raw(signalInput_channel);   //Get raw ADC value, this is a number between 0 and 4095, which corresponds to a voltage between 0 and 2.45V
    if (sample_queue != NULL) 
            xQueueSendFromISR(sample_queue, &adc_raw, &high_task_awoken);       //Send ADC reading to queue, and high_task_awoken is standard practice for freeRTOS
    else {
        printf("Error: Queue handle is NULL!\n");
    }    
    return high_task_awoken == pdTRUE;          //Switch to highest priority task after interrupt
}

void beatMonitor_task(void * pvParameters )
{
    uint16_t beat_miss_count = 0;           //used if the ADC reading is below the threshhold voltage for a heartbeat
    vTaskDelay(pdMS_TO_TICKS(500));         //freeRTOS recognizes ticks, not ms, so this converts it
    for(;;)
    {
        uint16_t current_adc = 0;           //stores the current ADC reading, cant determine voltage reading from this
        if (sample_queue != NULL) 
            xQueueReceive(sample_queue, &current_adc, portMAX_DELAY);       //wait forever, until we get the raw ADC value
        else 
            printf("Error: Queue handle is NULL!\n");
        
        float voltage = current_adc * 2.45 / 4095;  //convert raw ADC value to voltage, more readable by the user

        if(voltage < threshhold_voltage)
            beat_miss_count++;
        else
        {
            xTaskNotify(LED_TaskHandle, 1, eNoAction);      //notify the LED task that we have a beat, so it can turn on the LED for a short time
            xTaskNotifyGive(calculateBPM_TaskHandle);       //notify the BPM task that we have a new beat, so it can calculate the BPM
            beat_miss_count = 0;
            printf("Heartbeat Successfully Read. sample:%d voltage: %f\n", beat_miss_count+1, voltage);   
        }

        if(beat_miss_count == 250)      //if we dont get a beat for .5 seconds, we debug by printing the voltage.
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