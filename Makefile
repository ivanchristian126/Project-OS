CC = gcc
CPPFLAGS += -D_POSIX_C_SOURCE=200809L -Iinclude
CFLAGS ?= -O2 -g
CFLAGS += -std=c11 -Wall -Wextra -Wpedantic
LDLIBS += -lrt

SOURCES := $(wildcard src/*.c src/scheduler/*.c src/runtime/*.c src/kernel_bridge/*.c)
OBJECTS := $(patsubst src/%.c,build/%.o,$(SOURCES))
DEPS := $(OBJECTS:.o=.d)
TARGET := build/timesharing
SYSCALL_TEST := build/ts_mark_test

.PHONY: all run clean syscall-test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

build/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

run: $(TARGET)
	./$(TARGET)

syscall-test: $(SYSCALL_TEST)

$(SYSCALL_TEST): kernel/tests/ts_mark_test.c include/ts_syscall.h
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $<

clean:
	rm -rf build

-include $(DEPS)
