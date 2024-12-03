#ifndef UART_RECEIVER_H
#define UART_RECEIVER_H

#include "driver/uart.h"
#include "esp_log.h"
#include <stdint.h>
#include <stdio.h>

/**
 * UART PARAMETERS DEFINITION
 * **/

#define BAUD_RATE 115200
#define UART_NUM UART_NUM_2
#define TXD_PIN 18
#define RXD_PIN 17
#define RX_BUF_SIZE 128

static const uart_config_t uart_config = {
    .baud_rate = BAUD_RATE,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
};

void uart_init();
void rx_task(void *arg);

#endif
