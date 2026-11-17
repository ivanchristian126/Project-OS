#include "cli.h"
#include "timesharing.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
    timesharing_config config = {
        .process_count = TIMESHARING_DEFAULT_PROCESS_COUNT,
        .quantum_ms = TIMESHARING_DEFAULT_QUANTUM_MS,
        .workload_ms = TIMESHARING_DEFAULT_WORKLOAD_MS
    };

    printf("Time Sharing System v%s\n", TIMESHARING_VERSION);

    switch (cli_read_config(argc, argv, &config)) {
    case CLI_HELP:
        cli_print_usage(argv[0]);
        return 0;
    case CLI_VERSION:
        return 0;
    case CLI_ERROR:
        return 2;
    case CLI_OK:
        break;
    }

    printf("Konfigurasi diterima: %u proses, quantum %u ms, beban %u ms.\n",
           config.process_count, config.quantum_ms, config.workload_ms);
    puts("Status: penjadwalan dijalankan pada tahap integrasi.");
    return 0;
}
