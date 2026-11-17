#include "cli.h"

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROMPT_ATTEMPTS 3U
#define INPUT_LINE_SIZE 64

static bool parse_uint(const char *text, unsigned int *out)
{
    if (text == NULL || text[0] == '\0') {
        return false;
    }
    for (const char *scan = text; *scan != '\0'; ++scan) {
        if (*scan < '0' || *scan > '9') {
            return false;
        }
    }

    char *end = NULL;
    errno = 0;
    const unsigned long value = strtoul(text, &end, 10);

    if (errno == ERANGE || end == text || *end != '\0') {
        return false;
    }
    if (value > (unsigned long)UINT_MAX) {
        return false;
    }
    *out = (unsigned int)value;
    return true;
}

static bool accept_value(const char *label, const char *text,
                         unsigned int min, unsigned int max,
                         unsigned int *out)
{
    unsigned int value = 0U;

    if (!parse_uint(text, &value) || value < min || value > max) {
        fprintf(stderr, "%s tidak valid: %s. Harus bilangan bulat %u-%u.\n",
                label, text, min, max);
        return false;
    }
    *out = value;
    return true;
}

static void strip_newline(char *line)
{
    size_t length = strlen(line);

    while (length > 0U && (line[length - 1U] == '\n' ||
                           line[length - 1U] == '\r')) {
        line[--length] = '\0';
    }
}

/* Meminta satu nilai dari terminal; Enter kosong memakai nilai default. */
static bool prompt_value(const char *label, unsigned int min,
                         unsigned int max, unsigned int fallback,
                         unsigned int *out)
{
    char line[INPUT_LINE_SIZE];

    for (unsigned int attempt = 0U; attempt < PROMPT_ATTEMPTS; ++attempt) {
        printf("%s [%u-%u, Enter = %u]: ", label, min, max, fallback);
        fflush(stdout);

        if (fgets(line, sizeof line, stdin) == NULL) {
            printf("\n");
            *out = fallback;
            return true;
        }
        strip_newline(line);
        if (line[0] == '\0') {
            *out = fallback;
            return true;
        }
        if (accept_value(label, line, min, max, out)) {
            return true;
        }
    }

    fprintf(stderr, "Masukan tidak valid %u kali. Program dihentikan.\n",
            PROMPT_ATTEMPTS);
    return false;
}

static bool prompt_all(timesharing_config *config)
{
    puts("Masukkan konfigurasi simulasi.");
    return prompt_value("Jumlah proses", TIMESHARING_MIN_PROCESSES,
                        TIMESHARING_MAX_PROCESSES,
                        config->process_count, &config->process_count) &&
           prompt_value("Quantum (ms)", TIMESHARING_MIN_QUANTUM_MS,
                        TIMESHARING_MAX_QUANTUM_MS,
                        config->quantum_ms, &config->quantum_ms) &&
           prompt_value("Beban per proses (ms)", TIMESHARING_MIN_WORKLOAD_MS,
                        TIMESHARING_MAX_WORKLOAD_MS,
                        config->workload_ms, &config->workload_ms);
}

static bool option_matches(const char *arg, const char *longform,
                           const char *shortform)
{
    return strcmp(arg, longform) == 0 || strcmp(arg, shortform) == 0;
}

static cli_result parse_arguments(int argc, char *argv[],
                                  timesharing_config *config)
{
    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];

        if (option_matches(arg, "--help", "-h")) {
            return CLI_HELP;
        }
        if (option_matches(arg, "--version", "-v")) {
            return CLI_VERSION;
        }

        const bool wants_processes = option_matches(arg, "--processes", "-p");
        const bool wants_quantum = option_matches(arg, "--quantum", "-q");
        const bool wants_workload = option_matches(arg, "--workload", "-w");

        if (!wants_processes && !wants_quantum && !wants_workload) {
            fprintf(stderr, "Opsi tidak dikenal: %s.\n", arg);
            fprintf(stderr, "Jalankan %s --help untuk daftar opsi.\n", argv[0]);
            return CLI_ERROR;
        }
        if (i + 1 >= argc) {
            fprintf(stderr, "Opsi %s membutuhkan satu nilai.\n", arg);
            return CLI_ERROR;
        }

        const char *text = argv[++i];
        bool accepted;

        if (wants_processes) {
            accepted = accept_value("Jumlah proses", text,
                                    TIMESHARING_MIN_PROCESSES,
                                    TIMESHARING_MAX_PROCESSES,
                                    &config->process_count);
        } else if (wants_quantum) {
            accepted = accept_value("Quantum (ms)", text,
                                    TIMESHARING_MIN_QUANTUM_MS,
                                    TIMESHARING_MAX_QUANTUM_MS,
                                    &config->quantum_ms);
        } else {
            accepted = accept_value("Beban per proses (ms)", text,
                                    TIMESHARING_MIN_WORKLOAD_MS,
                                    TIMESHARING_MAX_WORKLOAD_MS,
                                    &config->workload_ms);
        }
        if (!accepted) {
            return CLI_ERROR;
        }
    }
    return CLI_OK;
}

cli_result cli_read_config(int argc, char *argv[], timesharing_config *config)
{
    if (config == NULL) {
        return CLI_ERROR;
    }
    if (argc > 1) {
        return parse_arguments(argc, argv, config);
    }
    if (!isatty(STDIN_FILENO)) {
        puts("Masukan non-interaktif, memakai konfigurasi default.");
        return CLI_OK;
    }
    return prompt_all(config) ? CLI_OK : CLI_ERROR;
}

void cli_print_usage(const char *program)
{
    printf("Penggunaan: %s [opsi]\n", program);
    puts("Tanpa opsi, program menanyakan konfigurasi lewat terminal.");
    puts("Opsi:");
    printf("  -p, --processes N  Jumlah proses (%u-%u, default %u).\n",
           TIMESHARING_MIN_PROCESSES, TIMESHARING_MAX_PROCESSES,
           TIMESHARING_DEFAULT_PROCESS_COUNT);
    printf("  -q, --quantum MS   Panjang quantum (%u-%u, default %u).\n",
           TIMESHARING_MIN_QUANTUM_MS, TIMESHARING_MAX_QUANTUM_MS,
           TIMESHARING_DEFAULT_QUANTUM_MS);
    printf("  -w, --workload MS  Beban tiap proses (%u-%u, default %u).\n",
           TIMESHARING_MIN_WORKLOAD_MS, TIMESHARING_MAX_WORKLOAD_MS,
           TIMESHARING_DEFAULT_WORKLOAD_MS);
    puts("  -v, --version      Tampilkan versi.");
    puts("  -h, --help         Tampilkan bantuan ini.");
}
