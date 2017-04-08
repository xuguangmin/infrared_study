configure_file(led.h             ${CUSTOM_INCLUDE_DIR_MAIN}/led.h)
configure_file(serial.h          ${CUSTOM_INCLUDE_DIR_MAIN}/serial.h)
configure_file(infrared.h        ${CUSTOM_INCLUDE_DIR_MAIN}/infrared.h)
configure_file(key_reset.h       ${CUSTOM_INCLUDE_DIR_MAIN}/key_reset.h)

aux_source_directory(. DEVICE_SRC_LIST)
add_library(device STATIC  ${DEVICE_SRC_LIST})
