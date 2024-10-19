#ifndef CONTROL_MODES_H
#define CONTROL_MODES_H

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "esp_timer.h"
#include "ir_sensor.h"
#include "motor_driver.h"
#include "rgb_led_driver.h"


#define YELLOW_HEX 0xFFFF00
#define GREEN_HEX 0x00FF00
#define BLUE_HEX 0x0000FF


extern TaskHandle_t remote_task_handle;
extern TaskHandle_t main_task_handle;

void main_loop(void *pvParameter);
void robot_with_remote(void *pvParameters);

#endif
