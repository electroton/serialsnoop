#ifndef DISPLAY_FORMATTER_H
#define DISPLAY_FORMATTER_H

#include <stdint.h>
#include <stddef.h>

/* Output format modes */
typedef enum {
    FMT_RAW     = 0,
    FMT_HEX     = 1,
    FMT_ASCII   = 2,
    FMT_MIXED   = 3,   /* hex + printable ASCII side-by-side */
    FMT_DECIMAL = 4
} display_format_t;

/* Column width for hex dump display */
#define DISPLAY_DEFAULT_COLS 16
#define DISPLAY_MAX_LINE_LEN 256

typedef struct {
    display_format_t format;
    int              show_timestamp;
    int              show_offsets;
    int              colorize;       /* ANSI color output */
    int              cols;           /* bytes per line in hex mode */
} display_config_t;

/* Initialize config with sensible defaults */
void display_config_init(display_config_t *cfg);

/*
 * Format a buffer of bytes into a human-readable string.
 * Output written to `out` (must be at least DISPLAY_MAX_LINE_LEN bytes).
 * Returns number of characters written, or -1 on error.
 */
int display_format_bytes(const display_config_t *cfg,
                         const uint8_t *data, size_t len,
                         char *out, size_t out_size);

/*
 * Format a single line prefix: optional timestamp + offset.
 * Returns number of characters written.
 */
int display_format_prefix(const display_config_t *cfg,
                          uint64_t timestamp_us, size_t offset,
                          char *out, size_t out_size);

/* Print formatted output directly to stdout */
void display_print(const display_config_t *cfg,
                   const uint8_t *data, size_t len,
                   uint64_t timestamp_us);

#endif /* DISPLAY_FORMATTER_H */
