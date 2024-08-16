#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "ir_sensor.h"
#include "line_sensor.h"
#include "motor_driver.h"
#include "queue_handler.h"
#include "rgb_led_driver.h"
#include "uart_receiver.h"

void app_main(void) {
  // queues_init();
  // uart_init();
  // ir_init();
  // motor_driver_init();
  // rgb_led_init();
  line_sensor_init();

  // xTaskCreatePinnedToCore(test_uart_queue, "Print Messages", 1024 * 2, NULL,
  // 1,
  //                         NULL, 0);
  // xTaskCreatePinnedToCore(rx_task, "RX Task", 1024 * 2, NULL, 2, NULL, 0);
  // xTaskCreatePinnedToCore(test_ir_queue, "Test IR queue", 1024 * 2, NULL, 0,
  //                         NULL, 1);
  // xTaskCreatePinnedToCore(test_motor_task, "Test Motor Task", 1024 * 2, NULL,
  // 1,
  //                         NULL, 1);
  // xTaskCreatePinnedToCore(task_led_blink, "Blink RGB LED", 1024 * 2, NULL, 1,
  //                         NULL, 0);
  while (1) {
    bool is_line_white = get_line_color(true);
    ESP_LOGI("Line Sensor", "Linha: %d", is_line_white);
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}
