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
  printf("test task is running\n");
  for(;;){
    Convert_BPM_to_7Seg('b');
    vTaskDelay(pdMS_TO_TICKS(1000));
    Convert_BPM_to_7Seg(114);
    vTaskDelay(pdMS_TO_TICKS(1000));
    Convert_BPM_to_7Seg(1825);
    vTaskDelay(pdMS_TO_TICKS(1000));
    Convert_BPM_to_7Seg(2635);
    vTaskDelay(pdMS_TO_TICKS(1000));
    Convert_BPM_to_7Seg(3645);
    vTaskDelay(pdMS_TO_TICKS(1000));
    Convert_BPM_to_7Seg(6600);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
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
    //configTimer();
    xTaskCreatePinnedToCore(SevenSegmentDisplay_task,"7-Segment", 2000, NULL, 2, &Display_TaskHandle, 0);
    xTaskCreatePinnedToCore(test_task,"test", 2000, NULL, 2, NULL, 0);
    
    //xTaskCreatePinnedToCore(beatMonitor_task, "monitor", 2000, NULL, 5, &beatMonitor_TaskHandle, 1 );
   //xTaskCreatePinnedToCore(userInput_task, "userInput", 2000, NULL, 5, NULL, 1 );
    //xTaskCreatePinnedToCore(LED_task,"LED", 2000, NULL, 2, &LED_TaskHandle, 1);

    while (1) {
        // Yield to let other FreeRTOS tasks run
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
  }
}



