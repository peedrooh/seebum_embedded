#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include <stdio.h>

// Motor pin definitions
#define AIN11_PIN GPIO_NUM_21
#define AIN12_PIN GPIO_NUM_47
#define AIN21_PIN GPIO_NUM_14
#define AIN22_PIN GPIO_NUM_13
// Motor direction
#define MOTOR_1_CW 1
#define MOTOR_2_CW 1
// PWM Timer configuration
#define PWM_TIMER LEDC_TIMER_0
#define PWM_MODE LEDC_LOW_SPEED_MODE
#define PWM_DUTY_RES LEDC_TIMER_10_BIT // Set duty resolution to 13 bits
#define PWM_FREQUENCY (100) // Frequency in Hertz. Set frequency at 4 kHz
// PWM Channel configuration
#define PWMA1 LEDC_CHANNEL_0
#define PWMA2 LEDC_CHANNEL_1
// PWM GPIO configuration
#define PWMA1_PIN GPIO_NUM_48
#define PWMA2_PIN GPIO_NUM_12

void motor_driver_init();
esp_err_t change_motor_duty(uint8_t duty_percentage, ledc_channel_t channel);
void test_motor_task();
