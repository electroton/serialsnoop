#include "error_reporter.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

void error_reporter_init(error_reporter_t *reporter) {
    if (!reporter) return;
    memset(reporter, 0, sizeof(*reporter));
}

void error_reporter_record(error_reporter_t *reporter,
                           error_code_t code,
                           error_severity_t severity,
                           const char *file, int line,
                           const char *fmt, ...) {
    if (!reporter || !fmt) return;

    error_entry_t *entry = &reporter->entries[reporter->head];
    entry->code      = code;
    entry->severity  = severity;
    entry->timestamp = time(NULL);
    entry->file      = file;
    entry->line      = line;

    va_list args;
    va_start(args, fmt);
    vsnprintf(entry->message, MAX_ERROR_MSG_LEN, fmt, args);
    va_end(args);

    reporter->head = (reporter->head + 1) % MAX_ERROR_HISTORY;
    if (reporter->count < MAX_ERROR_HISTORY)
        reporter->count++;

    if (severity >= ERR_SEVERITY_ERROR)
        reporter->total_errors++;
    else if (severity == ERR_SEVERITY_WARNING)
        reporter->total_warnings++;
}

const error_entry_t *error_reporter_last(const error_reporter_t *reporter) {
    if (!reporter || reporter->count == 0) return NULL;
    int idx = (reporter->head - 1 + MAX_ERROR_HISTORY) % MAX_ERROR_HISTORY;
    return &reporter->entries[idx];
}

int error_reporter_history(const error_reporter_t *reporter,
                            error_entry_t *out, int max_count) {
    if (!reporter || !out || max_count <= 0) return 0;
    int n = reporter->count < max_count ? reporter->count : max_count;
    int start = (reporter->head - reporter->count + MAX_ERROR_HISTORY) % MAX_ERROR_HISTORY;
    for (int i = 0; i < n; i++) {
        out[i] = reporter->entries[(start + i) % MAX_ERROR_HISTORY];
    }
    return n;
}

void error_reporter_clear(error_reporter_t *reporter) {
    if (!reporter) return;
    memset(reporter, 0, sizeof(*reporter));
}

const char *error_reporter_severity_str(error_severity_t severity) {
    switch (severity) {
        case ERR_SEVERITY_INFO:    return "INFO";
        case ERR_SEVERITY_WARNING: return "WARNING";
        case ERR_SEVERITY_ERROR:   return "ERROR";
        case ERR_SEVERITY_FATAL:   return "FATAL";
        default:                   return "UNKNOWN";
    }
}

const char *error_reporter_code_str(error_code_t code) {
    switch (code) {
        case ERR_NONE:            return "NONE";
        case ERR_SERIAL_OPEN:     return "SERIAL_OPEN";
        case ERR_SERIAL_READ:     return "SERIAL_READ";
        case ERR_SERIAL_CONFIG:   return "SERIAL_CONFIG";
        case ERR_BUFFER_OVERFLOW: return "BUFFER_OVERFLOW";
        case ERR_DECODE_FAILED:   return "DECODE_FAILED";
        case ERR_LOG_WRITE:       return "LOG_WRITE";
        case ERR_INVALID_PARAM:   return "INVALID_PARAM";
        case ERR_OUT_OF_MEMORY:   return "OUT_OF_MEMORY";
        default:                  return "UNKNOWN";
    }
}
