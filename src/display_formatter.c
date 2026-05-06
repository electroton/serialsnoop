#include "display_formatter.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* ANSI color codes */
#define ANSI_RESET   "\033[0m"
#define ANSI_CYAN    "\033[36m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_GREEN   "\033[32m"

void display_config_init(display_config_t *cfg) {
    if (!cfg) return;
    cfg->format         = FMT_MIXED;
    cfg->show_timestamp = 1;
    cfg->show_offsets   = 1;
    cfg->colorize       = 0;
    cfg->cols           = DISPLAY_DEFAULT_COLS;
}

int display_format_prefix(const display_config_t *cfg,
                          uint64_t timestamp_us, size_t offset,
                          char *out, size_t out_size) {
    int written = 0;
    if (!cfg || !out || out_size == 0) return -1;

    if (cfg->show_timestamp) {
        unsigned long sec  = (unsigned long)(timestamp_us / 1000000ULL);
        unsigned long usec = (unsigned long)(timestamp_us % 1000000ULL);
        if (cfg->colorize)
            written += snprintf(out + written, out_size - written,
                                ANSI_CYAN "[%lu.%06lu]" ANSI_RESET " ", sec, usec);
        else
            written += snprintf(out + written, out_size - written,
                                "[%lu.%06lu] ", sec, usec);
    }
    if (cfg->show_offsets) {
        if (cfg->colorize)
            written += snprintf(out + written, out_size - written,
                                ANSI_YELLOW "%04zx:" ANSI_RESET " ", offset);
        else
            written += snprintf(out + written, out_size - written,
                                "%04zx: ", offset);
    }
    return written;
}

int display_format_bytes(const display_config_t *cfg,
                         const uint8_t *data, size_t len,
                         char *out, size_t out_size) {
    if (!cfg || !data || !out || out_size == 0) return -1;

    int written = 0;
    int cols = (cfg->cols > 0) ? cfg->cols : DISPLAY_DEFAULT_COLS;

    switch (cfg->format) {
    case FMT_RAW:
        for (size_t i = 0; i < len && (size_t)written < out_size - 1; i++)
            written += snprintf(out + written, out_size - written, "%c", data[i]);
        break;

    case FMT_HEX:
        for (size_t i = 0; i < len && (size_t)written < out_size - 4; i++)
            written += snprintf(out + written, out_size - written, "%02X ", data[i]);
        break;

    case FMT_ASCII:
        for (size_t i = 0; i < len && (size_t)written < out_size - 1; i++)
            written += snprintf(out + written, out_size - written,
                                "%c", isprint(data[i]) ? data[i] : '.');
        break;

    case FMT_MIXED:
        for (size_t i = 0; i < len; i += cols) {
            size_t row = (len - i < (size_t)cols) ? len - i : (size_t)cols;
            /* hex part */
            for (size_t j = 0; j < (size_t)cols && (size_t)written < out_size - 4; j++) {
                if (j < row)
                    written += snprintf(out + written, out_size - written, "%02X ", data[i + j]);
                else
                    written += snprintf(out + written, out_size - written, "   ");
            }
            written += snprintf(out + written, out_size - written, " ");
            /* ascii part */
            if (cfg->colorize)
                written += snprintf(out + written, out_size - written, ANSI_GREEN);
            for (size_t j = 0; j < row && (size_t)written < out_size - 2; j++)
                written += snprintf(out + written, out_size - written,
                                    "%c", isprint(data[i + j]) ? data[i + j] : '.');
            if (cfg->colorize)
                written += snprintf(out + written, out_size - written, ANSI_RESET);
            if (i + cols < len)
                written += snprintf(out + written, out_size - written, "\n");
        }
        break;

    case FMT_DECIMAL:
        for (size_t i = 0; i < len && (size_t)written < out_size - 5; i++)
            written += snprintf(out + written, out_size - written, "%3u ", data[i]);
        break;
    }
    return written;
}

void display_print(const display_config_t *cfg,
                   const uint8_t *data, size_t len,
                   uint64_t timestamp_us) {
    if (!cfg || !data) return;
    char prefix[128] = {0};
    char body[DISPLAY_MAX_LINE_LEN] = {0};

    display_format_prefix(cfg, timestamp_us, 0, prefix, sizeof(prefix));
    display_format_bytes(cfg, data, len, body, sizeof(body));
    printf("%s%s\n", prefix, body);
}
