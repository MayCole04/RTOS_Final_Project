#include "config.h"

  
QueueHandle_t         sample_queue;

void configPins(void)
/*
Function to configure GPIO ports and the ADC
*/
{
  //ADC
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(signalInput_channel, ADC_ATTEN_DB_11);
  gpio_pad_select_gpio(signalInput_pin);  
  gpio_set_direction(signalInput_GPIO, GPIO_MODE_INPUT);
  gpio_input_enable(signalInput_GPIO);
  //yellow LED
  gpio_pad_select_gpio(yellowLED_pin);  
  gpio_set_direction(yellowLED_GPIO, GPIO_MODE_OUTPUT);
  gpio_output_enable(yellowLED_GPIO);
  gpio_set_level(yellowLED_GPIO, 0);    
  //built in LED
  gpio_pad_select_gpio(bulitInLED_pin); 
  gpio_set_direction(bulitInLED_GPIO, GPIO_MODE_OUTPUT);
  gpio_output_enable(bulitInLED_GPIO);
  gpio_set_level(bulitInLED_GPIO, 0);
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


