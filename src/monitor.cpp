#include "monitor.h"
#include <freertos/queue.h>
#include <cmath>


TaskHandle_t    beatMonitor_TaskHandle = NULL;
TaskHandle_t    calculateBPM_TaskHandle = NULL;
TaskHandle_t    colorChange_TaskHandle = NULL;
QueueHandle_t   bpm_queue;

void Convert_BPM_to_7Seg(uint16_t bpm){
                    digit1 = DIG_SEGS[bpm % 10];
                    digit2 = DIG_SEGS[(bpm / 10) % 10];
                    digit3 = DIG_SEGS[(bpm / 100) % 10];
                    digit4 = DIG_SEGS[(bpm / 1000) % 10];
                }

 bool IRAM_ATTR heartbeat_timer_callback(void *args) {
/*
 this ISR collects the raw ADC value periodically set by frequency that is user defined in config.h
*/
    BaseType_t high_task_awoken = pdFALSE;                                  //Dont switch to higher priority task when interrupt starts
    uint16_t adc_raw = adc1_get_raw(signalInput_channel);                   //Get raw ADC value, this is a number between 0 and 4095, which corresponds to a voltage between 0 and 2.45V
    if (sample_queue != NULL) 
            xQueueSendFromISR(sample_queue, &adc_raw, &high_task_awoken);   //Send ADC reading to queue, and high_task_awoken is standard practice for freeRTOS
    else {
        printf("Error: Queue handle is NULL!\n");
    }    
    return high_task_awoken == pdTRUE;                                      //Switch to highest priority task after interrupt
}





void beatMonitor_task(void * pvParameters )
/*************************************************************************************
    This thread Monitors for heartbeat occurances.
    It recieves the raw ADC value collected in the interrupt via a message queue.
    It check if the value is above a certain threshold, threshhold_voltage,
    which is the voltage that constitutes a heartbeat. If it is a 
    heartbeat, it then udpates the calculate BPM task's notification
    value, which is being used as a counting semaphore tracking the
    number of heartbeats. 
*************************************************************************************/
{
    bool  set = false;                                                                                           //freeRTOS recognizes ticks, not ms, so this converts it
    for(;;){
        uint16_t current_adc = 0;                                           //stores the current ADC reading, cant determine voltage reading from this
        if (sample_queue != NULL) 
            xQueueReceive(sample_queue, &current_adc, portMAX_DELAY);       //wait forever, until we get the raw ADC value
        else 
            printf("Error: Queue handle is NULL!\n");
        
        float voltage = current_adc * 2.45 / 4095;                          //convert raw ADC value to voltage, more readable by the user
        if(voltage < threshhold_voltage)
            set = false;
        else if(!set){
            xTaskNotify(LED_TaskHandle, 1, eSetBits);                   //notify the LED task that we have a beat, so it can turn on the LED for a short time
            xTaskNotifyGive(calculateBPM_TaskHandle);                   //notify the BPM task that we have a new beat, so it can calculate the BPM
            printf("Heartbeat Successfully Read. voltage: %f\n", voltage); 
            set = true;  
        }   
    }  
}



void calculateBPM_task(void * pvParameters)
    /**************************************************************************
     * This task calculates the BPM value after reading. 
     * While BPM is being read, its notifaction value is updated which
     * holds the number of heartbeat occur during the reading period,
     * which is defined by measure_time. When this period is done, 
     * it then calculates the final BPM value and sends it to the colorChange
     * thread. It also periodically sends signals to the LED task to
     * flash an LED at the same rate as the BPM.
     *************************************************************************/
{
    uint16_t bpm = 0;
    for (;;){   
       uint32_t notification = ulTaskNotifyTake(pdTRUE, 0);
       switch (notification >> 31)
       {
       case 1:{  //Measuring State
            printf("In measure state\n");
            vTaskDelay(pdMS_TO_TICKS(measure_time*1000));          //Count for 15 seconds
            notification = ulTaskNotifyTake(pdTRUE, 0);
            timer_pause(TIMER_GROUP_0, TIMER_0);                  // stop timer which starts ADC collection
            timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);   
            bpm_queue = xQueueCreate(queue_size, sizeof(uint16_t));
            while(bpm_queue == NULL)
                printf("bpm queue is NULL\n");
            bpm = (notification & 0x000000FF) << 2;        // Multiply by 4 to get full BPM
            xTaskCreatePinnedToCore(colorChange_task, "color", 2000, NULL, 5, &colorChange_TaskHandle, 0 );
            xQueueSend(bpm_queue,&bpm, portMAX_DELAY);  
            printf("sent bpm to color change task\n");
            break;
       } 

       default:{ //Idle State
            if(bpm != 0){
                printf("In idle state\n");
                printf("BPM: %d\n", bpm);
                float LED_period_float = round(60.0/bpm * 1000);
                uint16_t LED_period_int = (uint16_t) LED_period_float;
                vTaskDelay(pdMS_TO_TICKS(LED_period_int));            //Match LED frequency to BPM
                xTaskNotify(LED_TaskHandle, 1, eSetBits);
                xSemaphoreTake(xMutex1, pdMS_TO_TICKS(10));
                xSemaphoreTake(xMutex2, pdMS_TO_TICKS(10));
                xSemaphoreTake(xMutex3, pdMS_TO_TICKS(10));
                xSemaphoreTake(xMutex4, pdMS_TO_TICKS(10));

                
            }
            else
                vTaskDelay(pdMS_TO_TICKS(20)); 
            break;
       }  
       }      
    }
}







void colorChange_task(void * pvParameters){
    /***************************************************************************
     * This thread decides what color the LED should flash based on the 
     * BPM value. It recieves the BPM from the calculateBPM task and
     * checks if healthy. If healthy, it sends green, otherwise sends red.
     ***************************************************************************/
    uint16_t bpm;
    enum color{
        red,
        green
    };
    for(;;){
        xQueueReceive(bpm_queue, &bpm, portMAX_DELAY);
        printf("color change recieved bpm\n");
        enum color setColor;
        if((bpm <= base_highBPM) && (bpm >= base_lowBPM))
             setColor = green;
        else
             setColor = red; 
        xTaskNotify(LED_TaskHandle, (setColor << 1),eSetValueWithOverwrite); //Send Color bit to bit 2 of LED tasks, notification value
        printf("Reading done, see LED color for information\n");
    }
    
}