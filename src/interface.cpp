#include "interface.h" 
TaskHandle_t LED_TaskHandle = NULL;
void LED_task( void * pvParameters )
{
  for( ;; )
  {
    uint32_t LED_notification;
    xTaskNotifyWait(0, 1, &LED_notification, portMAX_DELAY);
    if((LED_notification & 0x01) == 1){
      if((LED_notification >> 1) == 0 ){
        gpio_set_level(redLED_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
        gpio_set_level(redLED_GPIO, 0);
      }
      else{
        gpio_set_level(greenLED_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
        gpio_set_level(greenLED_GPIO, 0);
      }
    }
  }  
}