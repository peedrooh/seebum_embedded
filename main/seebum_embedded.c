#include "freertos/idf_additions.h"
#include "motor_driver.h"
#include "queue_handler.h"
#include "uart_receiver.h"

void app_main(void) {
  // uart_init();
  // queue_init();
  motor_driver_init();
  //
  // xTaskCreatePinnedToCore(print_messages, "Print Messages", 1024 * 2, NULL,
  // 1,
  //                         NULL, 0);
  // xTaskCreatePinnedToCore(rx_task, "RX Task", 1024 * 2, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(test_motor_task, "Test Motor Task", 1024 * 2, NULL, 1,
                          NULL, 1);
}
