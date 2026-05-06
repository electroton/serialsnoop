#include "output_writer.h"
#include <string.h>
#include <stdio.h>

int output_writer_init(output_writer_t *w, output_dest_t dest,
                       const char *filepath,
                       const display_config_t *fmt_cfg) {
    if (!w) return -1;
    memset(w, 0, sizeof(*w));
    w->dest = dest;

    if (fmt_cfg)
        memcpy(&w->fmt_cfg, fmt_cfg, sizeof(display_config_t));
    else
        display_config_init(&w->fmt_cfg);

    if ((dest == OUT_FILE || dest == OUT_BOTH) && filepath) {
        strncpy(w->filepath, filepath, sizeof(w->filepath) - 1);
        w->file = fopen(filepath, "a");
        if (!w->file) {
            fprintf(stderr, "output_writer: cannot open '%s'\n", filepath);
            return -1;
        }
    }
    return 0;
}

int output_writer_write(output_writer_t *w,
                        const uint8_t *data, size_t len,
                        uint64_t timestamp_us) {
    if (!w || !data || len == 0) return -1;

    char prefix[128] = {0};
    char body[DISPLAY_MAX_LINE_LEN] = {0};

    display_format_prefix(&w->fmt_cfg, timestamp_us, (size_t)w->bytes_written,
                          prefix, sizeof(prefix));
    display_format_bytes(&w->fmt_cfg, data, len, body, sizeof(body));

    if (w->dest == OUT_STDOUT || w->dest == OUT_BOTH) {
        printf("%s%s\n", prefix, body);
    }

    if ((w->dest == OUT_FILE || w->dest == OUT_BOTH) && w->file) {
        /* Strip ANSI codes for file output by using colorize=0 clone */
        if (w->fmt_cfg.colorize) {
            display_config_t plain = w->fmt_cfg;
            plain.colorize = 0;
            char plain_prefix[128] = {0};
            char plain_body[DISPLAY_MAX_LINE_LEN] = {0};
            display_format_prefix(&plain, timestamp_us,
                                  (size_t)w->bytes_written,
                                  plain_prefix, sizeof(plain_prefix));
            display_format_bytes(&plain, data, len,
                                 plain_body, sizeof(plain_body));
            fprintf(w->file, "%s%s\n", plain_prefix, plain_body);
        } else {
            fprintf(w->file, "%s%s\n", prefix, body);
        }
    }

    w->bytes_written += (uint64_t)len;
    w->lines_written++;
    return 0;
}

void output_writer_flush(output_writer_t *w) {
    if (!w) return;
    fflush(stdout);
    if (w->file) fflush(w->file);
}

void output_writer_close(output_writer_t *w) {
    if (!w) return;
    if (w->file) {
        fclose(w->file);
        w->file = NULL;
    }
}
