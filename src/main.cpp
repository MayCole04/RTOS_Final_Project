#include "config.h"
#include "monitor.h"



//#include <esp32-hal-gpio.h>






//TickType_t msToTick(const int ms);
TickType_t msToTick(const int ms){
  return ms / portTICK_PERIOD_MS;
}

void LED_task( void * pvParameters )
{
  for( ;; )
  {
    gpio_set_direction(bulitInLED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(bulitInLED_GPIO, 1);
    
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_set_level(bulitInLED_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }  
}

void builtInLED_task(void * pvParameters){
  for( ;;){
    gpio_set_direction(yellowLED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(yellowLED_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_set_level(yellowLED_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(1000));
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
    timer_start(TIMER_GROUP_0, TIMER_0);
    xTaskCreatePinnedToCore(LED_task,"LED", 1000, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(builtInLED_task,"builtInLED", 1000, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(timeMonitor_task, "monitor", 2000, NULL, 2, NULL, 0 );
  }
  }



