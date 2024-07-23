#include "uart_receiver.h"
#include "queue_handler.h"

void uart_init() {
  ESP_ERROR_CHECK(
      uart_driver_install(UART_NUM, RX_BUF_SIZE * 2, 0, 0, NULL, 0));
  ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
  ESP_ERROR_CHECK(uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE,
                               UART_PIN_NO_CHANGE));
}

void rx_task(void *arg) {
  static const char *RX_TASK_TAG = "RX_TASK";
  esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);

  while (1) {
    // Alocating memory to store UART message
    uint8_t *data = (uint8_t *)malloc(RX_BUF_SIZE + 1);
    if (data == NULL) {
      ESP_LOGE(RX_TASK_TAG, "Failed to allocate memory");
      vTaskDelete(NULL); // Terminate task if memory allocation fails
    }

    // Read from UART
    const int rxBytes =
        uart_read_bytes(UART_NUM, data, RX_BUF_SIZE, 1000 / portTICK_PERIOD_MS);

    // Sends the data to the queue if it is bigger than 0 and smaller than 16
    if (rxBytes > 0 && rxBytes <= 16) {
      data[rxBytes] = '\0'; // Ensure null-termination for string operations
      ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, (char *)data);

      OponentData oponent_data = {.x = -1, .y = -1, .w = -1, .h = -1};
      sscanf((char *)data, "%d %d %d %d", &oponent_data.x, &oponent_data.y,
             &oponent_data.w, &oponent_data.h);

      if (xQueueSend(uart_queue_handler, &oponent_data, portMAX_DELAY) !=
          pdTRUE) {
        ESP_LOGE(RX_TASK_TAG, "Failed to send to queue");
        free(data);
      }
    } else if (rxBytes > 16) {
      ESP_LOGE(RX_TASK_TAG,
               "UART message must be 16 bytes or smaller. Actual size %d",
               rxBytes);
      free(data);
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
