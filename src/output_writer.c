#include "output_writer.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int output_writer_init(output_writer_t *ow, output_dest_t dest,
                       const char *filepath, bool use_color)
{
    if (!ow) return -1;

    memset(ow, 0, sizeof(*ow));
    ow->dest       = dest;
    ow->use_color  = use_color;
    ow->flush_each_write = false;
    ow->file       = NULL;

    if (dest == OUTPUT_FILE || dest == OUTPUT_BOTH) {
        if (!filepath) return -1;
        ow->file = fopen(filepath, "a");
        if (!ow->file) {
            return -1;
        }
    }
    return 0;
}

void output_writer_close(output_writer_t *ow)
{
    if (!ow) return;
    if (ow->file) {
        fflush(ow->file);
        fclose(ow->file);
        ow->file = NULL;
    }
}

int output_writer_write(output_writer_t *ow, const char *fmt, ...)
{
    if (!ow || !fmt) return -1;

    va_list ap;
    int ret = 0;

    if (ow->dest == OUTPUT_STDOUT || ow->dest == OUTPUT_BOTH) {
        va_start(ap, fmt);
        ret = vfprintf(stdout, fmt, ap);
        va_end(ap);
        if (ow->flush_each_write) fflush(stdout);
    }

    if ((ow->dest == OUTPUT_FILE || ow->dest == OUTPUT_BOTH) && ow->file) {
        va_start(ap, fmt);
        ret = vfprintf(ow->file, fmt, ap);
        va_end(ap);
        if (ow->flush_each_write) fflush(ow->file);
    }

    return ret;
}

int output_writer_write_bytes(output_writer_t *ow,
                              const uint8_t *data, size_t len)
{
    if (!ow || !data || len == 0) return -1;

    int ret = 0;

    if (ow->dest == OUTPUT_STDOUT || ow->dest == OUTPUT_BOTH) {
        ret = (int)fwrite(data, 1, len, stdout);
        if (ow->flush_each_write) fflush(stdout);
    }

    if ((ow->dest == OUTPUT_FILE || ow->dest == OUTPUT_BOTH) && ow->file) {
        ret = (int)fwrite(data, 1, len, ow->file);
        if (ow->flush_each_write) fflush(ow->file);
    }

    return ret;
}

void output_writer_flush(output_writer_t *ow)
{
    if (!ow) return;
    if (ow->dest == OUTPUT_STDOUT || ow->dest == OUTPUT_BOTH)
        fflush(stdout);
    if ((ow->dest == OUTPUT_FILE || ow->dest == OUTPUT_BOTH) && ow->file)
        fflush(ow->file);
}

bool output_writer_is_open(const output_writer_t *ow)
{
    if (!ow) return false;
    if (ow->dest == OUTPUT_STDOUT) return true;
    if ((ow->dest == OUTPUT_FILE || ow->dest == OUTPUT_BOTH) && ow->file)
        return true;
    return false;
}
