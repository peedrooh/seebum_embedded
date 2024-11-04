#include "line_sensor.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"

adc_cali_handle_t adc1_cali_handle = NULL;
adc_oneshot_unit_handle_t adc1_handle;

esp_err_t line_sensor_init() {

  const static char *TAG = "Line Sensor";
  // ADC 1 initialization
  adc_oneshot_unit_init_cfg_t adc1_init_config = {.unit_id = ADC_UNIT_1};

  if (adc_oneshot_new_unit(&adc1_init_config, &adc1_handle) != ESP_OK)
    return ESP_FAIL;

  // ADC 1 and channels configuration
  adc_oneshot_chan_cfg_t adc1_config = {.bitwidth = ADC_BITWIDTH_DEFAULT,
                                        .atten = ADC_ATTEN_DB_12};
  if (adc_oneshot_config_channel(adc1_handle, LS_RIGHT_PIN, &adc1_config) !=
      ESP_OK)
    return ESP_FAIL;
  if (adc_oneshot_config_channel(adc1_handle, LS_LEFT_PIN, &adc1_config) !=
      ESP_OK)
    return ESP_FAIL;

  // ADC 1 callibration

  ESP_LOGI(TAG, "calibration scheme version is Cruve Fitting");
  adc_cali_curve_fitting_config_t cali_config = {
      .unit_id = ADC_UNIT_1,
      .atten = ADC_ATTEN_DB_12,
      .bitwidth = ADC_BITWIDTH_DEFAULT,
  };
  if (adc_cali_create_scheme_curve_fitting(&cali_config, &adc1_cali_handle) !=
      ESP_OK)
    return ESP_FAIL;

  return ESP_OK;
}

bool get_line_color(bool is_right_ls) {
  int raw_reading;

  if (is_right_ls) {
    adc_oneshot_read(adc1_handle, LS_RIGHT_PIN, &raw_reading);
    // ESP_LOGI("Line Sensor", "Raw reading right: %d", raw_reading);
  } else {
    adc_oneshot_read(adc1_handle, LS_LEFT_PIN, &raw_reading);
    // ESP_LOGI("Line Sensor", "Raw reading left: %d", raw_reading);
  }

  if (raw_reading < WHITE_OFFSET)
    return true;
  return false;
}

void test_line_sensor(void *pvParameters) {
  while (1) {
    bool sensor_one = get_line_color(true);
    bool sensor_two = get_line_color(false);
    ESP_LOGI("Line Sensor", "Sensor Right: %d\nSensor left: %d", sensor_one,
             sensor_two);
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}
