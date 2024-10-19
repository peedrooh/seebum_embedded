idf_component_register(SRCS "control_modes.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_timer motor_driver ir_sensor rgb_led_driver)

