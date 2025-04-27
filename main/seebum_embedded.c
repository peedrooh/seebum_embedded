#include "control_modes.h"
#include "ir_sensor.h"
#include "line_sensor.h"
#include "motor_driver.h"
#include "queue_handler.h"
#include "rgb_led_driver.h"
#include "uart_receiver.h"
#include "xtensa/hal.h"

void app_main(void) {
  uart_init();
  motor_driver_init();
  control_motor(true, 0, true);
  control_motor(false, 0, true);

  queues_init();
  ir_init();
  rgb_led_init();
  line_sensor_init();

  xTaskCreatePinnedToCore(main_loop, "Main Loop", 1024 * 3, NULL, 1,
                          &main_task_handle, 0);
  xTaskCreatePinnedToCore(rx_task, "RX Task", 1024 * 2, NULL, 2, NULL, 0);
  // xTaskCreatePinnedToCore(test_uart_queue, "Print Messages", 1024 * 2, NULL,
  // 1,
  //                         NULL, 0);
  // xTaskCreatePinnedToCore(test_ir_queue, "Test IR queue", 1024 * 2, NULL, 0,
  //                         NULL, 1);
  // xTaskCreatePinnedToCore(test_motor_task, "Test Motor Task", 1024 * 2, NULL,
  // 1,
  //                         NULL, 1);
  // xTaskCreatePinnedToCore(task_led_blink, "Blink RGB LED", 1024 * 2, NULL, 1,
  //                         NULL, 0);
  // xTaskCreatePinnedToCore(test_line_sensor, "Test line Sensor", 1024 * 2,
  // NULL,
  //                         1, NULL, 0);
}
