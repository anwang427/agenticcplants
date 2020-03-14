# Arch Linux path fixes
ARDUINO_DIR = /usr/share/arduino 
BOARDS_TXT = /usr/share/arduino/hardware/archlinux-arduino/avr/boards.txt 
BOOTLOADER_PARENT = /usr/share/arduino/hardware/archlinux-arduino/avr/bootloaders
ARDUINO_DIR = /usr/share/arduino
ARDUINO_PLATFORM_LIB_PATH = /usr/share/arduino/hardware/archlinux-arduino/avr/cores
ARDUINO_VAR_PATH = /usr/share/arduino/hardware/archlinux-arduino/avr/variants

USER_LIB_DIR = include
BOARD_TAG     = nano
BOARD_SUB     = atmega328
MONITOR_PORT  = /dev/ttyUSB0
AVRDUDE_ARD_BAUDRATE = 57600

OBJDIR = build
TARGET = cepheus
 
CXXFLAGS_STD = -std=c++17 -fno-threadsafe-statics -flto -O3 -Iinclude/ -fno-rtti
#-fno-rtti
#-o $(CLION_EXE_DIR)/arduino_test

LOCAL_CPP_SRCS = $(wildcard src/*.cpp)  
LOCAL_C_SRCS = $(wildcard src/*.c)  
LOCAL_CC_SRCS = $(wildcard src/*.cc)  
#headers
LOCAL_SRCS += $(wildcard include/*.h)  
LOCAL_SRCS += $(wildcard include/*.hpp)
#atmega328p specific header(s)

#libraries (including arduino core)
ARDUINO_LIBS = arduino

include /usr/share/arduino/Arduino.mk
#include /usr/share/arduino/hardware/archlinux-arduino/avr/cores/arduino/Arduino.h

