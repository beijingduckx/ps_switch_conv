cmake_minimum_required(VERSION 3.13)


# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(ps_switch)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

add_executable(${PROJECT_NAME}
    main.c
    usb_descriptors.c
    psx_controller.c
    sw_controller.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(${PROJECT_NAME} pico_stdlib tinyusb_device tinyusb_board hardware_spi)
include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${PROJECT_NAME})
