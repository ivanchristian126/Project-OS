#ifndef TIMESHARING_H
#define TIMESHARING_H

#define TIMESHARING_VERSION "0.2.0"

/* Konfigurasi default aplikasi. */
#define TIMESHARING_DEFAULT_PROCESS_COUNT 3U
#define TIMESHARING_DEFAULT_QUANTUM_MS 100U
#define TIMESHARING_DEFAULT_WORKLOAD_MS 500U

/* Batas masukan yang diterima. */
#define TIMESHARING_MAX_PROCESSES 16U
#define TIMESHARING_MIN_PROCESSES 1U
#define TIMESHARING_MIN_QUANTUM_MS 10U
#define TIMESHARING_MAX_QUANTUM_MS 2000U
#define TIMESHARING_MIN_WORKLOAD_MS 50U
#define TIMESHARING_MAX_WORKLOAD_MS 10000U

typedef struct {
    unsigned int process_count;
    unsigned int quantum_ms;
    unsigned int workload_ms;
} timesharing_config;

#endif
