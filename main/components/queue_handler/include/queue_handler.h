#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "portmacro.h"
#include <stdint.h>
#include <stdio.h>

static const uint8_t queue_len = 5;
extern QueueHandle_t queue_handler;
typedef struct {
  int x; // Oponent center position on x axis
  int y; // Oponent center position on y axis
  int w; // Oponent width
  int h; // Oponent height
} OponentData;

void queue_init();
void print_messages(void *pvParameters);
