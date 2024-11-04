set(SRCS "control_modes.c")

list(APPEND SRCS "modes/remote_control.c")

set(INCLUDES "include" "modes")

idf_component_register(SRCS ${SRCS}
  INCLUDE_DIRS ${INCLUDES}
  REQUIRES esp_timer motor_driver ir_sensor rgb_led_driver line_sensor)

