
BINARY = main

OBJS += clock.o \
	pwm.o \
	led.o \
	keypad_masquerade.o \
	rotary_dial.o \
	cm0_priority_patch.o

CPPFLAGS += --std=gnu11 -Iinclude

OPENCM3_DIR = ../libopencm3

LDSCRIPT = $(OPENCM3_DIR)/lib/stm32/l0/stm32l0xx8.ld

include ../libopencm3.target.mk
