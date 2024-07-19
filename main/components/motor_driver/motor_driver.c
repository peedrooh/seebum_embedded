#include "motor_driver.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "hal/ledc_types.h"
#include "soc/clk_tree_defs.h"
#include <stdint.h>

void motor_driver_init() {
  // Set motor 1 pins direction and motor ratational direction
  gpio_set_direction(AIN11_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(AIN12_PIN, GPIO_MODE_OUTPUT);
  gpio_set_level(AIN11_PIN, MOTOR_1_CW);
  gpio_set_level(AIN12_PIN, !MOTOR_1_CW);

  // Set motor 2 pins direction and motor ratational direction
  gpio_set_direction(AIN21_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(AIN22_PIN, GPIO_MODE_OUTPUT);
  gpio_set_level(AIN21_PIN, MOTOR_2_CW);
  gpio_set_level(AIN22_PIN, !MOTOR_2_CW);

  // PWM configuration
  ledc_channel_config_t pwma1_configs = {0};
  pwma1_configs.speed_mode = PWM_MODE;
  pwma1_configs.channel = PWMA1;
  pwma1_configs.timer_sel = PWM_TIMER;
  pwma1_configs.intr_type = LEDC_INTR_DISABLE;
  pwma1_configs.gpio_num = PWMA1_PIN;
  pwma1_configs.duty = 0;
  ledc_channel_config_t pwma2_configs = {0};
  pwma2_configs.speed_mode = PWM_MODE;
  pwma2_configs.channel = PWMA2;
  pwma2_configs.timer_sel = PWM_TIMER;
  pwma2_configs.intr_type = LEDC_INTR_DISABLE;
  pwma2_configs.gpio_num = PWMA2_PIN;
  pwma2_configs.duty = 0;
  ESP_ERROR_CHECK(ledc_channel_config(&pwma1_configs));
  ESP_ERROR_CHECK(ledc_channel_config(&pwma2_configs));

  ledc_timer_config_t pwm_timer_configs = {0};
  pwm_timer_configs.speed_mode = PWM_MODE;
  pwm_timer_configs.freq_hz = PWM_FREQUENCY;
  pwm_timer_configs.duty_resolution = PWM_DUTY_RES;
  pwm_timer_configs.timer_num = PWM_TIMER;
  ESP_ERROR_CHECK(ledc_timer_config(&pwm_timer_configs));
}

esp_err_t change_motor_duty(uint8_t duty_percentage, ledc_channel_t channel) {
  static const char *CHANGE_PWM_DUTY = "CHANGE_PWM_DUTY";

  if (duty_percentage > 100) {
    ESP_LOGE(
        CHANGE_PWM_DUTY,
        "The duty cycle must be an integer betwwen 0 and 100 (percentage).");
    return ESP_FAIL;
  }
  if (channel != PWMA1 && channel != PWMA2) {
    ESP_LOGE(CHANGE_PWM_DUTY, "The channel must be either PWMA1 or PWMA2");
    return ESP_FAIL;
  }
  uint32_t duty = (1024) * duty_percentage / 100;
  ledc_set_duty(PWM_MODE, channel, duty);
  ledc_update_duty(PWM_MODE, channel);

  return ESP_OK;
}

void test_motor_task() {
  while (1) {
    ESP_LOGI("TEST MOTOR", "Duty 25");
    change_motor_duty(25, PWMA1);
    vTaskDelay(pdMS_TO_TICKS(3000));
    ESP_LOGI("TEST MOTOR", "Duty 50");
    change_motor_duty(50, PWMA1);
    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}
