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
UNIT_TEST := build/test_queue
WORKER_TEST := build/test_worker
TIMER_FAILURE_TEST := build/test_timer_failure
UNIT_TEST_SOURCES := tests/test_queue.c src/scheduler/process_queue.c \
	src/scheduler/round_robin.c

.PHONY: all run clean test check syscall-test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

build/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

run: $(TARGET)
	./$(TARGET)

$(UNIT_TEST): $(UNIT_TEST_SOURCES) include/process_queue.h include/scheduler.h
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(UNIT_TEST_SOURCES)

$(WORKER_TEST): tests/test_worker.c src/runtime/worker.c include/worker.h
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ tests/test_worker.c src/runtime/worker.c $(LDLIBS)

$(TIMER_FAILURE_TEST): $(OBJECTS) tests/fail_timer.c include/runtime_signals.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) tests/fail_timer.c \
		-Wl,--wrap=runtime_timer_start -Wl,--wrap=runtime_signals_restore $(LDLIBS)

test: $(UNIT_TEST) $(WORKER_TEST)
	timeout --kill-after=2s 20s ./$(UNIT_TEST)
	timeout --kill-after=2s 20s ./$(WORKER_TEST)

check: all
	bash tests/run_tests.sh

$(SYSCALL_TEST): kernel/tests/ts_mark_test.c include/ts_syscall.h
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $<

syscall-test: $(SYSCALL_TEST)

clean:
	rm -rf build

-include $(DEPS)
