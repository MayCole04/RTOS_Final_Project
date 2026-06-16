#include "interface.h" 

//active high hex for 7 segment displays
const uint8_t DIG_SEGS[14] = { 0x3F, 0x06 , 0x5B , 0x4F , 0x66 , 0x6D , 0x7D ,
0x07 , 0x7F , 0x67,0x3E, 0x73, 0x7C,  0x00 };

SemaphoreHandle_t xMutex1;
TaskHandle_t LED_TaskHandle = NULL;
TaskHandle_t Display_TaskHandle = NULL;
uint32_t digits = 0;                    //Combined variable for all 4 seven sigment digits






void Convert_BPM_to_7Seg(uint16_t bpm){
  uint8_t digit1_temp =0;
  uint8_t digit2_temp =0;
  uint8_t digit3_temp =0;
  uint8_t digit4_temp =0;
  if(bpm == 'b'){
    digit1_temp  =  DIG_SEGS[11];
    digit2_temp = DIG_SEGS[12];
  }
  else{
    //Digit1
    if(bpm == 6600)
      digit1_temp =  DIG_SEGS[11];
    else
      digit1_temp = DIG_SEGS[bpm % 10];
    //Digit 2
    if(bpm == 6600)
      digit2_temp = DIG_SEGS[10];
    else
      digit2_temp = DIG_SEGS[(bpm / 10) % 10];
    //Digit 3
    if(bpm> 99){
        digit3_temp = DIG_SEGS[(bpm / 100) % 10];
      }
    else
      digit3_temp = DIG_SEGS[13];
    //Digit 4
    if(bpm>999){
      digit4_temp = DIG_SEGS[(bpm / 1000) % 10];
      }
    else
      digit4_temp = DIG_SEGS[13];
  }

  xSemaphoreTake(xMutex1, pdMS_TO_TICKS(10));
  digits = (digit4_temp <<24) | (digit3_temp << 16) | (digit2_temp << 8) | (digit1_temp);
  xSemaphoreGive(xMutex1);
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
  printf("Seven seg thread started\n");
  uint32_t currentDigits;
  #define digit1 currentDigits & 0x0FF
  #define digit2 (currentDigits >> 8) & 0x0FF
  #define digit3 (currentDigits >> 16) & 0x0FF
  #define digit4 (currentDigits >> 24) & 0x0FF

  gpio_set_level(digit1_GPIO, 1);
  gpio_set_level(digit2_GPIO, 1);
  gpio_set_level(digit3_GPIO, 1);
  gpio_set_level(digit4_GPIO, 1);
  gpio_set_level(cathodeL_GPIO, 1);
  vTaskDelay(pdMS_TO_TICKS(15));

  for(;;){
    xSemaphoreTake(xMutex1, portMAX_DELAY); //Wait for signal to update digit 1, not hard deadline so wait indefinitely is acceptable
    currentDigits = digits;   
    xSemaphoreGive(xMutex1);
    
      //Digit 1
      LED_control(digit1);
      gpio_set_level(digit1_GPIO, 0);
      /*
       gpio_set_level(digit2_GPIO, 0);
        gpio_set_level(digit3_GPIO, 0);
         gpio_set_level(digit4_GPIO, 0);
         */
      vTaskDelay(pdMS_TO_TICKS(3));
      gpio_set_level(digit1_GPIO, 1);
      /*
      gpio_set_level(digit2_GPIO, 1);
      gpio_set_level(digit3_GPIO, 1);
      gpio_set_level(digit4_GPIO, 1);
      */
    
      //Digit 2
      LED_control(digit2);
      gpio_set_level(digit2_GPIO, 0);
      vTaskDelay(pdMS_TO_TICKS(3));
      gpio_set_level(digit2_GPIO, 1);

      //Colon
      if((digit4) != 0){
        printf("shlould show colon!\n");
        gpio_set_level(anode_A_GPIO, 1);
        gpio_set_level(anode_B_GPIO, 1);
        gpio_set_level(anode_C_GPIO, 0);
        gpio_set_level(cathodeL_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(3));
        gpio_set_level(cathodeL_GPIO, 1);
      }
      //Digit 4
      LED_control(digit4);
      gpio_set_level(digit4_GPIO, 0);
      vTaskDelay(pdMS_TO_TICKS(3));
      gpio_set_level(digit4_GPIO, 1);

      //Digit3
      LED_control((currentDigits >> 16) & 0x0FF);
      gpio_set_level(digit3_GPIO, 0);
      vTaskDelay(pdMS_TO_TICKS(3));
      gpio_set_level(digit3_GPIO, 1);
      
     
    #undef digit1
    #undef digit2
    #undef digit3
    #undef digit4
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
    uint16_t current_pot = 0;
    bool ready = true;
  for(;;){
    uint16_t new_pot = adc1_get_raw(pot_channel); 
    while (gpio_get_level(userInput_GPIO) == 0){
    
      if(current_pot != new_pot){
        ready = false;
        current_pot = new_pot;
        if(current_pot < 684 ){
          Convert_BPM_to_7Seg(1825);

        }
        else if((current_pot> 683) && (current_pot < 1366)){
          Convert_BPM_to_7Seg(2635);


        }
        else if((current_pot > 1365) && (current_pot < 2049)){
          Convert_BPM_to_7Seg(3645);

        }
        else if((current_pot > 2048) && (current_pot < 2731)){
          Convert_BPM_to_7Seg(4655);


        }
        else if((current_pot > 2730) &&(current_pot < 3414)){
          Convert_BPM_to_7Seg(5665);

        }
        else{
          Convert_BPM_to_7Seg(6600);
          
        }
         vTaskDelay(pdMS_TO_TICKS(100));
      }
    }
    
    if(!ready){
      ready = true;
      continue;
    }
    
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


