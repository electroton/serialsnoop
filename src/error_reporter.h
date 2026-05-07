#ifndef ERROR_REPORTER_H
#define ERROR_REPORTER_H

#include <stdint.h>
#include <time.h>

#define MAX_ERROR_MSG_LEN 256
#define MAX_ERROR_HISTORY 32

typedef enum {
    ERR_SEVERITY_INFO    = 0,
    ERR_SEVERITY_WARNING = 1,
    ERR_SEVERITY_ERROR   = 2,
    ERR_SEVERITY_FATAL   = 3
} error_severity_t;

typedef enum {
    ERR_NONE            = 0,
    ERR_SERIAL_OPEN     = 1,
    ERR_SERIAL_READ     = 2,
    ERR_SERIAL_CONFIG   = 3,
    ERR_BUFFER_OVERFLOW = 4,
    ERR_DECODE_FAILED   = 5,
    ERR_LOG_WRITE       = 6,
    ERR_INVALID_PARAM   = 7,
    ERR_OUT_OF_MEMORY   = 8
} error_code_t;

typedef struct {
    error_code_t     code;
    error_severity_t severity;
    time_t           timestamp;
    char             message[MAX_ERROR_MSG_LEN];
    const char      *file;
    int              line;
} error_entry_t;

typedef struct {
    error_entry_t entries[MAX_ERROR_HISTORY];
    int           count;
    int           head;
    uint32_t      total_errors;
    uint32_t      total_warnings;
} error_reporter_t;

void error_reporter_init(error_reporter_t *reporter);
void error_reporter_record(error_reporter_t *reporter,
                           error_code_t code,
                           error_severity_t severity,
                           const char *file, int line,
                           const char *fmt, ...);
const error_entry_t *error_reporter_last(const error_reporter_t *reporter);
int  error_reporter_history(const error_reporter_t *reporter,
                             error_entry_t *out, int max_count);
void error_reporter_clear(error_reporter_t *reporter);
const char *error_reporter_severity_str(error_severity_t severity);
const char *error_reporter_code_str(error_code_t code);

#define REPORT_ERROR(rep, code, sev, fmt, ...) \
    error_reporter_record((rep), (code), (sev), __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#endif /* ERROR_REPORTER_H */
