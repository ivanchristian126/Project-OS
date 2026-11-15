#include "timesharing.h"

#include <stdio.h>
#include <string.h>

static void print_help(const char *program)
{
    printf("Usage: %s [--help]\n", program);
    puts("Display application information and default configuration.");
    puts("Options:");
    puts("  -h, --help  Show this help message.");
}

int main(int argc, char *argv[])
{
    const timesharing_config config = {
        .process_count = TIMESHARING_DEFAULT_PROCESS_COUNT,
        .quantum_ms = TIMESHARING_DEFAULT_QUANTUM_MS
    };

    if (argc == 2 && (strcmp(argv[1], "--help") == 0 ||
                      strcmp(argv[1], "-h") == 0)) {
        print_help(argv[0]);
        return 0;
    }
    if (argc != 1) {
        fprintf(stderr, "Unsupported argument. Run %s --help.\n", argv[0]);
        return 2;
    }

    printf("Time Sharing System v%s\n", TIMESHARING_VERSION);
    printf("Default configuration: %u processes, %u ms quantum.\n",
           config.process_count, config.quantum_ms);
    puts("Status: initial version. Scheduling is not yet available.");
    return 0;
}
