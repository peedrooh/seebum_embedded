idf_component_register(SRCS "seebum_embedded.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver motor_driver queue_handler uart_receiver ir_sensor)

