#include "config.h"
#include "monitor.h"




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



extern "C" {
  void app_main(void){
    sample_queue = xQueueCreate(queue_size, sizeof(uint16_t)); 
    while(sample_queue == NULL){
      printf("sample queue is NULL\n");
    }

    configPins();
    configTimer();
    xTaskCreatePinnedToCore(test_task,"test", 2000, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(beatMonitor_task, "monitor", 2000, NULL, 5, &beatMonitor_TaskHandle, 1 );
    xTaskCreatePinnedToCore(userInput_task, "userInput", 2000, NULL, 5, NULL, 1 );
    xTaskCreatePinnedToCore(LED_task,"LED", 2000, NULL, 2, &LED_TaskHandle, 1);

    while (1) {
        // Yield to let other FreeRTOS tasks run
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
  
  }
}



