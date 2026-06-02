#include "interface.h" 


TaskHandle_t LED_TaskHandle = NULL;
void LED_task( void * pvParameters )
  /*
    Task to control LEDs. whenever an LED need to flash, it is done by this thread.
    other threads send signals to this thread to determine when to flash and what color.
    If notification bit 1 is set, that measn an LED needs to flashed.
    bits 2-3 determine what color LED to flash in the following way:
    00: red
    01: green
    1x: yellow 
  */

 {
  for( ;; )
  {
    uint32_t LED_notification;
    printf("LED task is running \n");
    xTaskNotifyWait(0, 1, &LED_notification, portMAX_DELAY);
    printf("LED task got notification\n");
    
    if((LED_notification & 0x01) == 1){ //Check if signal to flash led recieved
      printf("in LED task switch\n");
      switch (LED_notification >> 1)
      {
      case 0:                           //flash red led
      { 
        printf("Red LED should flash \n");
        gpio_set_level(redLED_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(50));
        gpio_set_level(redLED_GPIO, 0);
        break;
      }
      case 1:{                           //flash green led
        printf("green LED should flash \n");
        gpio_set_level(greenLED_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(50));
        gpio_set_level(greenLED_GPIO, 0);
        break;
      }
      default:{                           //flash yellow led
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
  /*
    Task to monitor user input. when a button is pressed,
    this thread starts the process to begin reading heartbeat.
    It send a signal the LED thread to use yellow LED, and creates the calulate BPM task.
    Finally, it will start the timer which allow heatbeat readings to start.
  */
  printf("Input thread running \n");
  for(;;){
    if(gpio_get_level(userInput_GPIO) == 1){         //start process to read bpm if user input recived
        printf("Starting Heartbeat readings\n");
        if(calculateBPM_TaskHandle ==NULL){          // start running calculate task
            xTaskCreatePinnedToCore(calculateBPM_task, "BPM", 2000, NULL, 4, &calculateBPM_TaskHandle, 0 );
            printf("created calculate task \n");
        }
        xTaskNotify(calculateBPM_TaskHandle, 0x80000000, eSetBits);
        timer_start(TIMER_GROUP_0, TIMER_0);
        xTaskNotify(LED_TaskHandle, 4, eSetBits);
        vTaskDelay(pdMS_TO_TICKS(15100));            // wait for reading to be done to start again
     // }
    }
    else
    vTaskDelay(pdMS_TO_TICKS(100));                  // periodically check for user input 
  }
}
