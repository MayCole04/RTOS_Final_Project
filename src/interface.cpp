#include "interface.h" 

//active high hex for 7 segment displays
const uint8_t DIG_SEGS[14] = { 0x3F, 0x06 , 0x5B , 0x4F , 0x66 , 0x6D , 0x7D ,
0x07 , 0x7F , 0x67,0x3E, 0x73, 0x7C,  0x00 };

SemaphoreHandle_t xMutex1 = NULL;
TaskHandle_t LED_TaskHandle = NULL;
TaskHandle_t button_TaskHandle = NULL;
TaskHandle_t pot_TaskHandle = NULL;
TaskHandle_t Display_TaskHandle = NULL;
uint32_t digits = 0;                    //Combined variable for all 4 seven segment digits






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
      vTaskDelay(pdMS_TO_TICKS(3));
      gpio_set_level(digit1_GPIO, 1);
      
    
      //Digit 2
      LED_control(digit2);
      gpio_set_level(digit2_GPIO, 0);
      vTaskDelay(pdMS_TO_TICKS(3));
      gpio_set_level(digit2_GPIO, 1);

      //Colon
      if((digit4) != 0){
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
      LED_control(digit3);
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



void button_task(void * pvParameters)
  /**************************************************************************************** 
    Task to monitor user input. when a button is pressed,
    this thread starts the process to begin reading heartbeat.
    It send a signal the LED thread to use yellow LED, and creates the calulate BPM task.
    Finally, it will start the timer which allow heatbeat readings to start.
  *****************************************************************************************/
 /*
 {
    uint16_t current_pot = 0;
    bool ready = true;
    bool release = true;
    uint8_t ageRange = 0;
    //Convert_BPM_to_7Seg(0);

  for(;;){
    uint16_t new_pot = adc1_get_raw(pot_channel); 
   if(gpio_get_level(userInput_GPIO) == 0){
      release = true;
      if(current_pot != new_pot){
        printf("pot value: %d \n", new_pot);
        ready = false;
        xTaskNotify(colorChange_TaskHandle, 0x100, eSetValueWithOverwrite);
        current_pot = new_pot;
        if(current_pot < 100 ){
          Convert_BPM_to_7Seg(6600);
          ageRange = 0;
        }
        else if((current_pot> 100) && (current_pot < 200)){
          Convert_BPM_to_7Seg(5665);
          ageRange = 1;
        }
        else if((current_pot > 200) && (current_pot < 300)){
          Convert_BPM_to_7Seg(4655);
          ageRange = 2;
        }
        else if((current_pot > 300) && (current_pot < 400)){
          Convert_BPM_to_7Seg(3645);
          ageRange = 3;

        }
        else if((current_pot > 400) &&(current_pot < 500)){
          Convert_BPM_to_7Seg(2635);
          ageRange = 4;
        }
        else{
          Convert_BPM_to_7Seg(1825);
          ageRange = 5;
        }
         vTaskDelay(pdMS_TO_TICKS(50));
      }
    }
    else{
      if(!ready){
        release = false;
        ready = true;
        xTaskNotify(colorChange_TaskHandle, ageRange, eSetValueWithOverwrite);
        vTaskDelay(pdMS_TO_TICKS(10));
      }

      if(release){         //start process to read bpm if user input recived
        printf("Starting Heartbeat readings\n");
        if(calculateBPM_TaskHandle ==NULL){          // start running calculate task
            xTaskCreatePinnedToCore(calculateBPM_task, "BPM", 3000, NULL, 4, &calculateBPM_TaskHandle, 1 );
            printf("created calculate task \n");
        }
        timer_start(TIMER_GROUP_0, TIMER_0);
        xTaskNotify(LED_TaskHandle, 4, eSetBits);
        xTaskNotify(calculateBPM_TaskHandle, 0x80000000, eSetBits);
        vTaskDelay(pdMS_TO_TICKS(15100));            // wait for reading to be done to start again
      }
      else
        vTaskDelay(pdMS_TO_TICKS(50));                  // periodically check for user input 
    }
  }  
}
  */
 {
  /*uint16_t current_pot = 0;
  uint8_t ageRange = 0;
  
  // State tracking flags
  bool age_locked = false; 
  bool last_button_state = 0; // Assuming active-low button (1 = unpressed, 0 = pressed)
  
  // Deadband to ignore ESP32 ADC electrical noise 
  const int ADC_DEADBAND = 15; 

  for(;;) {
    // --------------------------------------------------------
    // 1. Handle Potentiometer (Independent of the button)
    // --------------------------------------------------------
    uint16_t new_pot = adc1_get_raw(pot_channel); 
    
    // Check if the pot moved significantly (ignoring tiny noise fluctuations)
    if (abs((int)new_pot - (int)current_pot) > ADC_DEADBAND) {
      current_pot = new_pot;
      age_locked = false; // Pot was moved, so unlock the state
      
      xTaskNotify(colorChange_TaskHandle, 0x100, eSetValueWithOverwrite);
      
      if (current_pot < 100) {
        Convert_BPM_to_7Seg(6600);
        ageRange = 0;
      } else if (current_pot < 200) {
        Convert_BPM_to_7Seg(5665);
        ageRange = 1;
      } else if (current_pot < 300) {
        Convert_BPM_to_7Seg(4655);
        ageRange = 2;
      } else if (current_pot < 400) {
        Convert_BPM_to_7Seg(3645);
        ageRange = 3;
      } else if (current_pot < 500) {
        Convert_BPM_to_7Seg(2635);
        ageRange = 4;
      } else {
        Convert_BPM_to_7Seg(1825);
        ageRange = 5;
      }
    }*/

    // --------------------------------------------------------
    // 2. Handle Pushbutton
    // --------------------------------------------------------
     bool last_button_state = 0;
    for(;;) {
    bool current_button_state = gpio_get_level(userInput_GPIO);
    //uint32_t button_notification;
   
    //xTaskNotifyWait(0, 0, &button_notification, 0);
   // #define age_locked !(button_notification & 0x01)
    
    if (last_button_state == 0 && current_button_state == 1) {
      
      // Tiny software debounce delay
      vTaskDelay(pdMS_TO_TICKS(20)); 
      
      // Confirm it's still pressed after debounce
      if (gpio_get_level(userInput_GPIO) == 1) { 
        
        /*if (!age_locked) {
          // Action 1: Lock in the age
         // button_notification = 0;
          //ulTaskNotifyValueClear( NULL, 1 );
         // xTaskNotify(pot_TaskHandle, 1, eSetValueWithOverwrite);
         // xTaskNotify(colorChange_TaskHandle, ageRange, eSetValueWithOverwrite);
          //printf("Age locked in: %d\n", ageRange);
        } 
        else {. */
          // Action 2: Start Heartbeat (Age was already locked)
          printf("Starting Heartbeat readings\n");
          
          if (calculateBPM_TaskHandle == NULL) {
            xTaskCreatePinnedToCore(calculateBPM_task, "BPM", 3000, NULL, 4, &calculateBPM_TaskHandle, 1);
            printf("created calculate task \n");
          }
          
          timer_start(TIMER_GROUP_0, TIMER_0);
          xTaskNotify(LED_TaskHandle, 4, eSetBits);
          xTaskNotify(calculateBPM_TaskHandle, 0x80000000, eSetBits);
          
          // Wait for reading to be done
          vTaskDelay(pdMS_TO_TICKS(15100)); 
        }
        last_button_state = current_button_state;
      }
      
    }
    
    // Update button state for the next loop
    

    // Loop delay to yield to FreeRTOS scheduler and avoid Watchdog timeouts
    vTaskDelay(pdMS_TO_TICKS(50)); 
   // #undef age_locked
  }  



void pot_task(void * pvParameters)
    {
  uint16_t current_pot = 0;
  uint8_t ageRange = 0;
  
  // State tracking flags
  bool age_locked = false; 
  bool last_button_state = 0; // Assuming active-low button (1 = unpressed, 0 = pressed)
  
  // Deadband to ignore ESP32 ADC electrical noise 
  const int ADC_DEADBAND = 15; 

  for(;;) {
    // --------------------------------------------------------
    // 1. Handle Potentiometer (Independent of the button)
    // --------------------------------------------------------
    uint16_t new_pot = adc1_get_raw(pot_channel); 
    
    // Check if the pot moved significantly (ignoring tiny noise fluctuations)
    if (abs((int)new_pot - (int)current_pot) > ADC_DEADBAND) {
      current_pot = new_pot;
      age_locked = false; // Pot was moved, so unlock the state
      xTaskNotify(button_TaskHandle, 1, eSetValueWithOverwrite);
      xTaskNotify(colorChange_TaskHandle, 0x100, eSetValueWithOverwrite);
      
      if (current_pot < 100) {
        Convert_BPM_to_7Seg(6600);
        ageRange = 0;
      } else if (current_pot < 200) {
        Convert_BPM_to_7Seg(5665);
        ageRange = 1;
      } else if (current_pot < 300) {
        Convert_BPM_to_7Seg(4655);
        ageRange = 2;
      } else if (current_pot < 400) {
        Convert_BPM_to_7Seg(3645);
        ageRange = 3;
      } else if (current_pot < 500) {
        Convert_BPM_to_7Seg(2635);
        ageRange = 4;
      } else {
        Convert_BPM_to_7Seg(1825);
        ageRange = 5;
      }
    }
    else{
      uint32_t pot_notification;
      xTaskNotifyWait(0, 1, &pot_notification, 0);
      if(pot_notification == 1){
        xTaskNotify(colorChange_TaskHandle, ageRange, eSetValueWithOverwrite);
      }
      vTaskDelay(pdMS_TO_TICKS(50)); 
    }
  }
}



