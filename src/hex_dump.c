#include "hex_dump.h"

#include <string.h>
#include <ctype.h>

void hex_dump_options_init(HexDumpOptions *opts)
{
    if (!opts) return;
    opts->bytes_per_line = HEX_DUMP_DEFAULT_WIDTH;
    opts->show_ascii     = 1;
    opts->show_offset    = 1;
    opts->uppercase      = 0;
    opts->separator      = ' ';
}

int hex_dump_line(char *buf, size_t buf_size,
                  const uint8_t *data, size_t data_len,
                  size_t offset, const HexDumpOptions *opts)
{
    if (!buf || buf_size == 0 || !data || !opts) return -1;

    size_t bpl  = opts->bytes_per_line > 0 ? opts->bytes_per_line : HEX_DUMP_DEFAULT_WIDTH;
    const char *fmt_byte = opts->uppercase ? "%02X" : "%02x";
    int pos = 0;

    /* Offset column */
    if (opts->show_offset) {
        int n = snprintf(buf + pos, buf_size - (size_t)pos,
                         "%08zx  ", offset);
        if (n < 0 || (size_t)n >= buf_size - (size_t)pos) return -1;
        pos += n;
    }

    /* Hex bytes */
    for (size_t i = 0; i < bpl; i++) {
        if (i < data_len) {
            int n = snprintf(buf + pos, buf_size - (size_t)pos,
                             fmt_byte, data[i]);
            if (n < 0 || (size_t)n >= buf_size - (size_t)pos) return -1;
            pos += n;
        } else {
            /* Pad missing bytes */
            int n = snprintf(buf + pos, buf_size - (size_t)pos, "  ");
            if (n < 0) return -1;
            pos += n;
        }
        /* Separator (extra gap at midpoint) */
        char sep_str[3] = { opts->separator, '\0', '\0' };
        if (i == bpl / 2 - 1) sep_str[1] = opts->separator;
        int n = snprintf(buf + pos, buf_size - (size_t)pos, "%s", sep_str);
        if (n < 0) return -1;
        pos += n;
    }

    /* ASCII column */
    if (opts->show_ascii) {
        int n = snprintf(buf + pos, buf_size - (size_t)pos, " |");
        if (n < 0) return -1;
        pos += n;
        for (size_t i = 0; i < data_len; i++) {
            char c = isprint((unsigned char)data[i]) ? (char)data[i] : '.';
            if (pos < (int)buf_size - 1) buf[pos++] = c;
        }
        if (pos < (int)buf_size - 1) buf[pos++] = '|';
        buf[pos] = '\0';
    }

    return pos;
}

int hex_dump(FILE *out, const uint8_t *data, size_t len, const HexDumpOptions *opts)
{
    if (!out || !data || !opts) return -1;

    size_t bpl   = opts->bytes_per_line > 0 ? opts->bytes_per_line : HEX_DUMP_DEFAULT_WIDTH;
    char   line[256];
    int    lines = 0;

    for (size_t offset = 0; offset < len; offset += bpl) {
        size_t chunk = (offset + bpl <= len) ? bpl : (len - offset);
        int n = hex_dump_line(line, sizeof(line), data + offset, chunk, offset, opts);
        if (n < 0) return -1;
        fprintf(out, "%s\n", line);
        lines++;
    }
    return lines;
}

int hex_dump_default(const uint8_t *data, size_t len)
{
    HexDumpOptions opts;
    hex_dump_options_init(&opts);
    return hex_dump(stdout, data, len, &opts);
}
