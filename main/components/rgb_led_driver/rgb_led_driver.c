#include "rgb_led_driver.h"
#include "esp_err.h"

esp_err_t rgb_led_init(void) {
  ledc_timer_config_t rgb_timer = {.duty_resolution = RGB_PWM_RES,
                                   .freq_hz = RGB_FREQUENCY,
                                   .speed_mode = RGB_MODE,
                                   .timer_num = RGB_TIMER};
  if (ledc_timer_config(&rgb_timer) != ESP_OK)
    return ESP_FAIL;

  ledc_channel_config_t rgb_channels = {.speed_mode = RGB_MODE,
                                        .channel = R_CHANNEL,
                                        .timer_sel = RGB_TIMER,
                                        .intr_type = LEDC_INTR_DISABLE,
                                        .gpio_num = R_PIN,
                                        .duty = 0,
                                        .hpoint = 0};
  if (ledc_channel_config(&rgb_channels) != ESP_OK)
    return ESP_FAIL;

  rgb_channels.channel = G_CHANNEL;
  rgb_channels.gpio_num = G_PIN;
  if (ledc_channel_config(&rgb_channels) != ESP_OK)
    return ESP_FAIL;

  rgb_channels.channel = B_CHANNEL;
  rgb_channels.gpio_num = B_PIN;
  if (ledc_channel_config(&rgb_channels) != ESP_OK)
    return ESP_FAIL;

  return ESP_OK;
}

esp_err_t set_rgb_color(unsigned long hexcode) {

  if (ledc_set_duty(RGB_MODE, R_CHANNEL, (hexcode >> 16) & 0xFF) != ESP_OK)
    return ESP_FAIL;
  if (ledc_update_duty(RGB_MODE, R_CHANNEL) != ESP_OK)
    return ESP_FAIL;

  if (ledc_set_duty(RGB_MODE, G_CHANNEL, (hexcode >> 8) & 0xFF) != ESP_OK)
    return ESP_FAIL;
  if (ledc_update_duty(RGB_MODE, G_CHANNEL) != ESP_OK)
    return ESP_FAIL;

  if (ledc_set_duty(RGB_MODE, B_CHANNEL, hexcode & 0xFF) != ESP_OK)
    return ESP_FAIL;
  if (ledc_update_duty(RGB_MODE, B_CHANNEL) != ESP_OK)
    return ESP_FAIL;

  return ESP_OK;
}

void task_led_blink(void *pvParameters) {
  srand(time(NULL));
  while (1) {
    uint8_t r = rand() % 256;
    uint8_t g = rand() % 256;
    uint8_t b = rand() % 256;

    unsigned long hexcode = (r << 16) | (g << 8) | b;
    ESP_LOGI("RGB_TEST_TASK", "Hexcode: #%x", (int)hexcode);

    set_rgb_color(hexcode);

    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}
