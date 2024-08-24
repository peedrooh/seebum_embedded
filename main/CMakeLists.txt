idf_component_register(SRCS "seebum_embedded.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver esp_timer motor_driver queue_handler uart_receiver ir_sensor rgb_led_driver line_sensor)

