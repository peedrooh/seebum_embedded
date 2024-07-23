#ifndef QUEUE_HANDLER_H
#define QUEUE_HANDLER_H

#include "driver/rmt_rx.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "ir_sensor.h"
#include "portmacro.h"
#include <stdint.h>
#include <stdio.h>

// UART queue handler
static const uint8_t uart_queue_len = 5;
extern QueueHandle_t uart_queue_handler;
typedef struct {
  int x; // Oponent center position on x axis
  int y; // Oponent center position on y axis
  int w; // Oponent width
  int h; // Oponent height
} OponentData;

// IR RMT queue handler
extern QueueHandle_t ir_queue_handler;
static const uint8_t ir_queue_len = 1;

void queues_init();
void test_uart_queue(void *pvParameters);
void test_ir_queue(void *pvParameters);

#endif
