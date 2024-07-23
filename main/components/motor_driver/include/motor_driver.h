#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include <stdio.h>

#define AIN1_PIN GPIO_NUM_39
#define AIN1_PWM LEDC_CHANNEL_0
#define AIN2_PIN GPIO_NUM_40
#define AIN2_PWM LEDC_CHANNEL_1
#define MOTOR_A_CLOCKWISE 1

#define BIN1_PIN GPIO_NUM_37
#define BIN1_PWM LEDC_CHANNEL_2
#define BIN2_PIN GPIO_NUM_38
#define BIN2_PWM LEDC_CHANNEL_3
#define MOTOR_B_CLOCKWISE 1

#define PWM_TIMER LEDC_TIMER_0
#define PWM_MODE LEDC_LOW_SPEED_MODE
#define PWM_DUTY_RES LEDC_TIMER_10_BIT
#define PWM_FREQUENCY (4000)

esp_err_t motor_driver_init();
esp_err_t control_motor(bool is_motor_a, uint8_t duty_percentage,
                        bool is_clockwise);
void test_motor_task();

#endif
