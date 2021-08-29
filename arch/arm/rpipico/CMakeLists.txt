cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from submodule
# note: this must happen before project()
include(pico-sdk/pico_sdk_init.cmake)

project(my_project)

pico_sdk_init()

add_library(bsp STATIC
	    bsp.c)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(bsp pico_stdlib)

add_compile_definitions(PICO_TIME_DEFAULT_ALARM_POOL_DISABLED=1)

#pico_enable_stdio_usb(bsp 1)
pico_enable_stdio_uart(bsp 1)
