#include "motor_driver.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "soc/clk_tree_defs.h"

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
  ledc_timer_config_t pwm_timer_config = {.speed_mode = PWM_MODE,
                                          .timer_num = PWM_TIMER,
                                          .duty_resolution = PWM_DUTY_RES,
                                          .freq_hz = PWM_FREQUENCY,
                                          .clk_cfg = LEDC_AUTO_CLK};
  ESP_ERROR_CHECK(ledc_timer_config(&pwm_timer_config));

  ledc_channel_config_t pwma1_channel_config = {.speed_mode = PWM_MODE,
                                                .channel = PWMA1,
                                                .timer_sel = PWM_TIMER,
                                                .intr_type = LEDC_INTR_DISABLE,
                                                .gpio_num = PWMA1_PIN,
                                                .duty = 1023,
                                                .hpoint = 0};
  ESP_ERROR_CHECK(ledc_channel_config(&pwma1_channel_config));
  ledc_channel_config_t pwma2_channel_config = {.speed_mode = PWM_MODE,
                                                .channel = PWMA2,
                                                .timer_sel = PWM_TIMER,
                                                .intr_type = LEDC_INTR_DISABLE,
                                                .gpio_num = PWMA2_PIN,
                                                .duty = 1023,
                                                .hpoint = 0};
  ESP_ERROR_CHECK(ledc_channel_config(&pwma2_channel_config));

  ESP_ERROR_CHECK(ledc_set_duty(PWM_MODE, PWMA1, 1023));
  ESP_ERROR_CHECK(ledc_set_duty(PWM_MODE, PWMA2, 1023));
  ledc_update_duty(PWM_MODE, PWMA1);
  ledc_update_duty(PWM_MODE, PWMA2);
}

void test_motor_task() {
  while (1) {

    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}
