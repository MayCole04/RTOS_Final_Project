#pragma once

#ifndef CONFIG_H
#define CONFIG_H

#pragma once
#include <stdio.h>
#include <driver/gpio.h>
#include <rom/gpio.h>
#include <esp_private/gpio.h>
#include <driver/adc.h>
#include <soc/adc_channel.h>
#include <driver/adc.h>
#include "esp_system.h"
#include <freertos/FreeRTOS.h>
#include <freertos/mpu_wrappers.h>
#include "freertos/task.h"
#include <driver/timer.h>



#define LOW                  0x0
#define HIGH                 0x1
//GPIO FUNCTIONS
#define INPUT                0x01
#define OUTPUT               0x02
//pins
#define bulitInLED_pin       2
#define yellowLED_pin        23
#define signalInput_pin      36
//timer
#define hw_divider           2
#define sample_frequency     500
#define sample_ticks         APB_CLK_FREQ/sample_frequency/hw_divider
#define queue_size           10


#define bulitInLED_GPIO      GPIO_NUM_2
#define yellowLED_GPIO       GPIO_NUM_23
#define signalInput_GPIO     GPIO_NUM_36
#define signalInput_channel  ADC1_CHANNEL_0

extern QueueHandle_t         sample_queue;
extern bool IRAM_ATTR        heartbeat_timer_callback(void *args);

void configPins(void);
void configTimer(void);
    #endif // CONFIG_H