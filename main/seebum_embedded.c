#include "driver/rmt_common.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "ir_sensor.h"
#include "line_sensor.h"
#include "motor_driver.h"
#include "queue_handler.h"
#include "rgb_led_driver.h"
#include "uart_receiver.h"

#define YELLOW_HEX 0xFFFF00
#define GREEN_HEX 0x00FF00
#define BLUE_HEX 0x0000FF

TaskHandle_t remote_task_handle = NULL;
TaskHandle_t main_task_handle = NULL;

void main_loop(void *pvParameter);

void robot_with_remote(void *pvParameters) {
  queues_init();
  uint32_t last_click_time = esp_timer_get_time() / 1000;
  ir_restart();

  while (1) {
    set_rgb_color(BLUE_HEX);

    if ((esp_timer_get_time() / 1000) - last_click_time > 20) {
      control_motor(true, 0, true);
      control_motor(false, 0, true);
    }
    if (xQueueReceive(ir_queue_handler, &rx_data, pdMS_TO_TICKS(10)) ==
        pdPASS) {

      // parse the receive symbols and print the result
      if (!sirc_frame_parser(rx_data.received_symbols, rx_data.num_symbols)) {
        // restart ir receiver and go to next loop iteration
        ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols,
                                    sizeof(raw_symbols), &receive_config));
        continue;
      }
    } else {
      continue;
    }

    if (ir_command == 35 && ir_address == 23 && main_task_handle == NULL) {
      control_motor(true, 0, true);
      control_motor(false, 0, true);
      rmt_disable(rx_channel);
      xTaskCreatePinnedToCore(main_loop, "Main Loop", 1024 * 4, NULL, 1,
                              &main_task_handle, 0);
      remote_task_handle = NULL;
      vTaskDelete(remote_task_handle);
    } else if (ir_command == 116 && ir_address == 1) {
      last_click_time = esp_timer_get_time() / 1000;
      control_motor(true, 100, false);
      control_motor(false, 100, false);
    } else if (ir_command == 51 && ir_address == 1) {
      last_click_time = esp_timer_get_time() / 1000;
      control_motor(false, 100, false);
      control_motor(true, 0, false);
    } else if (ir_command == 52 && ir_address == 1) {
      last_click_time = esp_timer_get_time() / 1000;
      control_motor(false, 0, false);
      control_motor(true, 100, false);
    } else if (ir_command == 117 && ir_address == 1) {
      last_click_time = esp_timer_get_time() / 1000;
      control_motor(true, 100, true);
      control_motor(false, 100, true);
    }

    if ((esp_timer_get_time() / 1000) - last_click_time > 200) {
      control_motor(true, 0, true);
      control_motor(false, 0, true);
    }

    ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols),
                                &receive_config));
  }
}

void main_loop(void *pvParameters) {

  queues_init();
  ir_restart();

  while (1) {
    set_rgb_color(YELLOW_HEX);

    if (xQueueReceive(ir_queue_handler, &rx_data, pdMS_TO_TICKS(1000)) ==
        pdPASS) {
      // parse the receive symbols and print the result
      if (!sirc_frame_parser(rx_data.received_symbols, rx_data.num_symbols)) {
        // restart ir receiver and go to next loop iteration
        ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols,
                                    sizeof(raw_symbols), &receive_config));
        continue;
      }
    } else {
      continue;
    }

    if (ir_command == 0 && ir_address == 1 && remote_task_handle == NULL) {
      rmt_disable(rx_channel);
      vTaskDelay(pdMS_TO_TICKS(200));
      xTaskCreatePinnedToCore(robot_with_remote, "Remote Control Mode",
                              1024 * 3, NULL, 1, &remote_task_handle, 0);
      main_task_handle = NULL;
      vTaskDelete(main_task_handle);
    }

    ESP_LOGI("Main Loop", "Address: %d   -   Command: %d", ir_address,
             ir_command);

    ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols),
                                &receive_config));
  }
}

void app_main(void) {
  // uart_init();
  motor_driver_init();
  control_motor(true, 0, true);
  control_motor(false, 0, true);

  queues_init();
  ir_init();
  rgb_led_init();
  // line_sensor_init();

  xTaskCreatePinnedToCore(main_loop, "Main Loop", 1024 * 3, NULL, 1,
                          &main_task_handle, 0);
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
  // while (1) {
  //   bool is_line_white = get_line_color(true);
  //   ESP_LOGI("Line Sensor", "Linha: %d", is_line_white);
  //   vTaskDelay(pdMS_TO_TICKS(2000));
  // }
}
