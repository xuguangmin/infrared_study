configure_file(config.h             ${CUSTOM_INCLUDE_DIR_MAIN}/config.h)

set(EXE_FILENAME     infrared_study)

add_executable(${EXE_FILENAME}  main.c ir_study.c)
target_link_libraries(${EXE_FILENAME}  libutils.a; libdevice.a; libprotocol.a; -ldl; -lpthread)