#ifndef IR_SENSOR_H
#define IR_SENSOR_H

#include "driver/rmt_rx.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"
#include "queue_handler.h"

#define IR_RESOLUTION_HZ 1000000 // 1MHz resolution, 1 tick = 1us
#define IR_RX_GPIO_NUM 7

/**
 * @brief SIRC timing spec
 */
#define SIRC_START_DURATION 2400
#define SIRC_LOGIC_ONE_DURATION 1200
#define SIRC_LOGIC_ZERO_DURATION 600
#define SIRC_INTERBIT_DURATION 600
#define IR_SIRC_DECODE_TIME_MARGIN 200

extern rmt_channel_handle_t rx_channel;
extern rmt_receive_config_t receive_config;
extern rmt_symbol_word_t raw_symbols[64];
extern rmt_rx_done_event_data_t rx_data;
extern uint8_t ir_address;
extern uint8_t ir_command;

/**
 * @brief Remote Control Transiver (RMT) receiver configuration. This component
 * expecto to receive only the sirc rmt protocol
 */
esp_err_t ir_init();

void ir_restart();

/**
 * @brief Check whether a duration is within expected range
 */
bool sirc_check_in_range(uint32_t signal_duration, uint32_t spec_duration);

/**
 * @brief Check whether a RMT symbol represents SIRC logic zero
 */
bool sirc_parse_logic0(rmt_symbol_word_t *rmt_sirc_symbols);

/**
 * @brief Check whether a RMT symbol represents SIRC logic one
 */
bool sirc_parse_logic1(rmt_symbol_word_t *rmt_sirc_symbols);

/**
 * @brief Decode RMT symbols into SIRC scan code
 */
bool sirc_frame_parser(rmt_symbol_word_t *rmt_sirc_symbols,
                       size_t symbol_length);
/**
 * @brief Every time the receiver is done receiving a symbol, this function will
 * be called. It will then send the received data to the ir_queue_handler
 */
bool rmt_rx_done_callback(rmt_channel_handle_t channel,
                          const rmt_rx_done_event_data_t *edata,
                          void *user_data);

#endif
