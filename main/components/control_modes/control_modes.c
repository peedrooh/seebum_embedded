#include <stdio.h>
#include "control_modes.h"

TaskHandle_t remote_task_handle = NULL;
TaskHandle_t main_task_handle = NULL;

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

    // If user press Return button star main loop and stop remote control mode
    // task
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

    // Wait's until receiving a IR data from remote control
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

    // If 1 is clicked enter in the remote control mode and stop main task
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

