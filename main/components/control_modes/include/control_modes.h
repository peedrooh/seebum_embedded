#ifndef CONTROL_MODES_H
#define CONTROL_MODES_H

#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "ir_sensor.h"
#include "line_sensor.h"
#include "motor_driver.h"
#include "queue_handler.h"
#include "remote_control.h"
#include "rgb_led_driver.h"

#define YELLOW_HEX 0xFFFF00
#define GREEN_HEX 0x00FF00
#define BLUE_HEX 0x0000FF
#define RED_HEX 0xFF0000

extern TaskHandle_t remote_task_handle;
extern TaskHandle_t main_task_handle;
extern TaskHandle_t line_tracker_handle;

void line_tracker_mode(void *pvParameters);
void main_loop(void *pvParameter);

#endif
