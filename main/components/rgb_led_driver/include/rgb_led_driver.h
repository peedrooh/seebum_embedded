#ifndef RGB_LED_DRIVER_H
#define RGB_LED_DRIVER_H

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "hal/gpio_types.h"
#include "hal/ledc_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RGB_TIMER LEDC_TIMER_1
#define RGB_MODE LEDC_LOW_SPEED_MODE
#define R_PIN GPIO_NUM_48
#define G_PIN GPIO_NUM_47
#define B_PIN GPIO_NUM_21
#define R_CHANNEL LEDC_CHANNEL_4
#define G_CHANNEL LEDC_CHANNEL_5
#define B_CHANNEL LEDC_CHANNEL_6
#define RGB_PWM_RES LEDC_TIMER_8_BIT
#define RGB_FREQUENCY (4000)

esp_err_t rgb_led_init(void);
esp_err_t set_rgb_color(unsigned long hexcode);
void task_led_blink(void *pvParameters);

#endif
