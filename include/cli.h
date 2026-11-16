#ifndef CLI_H
#define CLI_H

#include "timesharing.h"

typedef enum {
    CLI_OK,
    CLI_HELP,
    CLI_VERSION,
    CLI_ERROR
} cli_result;

/* Mengisi config dari argumen terminal, atau prompt bila tanpa argumen. */
cli_result cli_read_config(int argc, char *argv[], timesharing_config *config);

void cli_print_usage(const char *program);

#endif
