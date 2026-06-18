#include "config.h"
#include "monitor.h"
#include "esp_log.h"



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
    Convert_BPM_to_7Seg(adc * 3300 / 4095);
    //printf("%d", adc * 950 / 4095 );
    vTaskDelay(pdMS_TO_TICKS(100));
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


static const char *TAG = "MAIN_DEBUG";
/*
extern "C" {
  void app_main(void){
    ESP_LOGI(TAG, "--- APP_MAIN STARTED ---");
    vTaskDelay(pdMS_TO_TICKS(3000));
    ESP_LOGI(TAG, "before smaple queue");
    sample_queue = xQueueCreate(queue_size, sizeof(uint16_t)); 
    while(sample_queue == NULL){
      printf("sample queue is NULL\n");
    } 
    ESP_LOGI(TAG, "---sample queue created----");
    xMutex1 = xSemaphoreCreateMutex();
    configPins();
    vTaskDelay(pdMS_TO_TICKS(5)); 
    configOther();
    configTimer();
    ESP_LOGI(TAG, "Hardware initialized.");
   
    xTaskCreatePinnedToCore(LED_task,"LED", 3000, NULL, 2, &LED_TaskHandle, 0);
    ESP_LOGI(TAG, "LED_task.");
   xTaskCreatePinnedToCore(SevenSegmentDisplay_task,"7-Segment", 3000, NULL, 2, &Display_TaskHandle, 1);
    ESP_LOGI(TAG, "Display_task.");
    xTaskCreatePinnedToCore(colorChange_task, "color", 2000, NULL, 5, &colorChange_TaskHandle, 1 );
    ESP_LOGI(TAG, "color_task.");
    xTaskCreatePinnedToCore(pot_task, "pot", 3000, NULL, 3, &pot_TaskHandle, 0 );
    ESP_LOGI(TAG, "pot_task.");
    xTaskCreatePinnedToCore(button_task, "button", 3000, NULL, 3, &button_TaskHandle, 1 );
    ESP_LOGI(TAG, "button_task.");
    
    //xTaskCreatePinnedToCore(test_task,"test", 2000, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(beatMonitor_task, "monitor", 3000, NULL, 5, &beatMonitor_TaskHandle, 0 );
    ESP_LOGI(TAG, "monitor_task.");


    while (1) {
        // Yield to let other FreeRTOS tasks run
        vTaskDelay(pdMS_TO_TICKS(1000)); 
        ESP_LOGI(TAG, "in loop");
    }
  }
}. */

extern "C" {
  void app_main(void){
    ESP_LOGI(TAG, "--- APP_MAIN STARTED ---");
    vTaskDelay(pdMS_TO_TICKS(3000));
    
    ESP_LOGI(TAG, "1. Attempting to create Mutex...");
    vTaskDelay(pdMS_TO_TICKS(50)); // Wait for text to send
    xMutex1 = xSemaphoreCreateMutex();
    
    ESP_LOGI(TAG, "2. Mutex created! Attempting to config pins...");
    vTaskDelay(pdMS_TO_TICKS(50)); // Wait for text to send
    configPins();
    
    ESP_LOGI(TAG, "3. Pins configured! We survived!");
    vTaskDelay(pdMS_TO_TICKS(50));
    
    while(1) { vTaskDelay(1000); }
}
}



