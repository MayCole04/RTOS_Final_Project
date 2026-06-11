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
    printf("test task is running\n");

    vTaskDelay(pdMS_TO_TICKS(500));
  
    gpio_pad_select_gpio(digit1_pin);  
    gpio_set_direction(digit1_GPIO, GPIO_MODE_OUTPUT);
    gpio_output_enable(digit1_GPIO);

    gpio_pad_select_gpio(anode_DP);  
    gpio_set_direction(anode_DP_GPIO, GPIO_MODE_OUTPUT);
    gpio_output_enable(anode_DP_GPIO);

    gpio_set_level(digit1_GPIO, 0);
        gpio_set_level(anode_DP_GPIO, 1);
         vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(digit1_GPIO, 1);
        
  //Convert_BPM_to_7Seg(1234);
  
  
  //  vTaskDelay(pdMS_TO_TICKS(500));
   // gpio_set_direction(GPIO_NUM_18, GPIO_MODE_OUTPUT);
   // gpio_output_enable(GPIO_NUM_18);
    //gpio_set_level(GPIO_NUM_18, 1);

  }
}



extern "C" {
  void app_main(void){
    sample_queue = xQueueCreate(queue_size, sizeof(uint16_t)); 
    while(sample_queue == NULL){
      printf("sample queue is NULL\n");
    }

    //configPins();
    configTimer();
    xTaskCreatePinnedToCore(test_task,"test", 2000, NULL, 2, NULL, 0);
    //xTaskCreatePinnedToCore(SevenSegmentDisplay_task,"7-Segment", 2000, NULL, 2, NULL, 0);
    //xTaskCreatePinnedToCore(beatMonitor_task, "monitor", 2000, NULL, 5, &beatMonitor_TaskHandle, 1 );
   //xTaskCreatePinnedToCore(userInput_task, "userInput", 2000, NULL, 5, NULL, 1 );
    //xTaskCreatePinnedToCore(LED_task,"LED", 2000, NULL, 2, &LED_TaskHandle, 1);

    while (1) {
        // Yield to let other FreeRTOS tasks run
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
  
  }
}



