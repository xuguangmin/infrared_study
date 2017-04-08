configure_file(log.h             ${CUSTOM_INCLUDE_DIR_MAIN}/log.h)
configure_file(task_queue.h      ${CUSTOM_INCLUDE_DIR_MAIN}/task_queue.h)

aux_source_directory(. UTILS_SRC_LIST)
add_library(utils STATIC  ${UTILS_SRC_LIST})
