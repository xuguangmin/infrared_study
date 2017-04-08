cmake_minimum_required(VERSION 2.8)
project(infrared_study)


# ------------ custom variables ------------
# custom compiler  arm-none-linux-gnueabi-gcc
set(CUSTOM_C_COMPILER           arm-none-linux-gnueabi-gcc)

# custom project root directory
set(CUSTOM_PROJECT_DIR          ${CMAKE_SOURCE_DIR})
# custom src directory
set(CUSTOM_SRC_DIR              ${CUSTOM_PROJECT_DIR}/src)

# custom bin output directory
set(CUSTOM_OUTPUT_DIR_BIN       ${CUSTOM_PROJECT_DIR}/bin)
# custom lib output directory
set(CUSTOM_OUTPUT_DIR_LIB       ${CUSTOM_PROJECT_DIR}/lib)

# custom include directory
set(CUSTOM_INCLUDE_DIR_MAIN     ${CUSTOM_PROJECT_DIR}/include)
# custom link directory
set(CUSTOM_LINK_DIR_MAIN        ${CUSTOM_OUTPUT_DIR_LIB})


# ------------ debug ------------
add_definitions(-Wall)
#set(CMAKE_C_FLAGS -static)
#add_definitions(-static)
#add_definitions(-g)
#add_definitions(-DDEBUG)


# ------------ set system variable ------------
# set bin output directory
set(EXECUTABLE_OUTPUT_PATH  ${CUSTOM_OUTPUT_DIR_BIN})
# set lib output directory
set(LIBRARY_OUTPUT_PATH     ${CUSTOM_OUTPUT_DIR_LIB})

# set compiler
set(CMAKE_C_COMPILER        ${CUSTOM_C_COMPILER})

# set include directory
include_directories(${CUSTOM_INCLUDE_DIR_MAIN})
# set link directory
link_directories(${CUSTOM_LINK_DIR_MAIN})


# ------------ target ------------
add_subdirectory(src)

