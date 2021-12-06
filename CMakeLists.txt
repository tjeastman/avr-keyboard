cmake_minimum_required(VERSION 3.18)

find_program(AVR_CC avr-gcc REQUIRED)
find_program(AVR_OBJCOPY avr-objcopy REQUIRED)
find_program(AVR_FLASH avrdude REQUIRED)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)
set(CMAKE_C_COMPILER ${AVR_CC})

project(keyboard LANGUAGES C)

include_directories(${PROJECT_SOURCE_DIR})

add_compile_definitions(F_CPU=16000000L BAUD=9600)
add_compile_options(-mmcu=atmega328p -Os)
add_link_options(-mmcu=atmega328p)

add_executable(
  main.elf
  main.c
  usart.c
  keyboard/base.c
  keyboard/protocol.c
  keyboard/state.c
  keyboard/scan.c
  keyboard/label.c
)

add_custom_command(
  TARGET main.elf
  POST_BUILD
  BYPRODUCTS main.hex
  COMMAND ${AVR_OBJCOPY} -j .text -j .data -O ihex main.elf main.hex
)

add_custom_target(
  flash
  DEPENDS main.hex
  COMMAND ${AVR_FLASH} -p atmega328p -c usbtiny -P usb -U flash:w:main.hex
)
