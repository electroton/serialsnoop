#ifndef HEX_DUMP_H
#define HEX_DUMP_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

/* Default number of bytes per line in hex dump output */
#define HEX_DUMP_DEFAULT_WIDTH 16

/* Options for hex dump formatting */
typedef struct {
    size_t  bytes_per_line;   /* Number of bytes per line (default: 16) */
    int     show_ascii;       /* Show ASCII representation on the right */
    int     show_offset;      /* Show byte offset at start of each line */
    int     uppercase;        /* Use uppercase hex digits */
    char    separator;        /* Separator between hex bytes (default: ' ') */
} HexDumpOptions;

/**
 * Initialize options with sensible defaults.
 */
void hex_dump_options_init(HexDumpOptions *opts);

/**
 * Print a hex dump of `len` bytes from `data` to `out`.
 * Uses provided options for formatting.
 * Returns the number of lines printed, or -1 on error.
 */
int hex_dump(FILE *out, const uint8_t *data, size_t len, const HexDumpOptions *opts);

/**
 * Convenience wrapper: dump to stdout with default options.
 */
int hex_dump_default(const uint8_t *data, size_t len);

/**
 * Format a single hex dump line into `buf` (must be at least 80 bytes).
 * Returns number of characters written, or -1 on error.
 */
int hex_dump_line(char *buf, size_t buf_size,
                  const uint8_t *data, size_t data_len,
                  size_t offset, const HexDumpOptions *opts);

#endif /* HEX_DUMP_H */
