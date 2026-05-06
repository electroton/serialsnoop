#ifndef OUTPUT_WRITER_H
#define OUTPUT_WRITER_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "display_formatter.h"

typedef enum {
    OUT_STDOUT = 0,
    OUT_FILE   = 1,
    OUT_BOTH   = 2
} output_dest_t;

typedef struct {
    output_dest_t    dest;
    display_config_t fmt_cfg;
    FILE            *file;        /* non-NULL when dest includes OUT_FILE */
    char             filepath[256];
    uint64_t         bytes_written;
    uint64_t         lines_written;
} output_writer_t;

/* Initialize writer; opens file if dest requires it. Returns 0 on success. */
int  output_writer_init(output_writer_t *w, output_dest_t dest,
                        const char *filepath,
                        const display_config_t *fmt_cfg);

/* Write a data buffer through the writer. Returns 0 on success. */
int  output_writer_write(output_writer_t *w,
                         const uint8_t *data, size_t len,
                         uint64_t timestamp_us);

/* Flush any pending output. */
void output_writer_flush(output_writer_t *w);

/* Close file handle if open; resets state. */
void output_writer_close(output_writer_t *w);

#endif /* OUTPUT_WRITER_H */
