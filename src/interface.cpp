#include "interface.h" 

SemaphoreHandle_t xMutex1;
SemaphoreHandle_t xMutex2;
SemaphoreHandle_t xMutex3;
SemaphoreHandle_t xMutex4;

//active high hex for 7 segment displays
uint8_t DIG_SEGS[11] = { 0x3F, 0x06 , 0x5B , 0x4F , 0x66 , 0x6D , 0x7D ,
0x07 , 0x7F , 0x67, 0x00 };


TaskHandle_t LED_TaskHandle = NULL;

uint8_t digit1 = 0;
uint8_t digit2 = 0;
uint8_t digit3 = 0;
uint8_t digit4 = 0;

void Convert_BPM_to_7Seg(uint16_t bpm){
  digit1 = DIG_SEGS[bpm % 10];
  digit2 = DIG_SEGS[(bpm / 10) % 10];
  if(bpm> 99)
    digit3 = DIG_SEGS[(bpm / 100) % 10];
  else
    digit3 = DIG_SEGS[11];
  if(bpm>1000)
    digit4 = DIG_SEGS[(bpm / 1000) % 10];
  else
    digit4 = DIG_SEGS[11];
    }

void LED_control(uint8_t digit)
{
  gpio_set_level(anode_A_GPIO, (digit & 0x01));
  gpio_set_level(anode_B_GPIO, (digit & 0x02) >> 1);
  gpio_set_level(anode_C_GPIO, (digit & 0x04) >> 2);
  gpio_set_level(anode_D_GPIO, (digit & 0x08) >> 3);
  gpio_set_level(anode_E_GPIO, (digit & 0x10) >> 4);
  gpio_set_level(anode_F_GPIO, (digit & 0x20) >> 5);
  gpio_set_level(anode_G_GPIO, (digit & 0x40) >> 6);
  gpio_set_level(anode_DP_GPIO, (digit & 0x80) >> 7);
}

void SevenSegmentDisplay_task(void * pvParameters)
{
  xMutex1 = xSemaphoreCreateMutex();
  xMutex2 = xSemaphoreCreateMutex();
  xMutex3 = xSemaphoreCreateMutex();
  xMutex4 = xSemaphoreCreateMutex();
  gpio_set_level(digit1_GPIO, 1);
  gpio_set_level(digit2_GPIO, 1);
  gpio_set_level(digit3_GPIO, 1);
  gpio_set_level(digit4_GPIO, 1);
  for(;;)
  {
     xSemaphoreTake(xMutex1, portMAX_DELAY);   //Wait for signal to update digit 1, not hard deadline so wait indefinitely is acceptable
      LED_control(digit1);
      printf("Digit 1: %d\n", digit1);
      xSemaphoreGive(xMutex1);
      gpio_set_level(digit1_GPIO, 0);
      printf("updated digit 1\n");
      vTaskDelay(pdMS_TO_TICKS(3));
      gpio_set_level(digit1_GPIO, 1);
    
      

      xSemaphoreTake(xMutex2, portMAX_DELAY);
      LED_control(digit2);
      xSemaphoreGive(xMutex2);
      gpio_set_level(digit2_GPIO, 0);
      vTaskDelay(pdMS_TO_TICKS(6));
      gpio_set_level(digit2_GPIO, 1);
      

      xSemaphoreTake(xMutex3, portMAX_DELAY);
      LED_control(digit3);
       xSemaphoreGive(xMutex3);
      gpio_set_level(digit3_GPIO, 0);
      vTaskDelay(pdMS_TO_TICKS(6));
      gpio_set_level(digit3_GPIO, 1);
     

      
      xSemaphoreTake(xMutex4, portMAX_DELAY);
      LED_control(digit4);
      xSemaphoreGive(xMutex4);
      gpio_set_level(digit4_GPIO, 0);
      printf("updated digit 4\n");
      vTaskDelay(pdMS_TO_TICKS(3));
      gpio_set_level(digit4_GPIO, 1);
      
  }
}

void LED_task( void * pvParameters )
  /****************************************************************************************** 
    Task to control LEDs. whenever an LED need to flash, it is done by this thread.
    other threads send signals to this thread to determine when to flash and what color.
    If notification bit 0 is set, that measn an LED needs to flashed.
    bits 1-2 determine what color LED to flash in the following way:
    00: red
    01: green
    1x: yellow 
  *******************************************************************************************/
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



void userInput_task(void * pvParameters)
  /**************************************************************************************** 
    Task to monitor user input. when a button is pressed,
    this thread starts the process to begin reading heartbeat.
    It send a signal the LED thread to use yellow LED, and creates the calulate BPM task.
    Finally, it will start the timer which allow heatbeat readings to start.
  *****************************************************************************************/
 {
  printf("Input thread running \n");
  for(;;){
    if(gpio_get_level(userInput_GPIO) == 1){         //start process to read bpm if user input recived
        printf("Starting Heartbeat readings\n");
        if(calculateBPM_TaskHandle ==NULL){          // start running calculate task
            xTaskCreatePinnedToCore(calculateBPM_task, "BPM", 2000, NULL, 4, &calculateBPM_TaskHandle, 0 );
            printf("created calculate task \n");
        }
        timer_start(TIMER_GROUP_0, TIMER_0);
        xTaskNotify(LED_TaskHandle, 4, eSetBits);
        xTaskNotify(calculateBPM_TaskHandle, 0x80000000, eSetBits);
        vTaskDelay(pdMS_TO_TICKS(15100));            // wait for reading to be done to start again
    }
    else
      vTaskDelay(pdMS_TO_TICKS(100));                  // periodically check for user input 
  }
}
