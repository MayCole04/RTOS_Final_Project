#include "config.h"
#include "monitor.h"



/*
void test_task ( void * pvParameters ){
  gpio_pad_select_gpio(23); 
  gpio_set_direction(GPIO_NUM_23, GPIO_MODE_OUTPUT);
  gpio_output_enable(GPIO_NUM_23);
  gpio_set_level(GPIO_NUM_23, 0);
  for(;;){
    gpio_set_level(GPIO_NUM_23, 1);
     vTaskDelay(pdMS_TO_TICKS(500));
     //printf("High\n");
     gpio_set_level(GPIO_NUM_23, 0);
    vTaskDelay(pdMS_TO_TICKS(500));
    //printf("Low\n");
  }
}
  */

void test_task(void * pvParameters){
  
  for(;;){
    uint16_t adc = adc1_get_raw(pot_channel); 
    Convert_BPM_to_7Seg(adc);
    vTaskDelay(pdMS_TO_TICKS(50));
  }
 /*
  gpio_pad_select_gpio(25); 
    gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);
    gpio_output_enable(GPIO_NUM_25);
    gpio_set_level(GPIO_NUM_25, 0);
  for(;;){
      gpio_set_level(GPIO_NUM_25, 1);
      vTaskDelay(pdMS_TO_TICKS(500));
      gpio_set_level(GPIO_NUM_25, 0);
      vTaskDelay(pdMS_TO_TICKS(500));  
         
  }*/
}



extern "C" {
  void app_main(void){
    printf("Starting MAinnn\n");
    sample_queue = xQueueCreate(queue_size, sizeof(uint16_t)); 
    while(sample_queue == NULL){
      printf("sample queue is NULL\n");
    }
    xMutex1 = xSemaphoreCreateMutex();
    configPins();
    configTimer();
    vTaskDelay(pdMS_TO_TICKS(5)); 
    xTaskCreatePinnedToCore(SevenSegmentDisplay_task,"7-Segment", 2000, NULL, 2, &Display_TaskHandle, 0);
    xTaskCreatePinnedToCore(test_task,"test", 2000, NULL, 2, NULL, 0);
    
    //xTaskCreatePinnedToCore(beatMonitor_task, "monitor", 2000, NULL, 5, &beatMonitor_TaskHandle, 1 );
    //xTaskCreatePinnedToCore(userInput_task, "userInput", 3000, NULL, 5, NULL, 1 );
   // xTaskCreatePinnedToCore(LED_task,"LED", 2000, NULL, 2, &LED_TaskHandle, 0);

    while (1) {
        // Yield to let other FreeRTOS tasks run
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
  }
}



