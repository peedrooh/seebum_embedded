#include "ir_sensor.h"
#include <stdio.h>

rmt_channel_handle_t rx_channel;
rmt_receive_config_t receive_config;
rmt_symbol_word_t raw_symbols[64];
rmt_rx_done_event_data_t rx_data;
uint8_t IR_address;
uint8_t IR_command;

esp_err_t ir_init() {
  static const char *TAG = "RMT Receiver Config";

  ESP_LOGI(TAG, "Create RMT RX channel");
  rmt_rx_channel_config_t rx_channel_cfg = {
      .clk_src = RMT_CLK_SRC_DEFAULT,
      .resolution_hz = IR_RESOLUTION_HZ,
      .mem_block_symbols = 48,
      .gpio_num = IR_RX_GPIO_NUM,
  };
  rx_channel = NULL;
  ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channel));

  ESP_LOGI(TAG, "Register RX done callback");
  rmt_rx_event_callbacks_t cbs = {
      .on_recv_done = rmt_rx_done_callback,
  };
  ESP_ERROR_CHECK(
      rmt_rx_register_event_callbacks(rx_channel, &cbs, ir_queue_handler));

  ESP_LOGI(TAG, "Enable RMT RX channel");
  receive_config.signal_range_min_ns = 1250;
  receive_config.signal_range_max_ns = 12000000;
  ESP_ERROR_CHECK(rmt_enable(rx_channel));
  ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols),
                              &receive_config));

  return ESP_OK;
}

bool sirc_check_in_range(uint32_t signal_duration, uint32_t spec_duration) {
  return (signal_duration < (spec_duration + IR_SIRC_DECODE_TIME_MARGIN)) &&
         (signal_duration > (spec_duration - IR_SIRC_DECODE_TIME_MARGIN));
}

bool sirc_parse_logic0(rmt_symbol_word_t *rmt_sirc_symbols) {
  return sirc_check_in_range(rmt_sirc_symbols->duration0,
                             SIRC_LOGIC_ZERO_DURATION) &&
         sirc_check_in_range(rmt_sirc_symbols->duration1,
                             SIRC_INTERBIT_DURATION);
}

bool sirc_parse_logic1(rmt_symbol_word_t *rmt_sirc_symbols) {
  return sirc_check_in_range(rmt_sirc_symbols->duration0,
                             SIRC_LOGIC_ONE_DURATION) &&
         sirc_check_in_range(rmt_sirc_symbols->duration1,
                             SIRC_INTERBIT_DURATION);
}

bool sirc_frame_parser(rmt_symbol_word_t *rmt_sirc_symbols, size_t symbol_num) {
  // printf("SIRC frame start---\r\n");
  // for (size_t i = 0; i < symbol_num; i++) {
  //   printf("{%d:%d},{%d:%d}\r\n", rmt_sirc_symbols[i].level0,
  //          rmt_sirc_symbols[i].duration0, rmt_sirc_symbols[i].level1,
  //          rmt_sirc_symbols[i].duration1);
  // }
  // printf("---SIRC frame end\n\r");

  if (symbol_num != 13 && symbol_num != 16)
    return false;

  // Check start bits
  rmt_symbol_word_t *cur = rmt_sirc_symbols;
  uint8_t address = 0;
  uint8_t command = 0;
  bool valid_start_code =
      sirc_check_in_range(cur->duration0, SIRC_START_DURATION);
  if (!valid_start_code) {
    return false;
  }
  cur++;

  // Check Address bits
  for (int i = 0; i < 7; i++) {
    if (sirc_parse_logic1(cur)) {
      command |= 1 << i;
    } else if (sirc_parse_logic0(cur)) {
      command &= ~(1 << i);
    } else {
      return false;
    }
    cur++;
  }

  // Check command bits
  if (symbol_num == 13) {
    for (int i = 0; i < 4; i++) {
      if (sirc_parse_logic1(cur)) {
        address |= 1 << i;
      } else if (sirc_parse_logic0(cur)) {
        address &= ~(1 << i);
      } else {
        return false;
      }
      cur++;
    }
  } else if (symbol_num == 16) {
    for (int i = 0; i < 7; i++) {
      if (sirc_parse_logic1(cur)) {
        address |= 1 << i;
      } else if (sirc_parse_logic0(cur)) {
        address &= ~(1 << i);
      } else {
        return false;
      }
      cur++;
    }
  }

  IR_address = address;
  IR_command = command;
  return true;
}

bool rmt_rx_done_callback(rmt_channel_handle_t channel,
                          const rmt_rx_done_event_data_t *edata,
                          void *user_data) {
  BaseType_t high_task_wakeup = pdFALSE;
  QueueHandle_t receive_queue = (QueueHandle_t)user_data;
  // send the received RMT symbols to the parser task
  xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);
  return high_task_wakeup == pdTRUE;
}
