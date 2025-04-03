cmake_minimum_required(VERSION 3.21)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include(pico_sdk_import.cmake)


project(MainProject)


pico_sdk_init()


add_executable(main_app main.cpp)
target_link_libraries(main_app VL53L0X_Lib pico_stdlib)
add_subdirectory(lib)

pico_enable_stdio_usb(main_app 1)
pico_enable_stdio_uart(main_app 0)

pico_add_extra_outputs(main_app)



