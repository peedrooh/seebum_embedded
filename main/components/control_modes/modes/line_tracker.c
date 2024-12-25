#include "line_tracker.h"
#include "control_modes.h"

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
