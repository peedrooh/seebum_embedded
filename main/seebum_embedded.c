// #include "freertos/idf_additions.h"
// #include "motor_driver.h"
// #include "queue_handler.h"
// #include "uart_receiver.h"
//
// void app_main(void) {
//   uart_init();
//   queue_init();
//   motor_driver_init();
//
//   // xTaskCreatePinnedToCore(print_messages, "Print Messages", 1024 * 2,
//   NULL,
//   // 1,
//   //                         NULL, 0);
//   xTaskCreatePinnedToCore(rx_task, "RX Task", 1024 * 2, NULL, 2, NULL, 0);
//   xTaskCreatePinnedToCore(test_motor_task, "Test Motor Task", 1024 * 2, NULL,
//   1,
//                           NULL, 1);
// }

#include "driver/rmt_rx.h"
#include "driver/rmt_tx.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#define EXAMPLE_IR_RESOLUTION_HZ 1000000 // 1MHz resolution, 1 tick = 1us
#define EXAMPLE_IR_TX_GPIO_NUM 18
#define EXAMPLE_IR_RX_GPIO_NUM 37
#define EXAMPLE_IR_SIRC_DECODE_MARGIN                                          \
  200 // Tolerance for parsing RMT symbols into bit stream

/**
 * @brief SIRC timing spec
 */
#define SIRC_START_DURATION 2400
#define SIRC_LOGIC_ONE_DURATION 1200
#define SIRC_LOGIC_ZERO_DURATION 600
#define SIRC_INTERBIT_DURATION 600

static const char *TAG = "example";

/**
 * @brief Saving SIRC decode results
 */
static uint8_t s_sirc_code_address;
static uint8_t s_sirc_code_command;

/**
 * @brief Check whether a duration is within expected range
 */
static inline bool sirc_check_in_range(uint32_t signal_duration,
                                       uint32_t spec_duration) {
  return (signal_duration < (spec_duration + EXAMPLE_IR_SIRC_DECODE_MARGIN)) &&
         (signal_duration > (spec_duration - EXAMPLE_IR_SIRC_DECODE_MARGIN));
}

/**
 * @brief Check whether a RMT symbol represents SIRC logic zero
 */
static bool sirc_parse_logic0(rmt_symbol_word_t *rmt_sirc_symbols) {
  return sirc_check_in_range(rmt_sirc_symbols->duration0,
                             SIRC_LOGIC_ZERO_DURATION) &&
         sirc_check_in_range(rmt_sirc_symbols->duration1,
                             SIRC_INTERBIT_DURATION);
}

/**
 * @brief Check whether a RMT symbol represents SIRC logic one
 */
static bool sirc_parse_logic1(rmt_symbol_word_t *rmt_sirc_symbols) {
  return sirc_check_in_range(rmt_sirc_symbols->duration0,
                             SIRC_LOGIC_ONE_DURATION) &&
         sirc_check_in_range(rmt_sirc_symbols->duration1,
                             SIRC_INTERBIT_DURATION);
}

/**
 * @brief Decode RMT symbols into SIRC address and command
 */
static bool sirc_parse_frame(rmt_symbol_word_t *rmt_sirc_symbols,
                             size_t symbol_num) {
  rmt_symbol_word_t *cur = rmt_sirc_symbols;
  uint8_t address = 0;
  uint8_t command = 0;
  bool valid_start_code =
      sirc_check_in_range(cur->duration0, SIRC_START_DURATION);
  if (!valid_start_code) {
    return false;
  }
  cur++;

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

  // save address and command
  s_sirc_code_address = address;
  s_sirc_code_command = command;
  return true;
}

/**
 * @brief Decode RMT symbols into SIRC scan code and print the result
 */
static void example_parse_sirc_frame(rmt_symbol_word_t *rmt_sirc_symbols,
                                     size_t symbol_num) {
  // printf("SIRC frame start---\r\n");
  // for (size_t i = 0; i < symbol_num; i++) {
  //   printf("{%d:%d},{%d:%d}\r\n", rmt_sirc_symbols[i].level0,
  //          rmt_sirc_symbols[i].duration0, rmt_sirc_symbols[i].level1,
  //          rmt_sirc_symbols[i].duration1);
  // }
  // printf("---SIRC frame end: ");

  if (symbol_num == 13 || symbol_num == 16) { // SIRC 12-bit frame
    if (sirc_parse_frame(rmt_sirc_symbols, symbol_num)) {
      printf("Address=%02X, Command=%02X\r\n\r\n", s_sirc_code_address,
             s_sirc_code_command);
    }
  } else {
    printf("Unknown SIRC frame\r\n\r\n");
  }
}

static bool example_rmt_rx_done_callback(rmt_channel_handle_t channel,
                                         const rmt_rx_done_event_data_t *edata,
                                         void *user_data) {
  BaseType_t high_task_wakeup = pdFALSE;
  QueueHandle_t receive_queue = (QueueHandle_t)user_data;
  // send the received RMT symbols to the parser task
  xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);
  return high_task_wakeup == pdTRUE;
}

void app_main(void) {
  ESP_LOGI(TAG, "create RMT RX channel");

  rmt_rx_channel_config_t rx_channel_cfg = {
      .clk_src = RMT_CLK_SRC_DEFAULT,
      .resolution_hz = EXAMPLE_IR_RESOLUTION_HZ,
      .mem_block_symbols =
          48, // amount of RMT symbols that the channel can store at a time
      .gpio_num = EXAMPLE_IR_RX_GPIO_NUM,
  };

  rmt_channel_handle_t rx_channel = NULL;
  ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channel));

  ESP_LOGI(TAG, "register RX done callback");
  QueueHandle_t receive_queue =
      xQueueCreate(1, sizeof(rmt_rx_done_event_data_t));
  rmt_rx_event_callbacks_t cbs = {
      .on_recv_done = example_rmt_rx_done_callback,
  };
  ESP_ERROR_CHECK(
      rmt_rx_register_event_callbacks(rx_channel, &cbs, receive_queue));

  rmt_receive_config_t receive_config = {
      .signal_range_min_ns =
          1250, // the shortest duration for SIRC signal is 600us, 1250ns <
                // 600us, valid signal won't be treated as noise
      .signal_range_max_ns =
          12000000, // the longest duration for SIRC signal is 2400us,
                    // 12000000ns > 2400us, the receive won't stop early
  };

  ESP_LOGI(TAG, "enable RMT TX and RX channels");
  ESP_ERROR_CHECK(rmt_enable(rx_channel));

  // save the received RMT symbols
  rmt_symbol_word_t raw_symbols[64]; // 64 symbols should be sufficient for a
                                     // standard SIRC frame
  rmt_rx_done_event_data_t rx_data;
  // ready to receive
  ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols),
                              &receive_config));
  while (1) {
    // wait for RX done signal
    if (xQueueReceive(receive_queue, &rx_data, pdMS_TO_TICKS(1000)) == pdPASS) {
      // parse the receive symbols and print the result
      example_parse_sirc_frame(rx_data.received_symbols, rx_data.num_symbols);
      // start receive again
      ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols),
                                  &receive_config));
    } else {
      vTaskDelay(pdMS_TO_TICKS(500));
    }
  }
}
