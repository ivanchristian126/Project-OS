#ifndef TIMESHARING_H
#define TIMESHARING_H

/* Default application configuration. */
#define TIMESHARING_VERSION "0.1.0"
#define TIMESHARING_DEFAULT_PROCESS_COUNT 3U
#define TIMESHARING_DEFAULT_QUANTUM_MS 100U
#define TIMESHARING_MAX_PROCESSES 16U
#define TIMESHARING_DEFAULT_WORKLOAD_MS 500U

typedef struct {
    unsigned int process_count;
    unsigned int quantum_ms;
} timesharing_config;

#endif
