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

//timer
#define hw_divider           2
#define sample_frequency     500
#define sample_ticks         APB_CLK_FREQ/sample_frequency/hw_divider
#define queue_size           10

#define LOW                  0x0
#define HIGH                 0x1
//GPIO FUNCTIONS
#define INPUT                0x01
#define OUTPUT               0x02
//Display functions
#define ON                   0
#define OFF                  1
//pins
#define redLED_pin           27
#define greenLED_pin         12
#define yellowLED_pin        14
#define userInput_pin        26
#define signalInput_pin      36
#define pot_pin              34
//Display Pins
#define digit1_pin           15
#define digit2_pin           0
#define cathodeL_pin         16
#define digit3_pin           5
#define digit4_pin           18
#define anode_D              17
#define anode_E              4
#define anode_DP             2
#define anode_F              23
#define anode_C              22
#define anode_A              1
#define anode_G              3
#define anode_B              21
//Display GPIO
#define digit1_GPIO         GPIO_NUM_15
#define digit2_GPIO         GPIO_NUM_0
#define cathodeL_GPIO        GPIO_NUM_16
#define digit3_GPIO         GPIO_NUM_5
#define digit4_GPIO         GPIO_NUM_18
#define anode_D_GPIO        GPIO_NUM_17
#define anode_E_GPIO        GPIO_NUM_4
#define anode_DP_GPIO        GPIO_NUM_2
#define anode_F_GPIO        GPIO_NUM_23
#define anode_C_GPIO        GPIO_NUM_22
#define anode_A_GPIO        GPIO_NUM_1
#define anode_G_GPIO        GPIO_NUM_3
#define anode_B_GPIO        GPIO_NUM_21

#define redLED_GPIO         GPIO_NUM_27
#define greenLED_GPIO       GPIO_NUM_12
#define yellowLED_GPIO      GPIO_NUM_14
#define userInput_GPIO      GPIO_NUM_26
#define signalInput_GPIO    GPIO_NUM_36
#define pot_GPIO            GPIO_NUM_34
#define pot_channel         ADC1_CHANNEL_6
#define signalInput_channel ADC1_CHANNEL_0


extern QueueHandle_t         sample_queue;
extern bool IRAM_ATTR        heartbeat_timer_callback(void *args);

void configPins(void);
void configOther(void);
void configTimer(void);
    #endif // CONFIG_H