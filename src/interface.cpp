#include "interface.h" 
TaskHandle_t LED_TaskHandle = NULL;
void LED_task( void * pvParameters )
{
  for( ;; )
  {
    uint32_t LED_notification;
    xTaskNotifyWait(0, 1, &LED_notification, portMAX_DELAY);
    
    if((LED_notification & 0x01) == 1){
      switch (LED_notification >> 1)
      {
      case 0:
      { 
      gpio_set_level(redLED_GPIO, 1);
      vTaskDelay(pdMS_TO_TICKS(200));
      gpio_set_level(redLED_GPIO, 0);
      break;
      }
      case 1:{
      gpio_set_level(greenLED_GPIO, 1);
      vTaskDelay(pdMS_TO_TICKS(200));
      gpio_set_level(greenLED_GPIO, 0);
      break;
      }
      default:{
      gpio_set_level(yellowLED_GPIO, 1);
      vTaskDelay(pdMS_TO_TICKS(200));
      gpio_set_level(yellowLED_GPIO, 0);
      break;
      }
      }
      printf("beat\n");
    }
  }  
}


void userInput_task(void * pvParameters){
  for(;;){
    if(gpio_get_level(userInput_GPIO) == 1){
       timer_start(TIMER_GROUP_0, TIMER_0);
      //xTaskNotify(beatMonitor_TaskHandle, 1, eSetBits);
      vTaskDelay(pdMS_TO_TICKS(15000));
      xTaskNotify(LED_TaskHandle, 4, eSetBits);
      printf("Starting Heartbeat readings\n");
    }
    else
    vTaskDelay(pdMS_TO_TICKS(100));
  }

}