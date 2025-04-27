#include "control_modes.h"
#include "driver/rmt_common.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "ir_sensor.h"
#include "motor_driver.h"
#include "rgb_led_driver.h"
#include <stdbool.h>
#include <stdio.h>

TaskHandle_t remote_task_handle = NULL;
TaskHandle_t main_task_handle = NULL;
TaskHandle_t line_tracker_handle = NULL;
TaskHandle_t oponent_detection_handle = NULL;

void oponent_detection(void *pvParameters) {
  static const char *OPONENT_DETECTION_TASK = "OPONENT_DETECTION";

  queues_init();
  ir_restart();
  set_rgb_color(GREEN_HEX);
  OponentData received_data;

  while (1) {

    // If receive go back command from remote control goes back to main loop
    if (xQueueReceive(ir_queue_handler, &rx_data, pdMS_TO_TICKS(200)) ==
        pdPASS) {
      if (sirc_frame_parser(rx_data.received_symbols, rx_data.num_symbols)) {
        ESP_LOGI(OPONENT_DETECTION_TASK, "Received ir signal");
      }
      // start receive again
      ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols),
                                  &receive_config));
    }
    if (ir_command == 35 && ir_address == 23 && main_task_handle == NULL) {
      control_motor(true, 0, true);
      control_motor(false, 0, true);
      rmt_disable(rx_channel);
      xTaskCreatePinnedToCore(main_loop, "Main Loop", 1024 * 4, NULL, 1,
                              &main_task_handle, 0);
      oponent_detection_handle = NULL;
      vTaskDelete(oponent_detection_handle);
    }
    if (ir_command != 101 || ir_address != 1) {
      ir_command = 0;
      continue;
    }

    vTaskDelay(pdMS_TO_TICKS(4500));

    while (1) {
      // CHECK IR SIGNAL
      if (xQueueReceive(ir_queue_handler, &rx_data, pdMS_TO_TICKS(200)) ==
          pdPASS) {
        if (sirc_frame_parser(rx_data.received_symbols, rx_data.num_symbols)) {
          ESP_LOGI(OPONENT_DETECTION_TASK, "Received ir signal");
        }
        // start receive again
        ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols,
                                    sizeof(raw_symbols), &receive_config));
      }

      // RETURN TO MAIN LOOP
      if (ir_command == 35 && ir_address == 23 && main_task_handle == NULL) {
        control_motor(true, 0, true);
        control_motor(false, 0, true);
        rmt_disable(rx_channel);
        xTaskCreatePinnedToCore(main_loop, "Main Loop", 1024 * 4, NULL, 1,
                                &main_task_handle, 0);
        oponent_detection_handle = NULL;
        vTaskDelete(oponent_detection_handle);
      }
      ir_command = 0;

      // CONTROL LOOP
      if (xQueueReceive(uart_queue_handler, &received_data, portMAX_DELAY) ==
          pdTRUE) {
        // ESP_LOGI(
        //     "MAIN LOOP UART",
        //     "\n ----- Oponent Position and Size ----- \n  x:  %d\n  y:  %d\n
        //     " "w:  %d\n  h:  %d\n", received_data.x, received_data.y,
        //     received_data.w, received_data.h);

        if (received_data.x < 100) {
          control_motor(true, 80, false);
          control_motor(false, 30, false);
        } else if (received_data.x > 200) {
          control_motor(false, 80, false);
          control_motor(true, 30, false);
        } else {
          control_motor(true, 100, false);
          control_motor(false, 100, false);
        }
        // free(&received_data); // Free the memory after processing
        received_data.x = 0;
        received_data.y = 0;
        received_data.h = 0;
        received_data.w = 0;
      }
    }
  }
}

void main_loop(void *pvParameters) {

  queues_init();
  ir_restart();
  set_rgb_color(YELLOW_HEX);

  while (1) {

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

    if (ir_command == 1 && ir_address == 1 && line_tracker_handle == NULL) {
      rmt_disable(rx_channel);
      vTaskDelay(pdMS_TO_TICKS(200));
      xTaskCreatePinnedToCore(line_tracker_mode, "Line Tracker Mode", 1024 * 3,
                              NULL, 1, &line_tracker_handle, 0);

      main_task_handle = NULL;
      vTaskDelete(main_task_handle);
    }

    if (ir_command == 2 && ir_address == 1 &&
        oponent_detection_handle == NULL) {
      rmt_disable(rx_channel);
      vTaskDelay(pdMS_TO_TICKS(200));
      xTaskCreatePinnedToCore(oponent_detection, "Oponent Detection Mode",
                              1024 * 3, NULL, 1, &oponent_detection_handle, 0);
      main_task_handle = NULL;
      vTaskDelete(main_task_handle);
    }

    if (ir_command == 9 && ir_address == 1) {
      esp_restart();
    }

    ESP_LOGI("Main Loop", "Address: %d   -   Command: %d", ir_address,
             ir_command);

    ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols),
                                &receive_config));
  }
}
