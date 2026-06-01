#include "interface.h" 


TaskHandle_t LED_TaskHandle = NULL;
void LED_task( void * pvParameters )
{
  for( ;; )
  {
    uint32_t LED_notification;
    printf("LED task is running \n");
    xTaskNotifyWait(0, 1, &LED_notification, portMAX_DELAY);
    printf("LED task got notification\n");
    
    if((LED_notification & 0x01) == 1){
      printf("in LED task switch\n");
      switch (LED_notification >> 1)
      {
      case 0:
      { 
      printf("Red LED should flash \n");
      gpio_set_level(redLED_GPIO, 1);
      vTaskDelay(pdMS_TO_TICKS(50));
      gpio_set_level(redLED_GPIO, 0);
      break;
      }
      case 1:{
        printf("green LED should flash \n");
      gpio_set_level(greenLED_GPIO, 1);
      vTaskDelay(pdMS_TO_TICKS(50));
      gpio_set_level(greenLED_GPIO, 0);
      break;
      }
      default:{
        printf("yellow LED should flash \n");
      gpio_set_level(yellowLED_GPIO, 1);
      vTaskDelay(pdMS_TO_TICKS(50));
      gpio_set_level(yellowLED_GPIO, 0);
      break;
      }
      }
      printf("beat\n");
    }
  }  
}


void userInput_task(void * pvParameters){
  printf("Input thread running \n");
  for(;;){
    if(gpio_get_level(userInput_GPIO) == 1){
      vTaskDelay(pdMS_TO_TICKS(10));
      //if(gpio_get_level(userInput_GPIO) == 1){
        printf("Starting Heartbeat readings\n");
        xTaskNotify(LED_TaskHandle, 4, eSetBits);
        if(calculateBPM_TaskHandle ==NULL){
            xTaskCreatePinnedToCore(calculateBPM_task, "BPM", 2000, NULL, 4, &calculateBPM_TaskHandle, 0 );
            printf("created calculate task \n");
        }
        xTaskNotify(calculateBPM_TaskHandle, 0x80000000, eSetBits);
        timer_start(TIMER_GROUP_0, TIMER_0);
        vTaskDelay(pdMS_TO_TICKS(3000));
        vTaskDelay(pdMS_TO_TICKS(3000));
        vTaskDelay(pdMS_TO_TICKS(3000));
        vTaskDelay(pdMS_TO_TICKS(3000));
        vTaskDelay(pdMS_TO_TICKS(3000));
     // }
    }
    else
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
