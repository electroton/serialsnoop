#ifndef OUTPUT_WRITER_H
#define OUTPUT_WRITER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    OUTPUT_STDOUT,
    OUTPUT_FILE,
    OUTPUT_BOTH
} output_dest_t;

typedef struct {
    output_dest_t dest;
    FILE         *file;
    bool          use_color;
    bool          flush_each_write;
} output_writer_t;

int  output_writer_init(output_writer_t *ow, output_dest_t dest,
                        const char *filepath, bool use_color);
void output_writer_close(output_writer_t *ow);
int  output_writer_write(output_writer_t *ow, const char *fmt, ...);
int  output_writer_write_bytes(output_writer_t *ow,
                               const uint8_t *data, size_t len);
void output_writer_flush(output_writer_t *ow);
bool output_writer_is_open(const output_writer_t *ow);

#endif /* OUTPUT_WRITER_H */
