#include "queue_handler.h"

QueueHandle_t queue_handler;

void queue_init() {
  queue_handler = xQueueCreate(queue_len, sizeof(OponentData));
  if (queue_handler == NULL) {
    ESP_LOGE("QUEUE_INIT", "Failed to create queue");
  }
}

void print_messages(void *pvParameters) {
  static const char *QUEUE_TASK_TAG = "QUEUE_TASK";
  OponentData received_data;

  while (1) {
    if (xQueueReceive(queue_handler, &received_data, portMAX_DELAY) == pdTRUE) {
      ESP_LOGI(
          QUEUE_TASK_TAG,
          "\n ----- Oponent Position and Size ----- \n  x:  %d\n  y:  %d\n  "
          "w:  %d\n  h:  %d\n",
          received_data.x, received_data.y, received_data.w, received_data.h);
      // free(received_data); // Free the memory after processing
    }
  }
}
