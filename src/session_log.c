/**
 * session_log.c - Session logging implementation
 */

#include "session_log.h"
#include <string.h>
#include <errno.h>

static void write_json_header(FILE *f, time_t opened_at)
{
    char timebuf[32];
    struct tm *tm_info = localtime(&opened_at);
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%dT%H:%M:%S", tm_info);
    fprintf(f, "{\"session_start\":\"%s\",\"entries\":[\n", timebuf);
}

static void write_csv_header(FILE *f)
{
    fprintf(f, "timestamp,type,protocol,data\n");
}

int session_log_open(session_log_t *log, const char *path, log_format_t format)
{
    if (!log || !path) return -1;

    memset(log, 0, sizeof(*log));
    strncpy(log->path, path, SESSION_LOG_MAX_PATH - 1);
    log->format = format;
    log->opened_at = time(NULL);

    log->file = fopen(path, "w");
    if (!log->file) return -1;

    switch (format) {
        case LOG_FORMAT_TEXT:
            fprintf(log->file, SESSION_LOG_HEADER);
            break;
        case LOG_FORMAT_CSV:
            write_csv_header(log->file);
            break;
        case LOG_FORMAT_JSON:
            write_json_header(log->file, log->opened_at);
            break;
    }

    log->is_open = true;
    return 0;
}

int session_log_write_raw(session_log_t *log, const uint8_t *data, size_t len,
                          const char *timestamp_str)
{
    if (!log || !log->is_open || !data || len == 0) return -1;

    int written = 0;
    if (log->format == LOG_FORMAT_TEXT) {
        written = fprintf(log->file, "[%s] RAW (%zu bytes):", timestamp_str, len);
        for (size_t i = 0; i < len; i++)
            fprintf(log->file, " %02X", data[i]);
        fprintf(log->file, "\n");
    } else if (log->format == LOG_FORMAT_CSV) {
        fprintf(log->file, "%s,raw,,", timestamp_str);
        for (size_t i = 0; i < len; i++)
            fprintf(log->file, "%02X", data[i]);
        written = fprintf(log->file, "\n");
    } else if (log->format == LOG_FORMAT_JSON) {
        if (log->entries_written > 0) fprintf(log->file, ",\n");
        fprintf(log->file, "{\"ts\":\"%s\",\"type\":\"raw\",\"data\":\"",
                timestamp_str);
        for (size_t i = 0; i < len; i++)
            fprintf(log->file, "%02X", data[i]);
        written = fprintf(log->file, "\"}");
    }

    if (written < 0) return -1;
    log->bytes_written += len;
    log->entries_written++;
    return 0;
}

int session_log_write_packet(session_log_t *log, const char *protocol,
                             const char *decoded, const char *timestamp_str)
{
    if (!log || !log->is_open || !protocol || !decoded) return -1;

    int written = 0;
    if (log->format == LOG_FORMAT_TEXT) {
        written = fprintf(log->file, "[%s] PKT [%s] %s\n",
                          timestamp_str, protocol, decoded);
    } else if (log->format == LOG_FORMAT_CSV) {
        written = fprintf(log->file, "%s,packet,%s,\"%s\"\n",
                          timestamp_str, protocol, decoded);
    } else if (log->format == LOG_FORMAT_JSON) {
        if (log->entries_written > 0) fprintf(log->file, ",\n");
        written = fprintf(log->file,
            "{\"ts\":\"%s\",\"type\":\"packet\",\"protocol\":\"%s\",\"data\":\"%s\"}",
            timestamp_str, protocol, decoded);
    }

    if (written < 0) return -1;
    log->entries_written++;
    return 0;
}

void session_log_flush(session_log_t *log)
{
    if (log && log->is_open)
        fflush(log->file);
}

void session_log_close(session_log_t *log)
{
    if (!log || !log->is_open) return;

    if (log->format == LOG_FORMAT_TEXT) {
        fprintf(log->file, "# entries: %llu  bytes: %llu\n",
                (unsigned long long)log->entries_written,
                (unsigned long long)log->bytes_written);
    } else if (log->format == LOG_FORMAT_JSON) {
        fprintf(log->file, "\n],\"total_entries\":%llu}\n",
                (unsigned long long)log->entries_written);
    }

    fclose(log->file);
    log->file = NULL;
    log->is_open = false;
}

bool session_log_is_open(const session_log_t *log)
{
    return log && log->is_open;
}
