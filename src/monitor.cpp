#include "monitor.h"
#include <freertos/queue.h>
#include <cmath>

/*
; this ISR collects the raw ADC value periodically set by frequency that is user defined in config.h
*/
TaskHandle_t    beatMonitor_TaskHandle = NULL;
TaskHandle_t    calculateBPM_TaskHandle = NULL;
QueueHandle_t   bpm_queue;
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
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        uint16_t current_adc = 0;           //stores the current ADC reading, cant determine voltage reading from this
        if (sample_queue != NULL) 
            xQueueReceive(sample_queue, &current_adc, portMAX_DELAY);       //wait forever, until we get the raw ADC value
        else 
            printf("Error: Queue handle is NULL!\n");
        
        float voltage = current_adc * 2.45 / 4095;  //convert raw ADC value to voltage, more readable by the user
        if(voltage < threshhold_voltage)
            beat_miss_count++;
        else {
            if(calculateBPM_TaskHandle ==NULL)
                xTaskCreatePinnedToCore(calculateBPM_task, "BPM", 2000, NULL, 4, &calculateBPM_TaskHandle, 0 );
            xTaskNotify(LED_TaskHandle, 1, eSetBits);      //notify the LED task that we have a beat, so it can turn on the LED for a short time
            xTaskNotifyGive(calculateBPM_TaskHandle);       //notify the BPM task that we have a new beat, so it can calculate the BPM
            beat_miss_count = 0;
            printf("Heartbeat Successfully Read. sample:%d voltage: %f\n", beat_miss_count+1, voltage);   
        }

        if(beat_miss_count == 250)      //if we dont get a beat for .5 seconds, we debug by printing the voltage.
            printf("Voltage: %f\n", voltage);   
        if(beat_miss_count == 65535){
            beat_miss_count = 0;
            printf("Error: beat_miss_count overflow\n");
        }   
    }
}


void calculateBPM_task(void * pvParameters){
    
    uint8_t bpm =0;
    for (;;){   
       uint32_t notification = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
       switch (notification >> 31)
       {
       case 1:{  //Measuring State
            vTaskDelay(pdMS_TO_TICKS(measure_time*1000));          //Count for 15 seconds
            notification = ulTaskNotifyTake(pdTRUE, 0);
            timer_pause(TIMER_GROUP_0, TIMER_0);
            timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);

            bpm_queue = xQueueCreate(queue_size, sizeof(uint8_t));
            while(bpm_queue == NULL)
                printf("bpm queue is NULL\n");
            bpm = (notification & 0x000000FF);
            xTaskCreatePinnedToCore(colorChange_task, "color", 2000, NULL, 4, NULL, 0 );
            xQueueSend(bpm_queue,&bpm, portMAX_DELAY);  
            break;
       } 
       default:{ //Idle State
            if(bpm != 0){
                float LED_period_float = round(60/bpm * 1000);
                uint16_t LED_period_int = (uint16_t) LED_period_float;
                vTaskDelay(pdMS_TO_TICKS(LED_period_int));            //Match LED frequency to BPM
                xTaskNotify(LED_TaskHandle, 1, eSetBits);
            }
            break;
       }  
       }      
    }
}

void colorChange_task(void * pvParameters){
    uint8_t bpm;
    enum color{
        red,
        green
    };
    for(;;){
        xQueueReceive(bpm_queue, &bpm, portMAX_DELAY);
        enum color setColor;
        if((bpm <= base_highBPM) && (bpm >= base_lowBPM))
             setColor = green;
        else
             setColor = red; 
        xTaskNotify(LED_TaskHandle, (setColor << 1),eSetValueWithOverwrite); //Send Color bit to bit 2 of LED tasks, notification value
        printf("Reading done, see LED color for information\n");
    }
    
}