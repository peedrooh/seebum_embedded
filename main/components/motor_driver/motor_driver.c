#include "motor_driver.h"

esp_err_t motor_driver_init() {
  // Motor A PWM configuration
  ledc_channel_config_t ain1_config = {0};
  ain1_config.speed_mode = PWM_MODE;
  ain1_config.timer_sel = PWM_TIMER;
  ain1_config.intr_type = LEDC_INTR_DISABLE;
  ain1_config.duty = 0;
  ain1_config.channel = AIN1_PWM;
  ain1_config.gpio_num = AIN1_PIN;
  if (ledc_channel_config(&ain1_config) != ESP_OK)
    return ESP_FAIL;

  ledc_channel_config_t ain2_config = {0};
  ain2_config.speed_mode = PWM_MODE;
  ain2_config.timer_sel = PWM_TIMER;
  ain2_config.intr_type = LEDC_INTR_DISABLE;
  ain2_config.duty = 0;
  ain2_config.channel = AIN2_PWM;
  ain2_config.gpio_num = AIN2_PIN;
  if (ledc_channel_config(&ain2_config) != ESP_OK)
    return ESP_FAIL;

  // Motor B PWM configuration
  ledc_channel_config_t bin1_config = {0};
  bin1_config.speed_mode = PWM_MODE;
  bin1_config.timer_sel = PWM_TIMER;
  bin1_config.intr_type = LEDC_INTR_DISABLE;
  bin1_config.duty = 0;
  bin1_config.channel = BIN1_PWM;
  bin1_config.gpio_num = BIN1_PIN;
  if (ledc_channel_config(&bin1_config) != ESP_OK)
    return ESP_FAIL;

  ledc_channel_config_t bin2_config = {0};
  bin2_config.speed_mode = PWM_MODE;
  bin2_config.timer_sel = PWM_TIMER;
  bin2_config.intr_type = LEDC_INTR_DISABLE;
  bin2_config.duty = 0;
  bin2_config.channel = BIN2_PWM;
  bin2_config.gpio_num = BIN2_PIN;
  if (ledc_channel_config(&bin2_config) != ESP_OK)
    return ESP_FAIL;

  // Timer configuration
  ledc_timer_config_t timer_config = {0};
  timer_config.speed_mode = PWM_MODE;
  timer_config.freq_hz = PWM_FREQUENCY;
  timer_config.duty_resolution = PWM_DUTY_RES;
  timer_config.timer_num = PWM_TIMER;
  if (ledc_timer_config(&timer_config) != ESP_OK)
    return ESP_FAIL;

  // Start all channels off
  ledc_set_duty(PWM_MODE, AIN1_PWM, 0);
  ledc_update_duty(PWM_MODE, AIN1_PWM);
  ledc_set_duty(PWM_MODE, AIN2_PWM, 0);
  ledc_update_duty(PWM_MODE, AIN2_PWM);
  ledc_set_duty(PWM_MODE, BIN1_PWM, 0);
  ledc_update_duty(PWM_MODE, BIN1_PWM);
  ledc_set_duty(PWM_MODE, BIN2_PWM, 0);
  ledc_update_duty(PWM_MODE, BIN2_PWM);

  return ESP_OK;
}

esp_err_t control_motor(bool is_motor_a, uint8_t duty_percentage,
                        bool is_clockwise) {
  uint32_t duty = (1024) * duty_percentage / 100;

  if (is_motor_a) {
    if (is_clockwise) {
      ledc_set_duty(PWM_MODE, AIN1_PWM, duty);
      ledc_update_duty(PWM_MODE, AIN1_PWM);
      ledc_set_duty(PWM_MODE, AIN2_PWM, 0);
      ledc_update_duty(PWM_MODE, AIN2_PWM);
    } else {
      ledc_set_duty(PWM_MODE, AIN1_PWM, 0);
      ledc_update_duty(PWM_MODE, AIN1_PWM);
      ledc_set_duty(PWM_MODE, AIN2_PWM, duty);
      ledc_update_duty(PWM_MODE, AIN2_PWM);
    }
  }
  if (!is_motor_a) {
    if (is_clockwise) {
      ledc_set_duty(PWM_MODE, BIN1_PWM, duty);
      ledc_update_duty(PWM_MODE, BIN1_PWM);
      ledc_set_duty(PWM_MODE, BIN2_PWM, 0);
      ledc_update_duty(PWM_MODE, BIN2_PWM);
    } else {
      ledc_set_duty(PWM_MODE, BIN1_PWM, 0);
      ledc_update_duty(PWM_MODE, BIN1_PWM);
      ledc_set_duty(PWM_MODE, BIN2_PWM, duty);
      ledc_update_duty(PWM_MODE, BIN2_PWM);
    }
  }

  return ESP_OK;
}

void test_motor_task() {
  while (1) {
    control_motor(true, 100, false);
    // control_motor(false, 30, false);
    vTaskDelay(pdMS_TO_TICKS(2000));
    control_motor(true, 80, true);
    // control_motor(false, 80, false);
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}
