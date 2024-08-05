#include "freertos/idf_additions.h"
#include "motor_driver.h"
#include "queue_handler.h"
#include "uart_receiver.h"

void app_main(void) {
  // uart_init();
  // queue_init();
  motor_driver_init();

  // xTaskCreatePinnedToCore(print_messages, "Print Messages", 1024 * 2,
  // NULL,
  // 1,
  //                         NULL, 0);
  // xTaskCreatePinnedToCore(rx_task, "RX Task", 1024 * 2, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(test_motor_task, "Test Motor Task", 1024 * 2, NULL, 1,
                          NULL, 1);
}

// #include "esp_log.h"
// #include "freertos/idf_additions.h"
// #include "freertos/projdefs.h"
// #include "ir_sensor.h"
// #include "queue_handler.h"
// #include "uart_receiver.h"
//
// void app_main(void) {
//   queues_init();
//   ir_init();
//   uart_init();
//
//   xTaskCreatePinnedToCore(test_uart_queue, "Test UART queue", 1024 * 2, NULL,
//   0,
//                           NULL, 0);
//   xTaskCreatePinnedToCore(rx_task, "UART receiver task", 1024 * 2, NULL, 0,
//                           NULL, 1);
//   xTaskCreatePinnedToCore(test_ir_queue, "Test IR queue", 1024 * 2, NULL, 0,
//                           NULL, 1);
//
//   while (1) {
//     // while the ir_command is not 101 it does nothing. When it is equal to
//     101
//     // it continius the program
//     if (ir_command != 101 && ir_address != 1) {
//       vTaskDelay(pdMS_TO_TICKS(50));
//       continue;
//     }
//     ir_command = 0;
//     ir_address = 0;
//     vTaskDelay(pdMS_TO_TICKS(2000));
//   }
// }
