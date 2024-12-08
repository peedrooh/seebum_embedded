#include "control_modes.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "motor_driver.h"
#include "rgb_led_driver.h"
#include <stdbool.h>
#include <stdio.h>

TaskHandle_t remote_task_handle = NULL;
TaskHandle_t main_task_handle = NULL;
TaskHandle_t line_tracker_handle = NULL;

void line_tracker_mode(void *pvParameters) {
  static const char *LINE_TRACKER_TAG = "LINE TRACKER";

  queues_init();
  ir_restart();
  set_rgb_color(RED_HEX);

  while (1) {
    // If receive go back command from remote control goes back to main loop
    if (xQueueReceive(ir_queue_handler, &rx_data, pdMS_TO_TICKS(200)) ==
        pdPASS) {
      if (sirc_frame_parser(rx_data.received_symbols, rx_data.num_symbols)) {
        ESP_LOGI(LINE_TRACKER_TAG, "Received ir signal");
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

      line_tracker_handle = NULL;
      vTaskDelete(line_tracker_handle);
    }

    // if see white line do deviation maneuver
    if (get_line_color(true) || get_line_color(false)) {
      // ESP_LOGI(LINE_TRACKER_TAG, "Manobra de desvio");
      control_motor(true, 100, true);
      control_motor(false, 100, true);
      vTaskDelay(pdMS_TO_TICKS(300));
      control_motor(true, 40, false);
      control_motor(false, 100, true);
      vTaskDelay(pdMS_TO_TICKS(200));
    }

    control_motor(true, 70, false);
    control_motor(false, 70, false);
  }
}

void main_loop(void *pvParameters) {

  queues_init();
  ir_restart();
  set_rgb_color(YELLOW_HEX);

  while (1) {
    OponentData received_data;

    if (xQueueReceive(uart_queue_handler, &received_data, portMAX_DELAY) ==
        pdTRUE) {
      ESP_LOGI(
          "MAIN LOOP UART",
          "\n ----- Oponent Position and Size ----- \n  x:  %d\n  y:  %d\n  "
          "w:  %d\n  h:  %d\n",
          received_data.x, received_data.y, received_data.w, received_data.h);
      // free(received_data); // Free the memory after processing
    }

    // // Wait's until receiving a IR data from remote control
    // if (xQueueReceive(ir_queue_handler, &rx_data, pdMS_TO_TICKS(1000)) ==
    //     pdPASS) {
    //   // parse the receive symbols and print the result
    //   if (!sirc_frame_parser(rx_data.received_symbols, rx_data.num_symbols))
    //   {
    //     // restart ir receiver and go to next loop iteration
    //     ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols,
    //                                 sizeof(raw_symbols), &receive_config));
    //     continue;
    //   }
    // } else {
    //   continue;
    // }
    //
    // // If 1 is clicked enter in the remote control mode and stop main task
    // if (ir_command == 0 && ir_address == 1 && remote_task_handle == NULL) {
    //   rmt_disable(rx_channel);
    //   vTaskDelay(pdMS_TO_TICKS(200));
    //   xTaskCreatePinnedToCore(robot_with_remote, "Remote Control Mode",
    //                           1024 * 3, NULL, 1, &remote_task_handle, 0);
    //   main_task_handle = NULL;
    //   vTaskDelete(main_task_handle);
    // }
    //
    // if (ir_command == 1 && ir_address == 1 && line_tracker_handle == NULL) {
    //   rmt_disable(rx_channel);
    //   vTaskDelay(pdMS_TO_TICKS(200));
    //   xTaskCreatePinnedToCore(line_tracker_mode, "Line Tracker Mode", 1024 *
    //   3,
    //                           NULL, 1, &line_tracker_handle, 0);
    //
    //   main_task_handle = NULL;
    //   vTaskDelete(main_task_handle);
    // }
    //
    // ESP_LOGI("Main Loop", "Address: %d   -   Command: %d", ir_address,
    //          ir_command);
    //
    // ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols),
    //                             &receive_config));
  }
}
