cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME "7405Xv5")

project(${PROJECT_NAME})

if (CMAKE_HOST_SYSTEM_NAME MATCHES Windows)
    set(V5_TOOLCHAIN_PATH "C:/Program Files/PROS/toolchain/usr/bin")
endif (CMAKE_HOST_SYSTEM_NAME MATCHES Windows)

if (CMAKE_HOST_SYSTEM_NAME MATCHES Darwin)
    set(V5_TOOLCHAIN_PATH "/usr/local/Cellar/arm-gcc-bin@10/10-2020-q4-major/bin")
endif (CMAKE_HOST_SYSTEM_NAME MATCHES Darwin)

if (CMAKE_HOST_SYSTEM_NAME MATCHES Linux)
    set(V5_TOOLCHAIN_PATH "/opt/gcc-arm-none-eabi-10-2020-q4-major/bin")
endif (CMAKE_HOST_SYSTEM_NAME MATCHES Linux)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER "${V5_TOOLCHAIN_PATH}/arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "${V5_TOOLCHAIN_PATH}/arm-none-eabi-g++")

set(GNU_FLAGS "-mcpu=cortex-a9 -mfpu=neon-fp16 -mfloat-abi=softfp -O3 -g -Wno-psabi -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${GNU_FLAGS} -D_POSIX_THREADS -D_UNIX98_THREAD_MUTEX_ATTRIBUTES --std=gnu++17")
set(CMAKE_C_FLAGS "${GNU_FLAGS} -ffunction-sections -fdata-sections -fdiagnostics-color -funwind-tables --std=gnu11")


INCLUDE_DIRECTORIES("include" "${COMMON_LIB_PATH}/include" "${PROJECT_SOURCE_DIR}/../pros/include" "${PROJECT_SOURCE_DIR}/../pros/firmware/libv5rts/sdk/vexv5/include")
file(GLOB_RECURSE SOURCES "src/*.c" "src/*.cpp")
file(GLOB_RECURSE INCLUDES "include/*.h" "include/*.hpp")

add_library(${PROJECT_NAME} STATIC ${INCLUDES} ${SOURCES})

set_target_properties( ${PROJECT_NAME}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/firmware"
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/firmware"
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/firmware"
        )
