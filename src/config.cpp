#include "config.h"

  
QueueHandle_t   sample_queue;

void configPins(void)
/*
Function to configure GPIO ports and the ADC
*/
/*
{
  
  
  //Seven Segment 
  gpio_pad_select_gpio(digit1_pin);  
  gpio_set_direction(digit1_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_pull_mode(digit1_GPIO, GPIO_PULLUP_ONLY);
  gpio_output_enable(digit1_GPIO);

  gpio_pad_select_gpio(digit2_pin);  
  gpio_set_direction(digit2_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_pull_mode(digit2_GPIO, GPIO_PULLUP_ONLY);
  gpio_output_enable(digit2_GPIO);

  gpio_pad_select_gpio(cathodeL_pin);  
  gpio_set_direction(cathodeL_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_pull_mode(cathodeL_GPIO, GPIO_PULLUP_ONLY);
  gpio_output_enable(cathodeL_GPIO);

  gpio_pad_select_gpio(digit3_pin);  
  gpio_set_direction(digit3_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_pull_mode(digit3_GPIO, GPIO_PULLUP_ONLY);
  gpio_output_enable(digit3_GPIO);

  gpio_pad_select_gpio(digit4_pin);  
  gpio_set_direction(digit4_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_pull_mode(digit4_GPIO, GPIO_PULLUP_ONLY);
  gpio_output_enable(digit4_GPIO);

  gpio_pad_select_gpio(anode_A);  
  gpio_set_direction(anode_A_GPIO, GPIO_MODE_OUTPUT);
  gpio_output_enable(anode_A_GPIO);
  gpio_set_pull_mode(anode_A_GPIO, GPIO_PULLDOWN_ONLY);

  gpio_pad_select_gpio(anode_B);  
  gpio_set_direction(anode_B_GPIO, GPIO_MODE_OUTPUT);
  gpio_output_enable(anode_B_GPIO);
  gpio_set_pull_mode(anode_B_GPIO, GPIO_PULLDOWN_ONLY);

  gpio_pad_select_gpio(anode_C);  
  gpio_set_direction(anode_C_GPIO, GPIO_MODE_OUTPUT);
  gpio_output_enable(anode_C_GPIO);
  gpio_set_pull_mode(anode_C_GPIO, GPIO_PULLDOWN_ONLY);

  gpio_pad_select_gpio(anode_D);  
  gpio_set_direction(anode_D_GPIO, GPIO_MODE_OUTPUT);
  gpio_output_enable(anode_D_GPIO);
  gpio_set_pull_mode(anode_D_GPIO, GPIO_PULLDOWN_ONLY);

  gpio_pad_select_gpio(anode_E);  
  gpio_set_direction(anode_E_GPIO, GPIO_MODE_OUTPUT);
  gpio_output_enable(anode_E_GPIO);
  gpio_set_pull_mode(anode_E_GPIO, GPIO_PULLDOWN_ONLY);

  gpio_pad_select_gpio(anode_F);  
  gpio_set_direction(anode_F_GPIO, GPIO_MODE_OUTPUT);
  gpio_output_enable(anode_F_GPIO);
  gpio_set_pull_mode(anode_F_GPIO, GPIO_PULLDOWN_ONLY);

  gpio_pad_select_gpio(anode_G);  
  gpio_set_direction(anode_G_GPIO, GPIO_MODE_OUTPUT);
  gpio_output_enable(anode_G_GPIO);
  gpio_set_pull_mode(anode_G_GPIO, GPIO_PULLDOWN_ONLY);
  */

{
    gpio_config_t io_conf = {};

    // 1. Configure all Digit pins (Output, Pull-up)
    io_conf.pin_bit_mask = (1ULL << digit1_GPIO) | (1ULL << digit2_GPIO) | 
                           (1ULL << digit3_GPIO) | (1ULL << digit4_GPIO) | 
                           (1ULL << cathodeL_pin);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);

    // 2. Configure all Anode pins (Output, Pull-down)
    io_conf.pin_bit_mask = (1ULL << anode_A_GPIO) | (1ULL << anode_B_GPIO) | 
                           (1ULL << anode_C_GPIO) | (1ULL << anode_D_GPIO) | 
                           (1ULL << anode_E_GPIO) | (1ULL << anode_F_GPIO) | 
                           (1ULL << anode_G_GPIO);
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpio_config(&io_conf);

}
void configOther(void){
  //ADC
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(signalInput_channel, ADC_ATTEN_DB_11);
  gpio_pad_select_gpio(signalInput_pin);  
  gpio_set_direction(signalInput_GPIO, GPIO_MODE_INPUT);
  gpio_input_enable(signalInput_GPIO);

  /*
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(pot_channel, ADC_ATTEN_DB_11);
  gpio_pad_select_gpio(pot_pin);  
  gpio_set_direction(pot_GPIO, GPIO_MODE_INPUT);
  gpio_input_enable(pot_GPIO);
  */
  //Red LED
  gpio_pad_select_gpio(redLED_pin);  
  gpio_set_direction(redLED_GPIO, GPIO_MODE_OUTPUT);
  gpio_output_enable(redLED_GPIO);
  gpio_set_level(redLED_GPIO, 0);    
  //Green LED
  gpio_pad_select_gpio(greenLED_pin); 
  gpio_set_direction(greenLED_GPIO, GPIO_MODE_OUTPUT);
  gpio_output_enable(greenLED_GPIO);
  gpio_set_level(greenLED_GPIO, 0);
  //yellow LED
  gpio_pad_select_gpio(yellowLED_pin); 
  gpio_set_direction(yellowLED_GPIO, GPIO_MODE_OUTPUT);
  gpio_output_enable(yellowLED_GPIO);
  gpio_set_level(yellowLED_GPIO, 0);
  //User input
  
  gpio_pad_select_gpio(userInput_pin); 
  gpio_set_direction(userInput_GPIO, GPIO_MODE_INPUT);
   gpio_set_pull_mode(userInput_GPIO, GPIO_PULLDOWN_ONLY);
  gpio_input_enable(userInput_GPIO);

}

 

void configTimer(void)
/*
Function to configure and enable the timer. Also enables the timer to 
trigger an interupt at the time specified at the top of the file
*/
{
  const timer_config_t interrupt_timer_config = 
  {
    .alarm_en = TIMER_ALARM_EN,
    .counter_en = TIMER_PAUSE, 
    .counter_dir = TIMER_COUNT_UP,
    .auto_reload =TIMER_AUTORELOAD_EN,
    .divider = hw_divider 
    
  };

  timer_init(TIMER_GROUP_0, TIMER_0, &interrupt_timer_config );
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, sample_ticks);      //defined in config.h, sample ticks defines how often we run the ISR
  timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
  timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, heartbeat_timer_callback, NULL, 0);
  
}


