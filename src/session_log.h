/**
 * session_log.h - Session logging for serialsnoop
 *
 * Records captured serial data and decoded packets to a log file
 * with timestamps and session metadata.
 */

#ifndef SESSION_LOG_H
#define SESSION_LOG_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define SESSION_LOG_MAX_PATH 256
#define SESSION_LOG_HEADER   "# serialsnoop session log\n"

typedef enum {
    LOG_FORMAT_TEXT = 0,
    LOG_FORMAT_CSV,
    LOG_FORMAT_JSON
} log_format_t;

typedef struct {
    FILE        *file;
    char         path[SESSION_LOG_MAX_PATH];
    log_format_t format;
    uint64_t     bytes_written;
    uint64_t     entries_written;
    time_t       opened_at;
    bool         is_open;
} session_log_t;

/**
 * Open a session log file for writing.
 * Returns 0 on success, -1 on error.
 */
int session_log_open(session_log_t *log, const char *path, log_format_t format);

/**
 * Write a raw byte entry to the log.
 * Returns 0 on success, -1 on error.
 */
int session_log_write_raw(session_log_t *log, const uint8_t *data, size_t len,
                          const char *timestamp_str);

/**
 * Write a decoded packet entry to the log.
 * Returns 0 on success, -1 on error.
 */
int session_log_write_packet(session_log_t *log, const char *protocol,
                             const char *decoded, const char *timestamp_str);

/**
 * Flush pending writes to disk.
 */
void session_log_flush(session_log_t *log);

/**
 * Close the session log and write a summary footer.
 */
void session_log_close(session_log_t *log);

/**
 * Return true if the log is currently open.
 */
bool session_log_is_open(const session_log_t *log);

#endif /* SESSION_LOG_H */
