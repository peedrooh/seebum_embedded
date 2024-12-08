#include "uart_receiver.h"
#include "esp_log.h"
#include "queue_handler.h"
#include <stdlib.h>
#include <string.h>

#define TERMINATION_CHAR '\n'
#define RX_BUF_SIZE 1024

static char message_buffer[RX_BUF_SIZE];
static int message_pos = 0;

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

  uint8_t *data = (uint8_t *)malloc(RX_BUF_SIZE);
  if (data == NULL) {
    ESP_LOGE(RX_TASK_TAG, "Failed to allocate memory");
    vTaskDelete(NULL); // Terminate task if memory allocation fails
  }

  while (1) {
    // Read from UART
    const int rxBytes =
        uart_read_bytes(UART_NUM, data, RX_BUF_SIZE, 1000 / portTICK_PERIOD_MS);

    if (rxBytes > 0) {
      for (int i = 0; i < rxBytes; i++) {
        if (data[i] == TERMINATION_CHAR) {
          // Null-terminate the message and process it
          message_buffer[message_pos] = '\0';
          // ESP_LOGI(RX_TASK_TAG, "Complete message: '%s'", message_buffer);

          // Parse the message
          OponentData oponent_data = {.x = -1, .y = -1, .w = -1, .h = -1};
          int matched =
              sscanf(message_buffer, "x_center: %d, y_center: %d, w: %d, h: %d",
                     &oponent_data.x, &oponent_data.y, &oponent_data.w,
                     &oponent_data.h);

          if (xQueueSend(uart_queue_handler, &oponent_data, portMAX_DELAY) !=
              pdTRUE) {
            ESP_LOGE(RX_TASK_TAG, "Failed to send to queue");
          }

          // Reset the buffer for the next message
          message_pos = 0;
        } else {
          // Add character to the message buffer
          if (message_pos < RX_BUF_SIZE - 1) {
            message_buffer[message_pos++] = data[i];
          } else {
            ESP_LOGE(RX_TASK_TAG, "Message buffer overflow");
            message_pos = 0; // Reset buffer in case of overflow
          }
        }
      }
    }

    vTaskDelay(pdMS_TO_TICKS(100)); // Small delay to avoid busy loop
  }

  free(data); // Free allocated memory
}
