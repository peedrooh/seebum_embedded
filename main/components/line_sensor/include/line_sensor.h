#ifndef LINE_SENSOR_H
#define LINE_SENOSOR_H

#include "driver/adc_types_legacy.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
#include "esp_log.h"
#include "hal/adc_types.h"

// Both GPIO's 4 and 5 belongs to ADC 1
// GPIO4 is equivalent to ADC 1 Channel 3
// (https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32s3/api-reference/peripherals/adc.html#_CPPv414ADC1_CHANNEL_3)
// GPIO5 is equivalent ot ADC 1 Channel 4
#define LS_RIGHT_PIN ADC1_CHANNEL_4
#define LS_LEFT_PIN ADC1_CHANNEL_3
#define WHITE_OFFSET 300

esp_err_t line_sensor_init();
bool get_line_color(bool is_right);
void test_line_sensor(void *pvParameters);

#endif
