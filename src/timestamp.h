#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <stdint.h>
#include <time.h>

/* Timestamp format options */
typedef enum {
    TS_FORMAT_RELATIVE,   /* milliseconds since capture start */
    TS_FORMAT_ABSOLUTE,   /* wall-clock HH:MM:SS.mmm */
    TS_FORMAT_UNIX,       /* unix epoch with fractional seconds */
    TS_FORMAT_DELTA       /* delta from previous event in ms */
} ts_format_t;

typedef struct {
    struct timespec tv;   /* raw monotonic time */
    uint64_t rel_ms;      /* milliseconds since capture start */
    uint64_t delta_ms;    /* milliseconds since last event */
} timestamp_t;

typedef struct {
    struct timespec start_time;  /* capture start (monotonic) */
    struct timespec last_time;   /* previous event time */
    ts_format_t format;          /* output format */
    int initialized;             /* has start_time been set */
} timestamp_ctx_t;

/* Initialize timestamp context, record capture start time */
void ts_init(timestamp_ctx_t *ctx, ts_format_t format);

/* Capture current time and populate timestamp_t */
void ts_capture(timestamp_ctx_t *ctx, timestamp_t *ts);

/* Format timestamp into buf (returns buf for convenience) */
char *ts_format(const timestamp_ctx_t *ctx, const timestamp_t *ts,
                char *buf, size_t buf_len);

/* Reset delta tracking (e.g. after a pause) */
void ts_reset_delta(timestamp_ctx_t *ctx);

#endif /* TIMESTAMP_H */
