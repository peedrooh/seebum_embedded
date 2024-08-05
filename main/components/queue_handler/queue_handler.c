#include "queue_handler.h"

QueueHandle_t uart_queue_handler;
QueueHandle_t ir_queue_handler;

void queues_init() {
  static const char *TAG = "QUEUE_INIT";
  uart_queue_handler = xQueueCreate(uart_queue_len, sizeof(OponentData));
  if (uart_queue_handler == NULL)
    ESP_LOGE(TAG, "Failed to create uart queue");

  ir_queue_handler =
      xQueueCreate(ir_queue_len, sizeof(rmt_rx_done_event_data_t));
  if (ir_queue_handler == NULL)
    ESP_LOGE(TAG, "Failed to create ir sensor queue");
}

void test_uart_queue(void *pvParameters) {
  static const char *QUEUE_TASK_TAG = "QUEUE_TASK";
  OponentData received_data;

  while (1) {
    if (xQueueReceive(uart_queue_handler, &received_data, portMAX_DELAY) ==
        pdTRUE) {
      ESP_LOGI(
          QUEUE_TASK_TAG,
          "\n ----- Oponent Position and Size ----- \n  x:  %d\n  y:  %d\n  "
          "w:  %d\n  h:  %d\n",
          received_data.x, received_data.y, received_data.w, received_data.h);
      // free(received_data); // Free the memory after processing
    }
  }
}

void test_ir_queue(void *pvParameters) {
  while (1) {
    if (xQueueReceive(ir_queue_handler, &rx_data, pdMS_TO_TICKS(1000)) ==
        pdPASS) {
      // parse the receive symbols and print the result
      if (sirc_frame_parser(rx_data.received_symbols, rx_data.num_symbols)) {
        ESP_LOGI("IR Data", "Address: %d   -   Command: %d", ir_address,
                 ir_command);
      }

      // start receive again
      ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols),
                                  &receive_config));
    }
  }
}
