configure_file(protocol_define.h       ${CUSTOM_INCLUDE_DIR_MAIN}/protocol_define.h)
configure_file(protocol.h             ${CUSTOM_INCLUDE_DIR_MAIN}/protocol.h)

aux_source_directory(. PROTOCOL_SRC_LIST)
add_library(protocol STATIC  ${PROTOCOL_SRC_LIST})
