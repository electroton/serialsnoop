#include "timestamp.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

static uint64_t timespec_diff_ms(const struct timespec *a,
                                  const struct timespec *b)
{
    int64_t sec_diff  = (int64_t)a->tv_sec  - (int64_t)b->tv_sec;
    int64_t nsec_diff = (int64_t)a->tv_nsec - (int64_t)b->tv_nsec;
    return (uint64_t)(sec_diff * 1000 + nsec_diff / 1000000);
}

void ts_init(timestamp_ctx_t *ctx, ts_format_t format)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->format = format;
    clock_gettime(CLOCK_MONOTONIC, &ctx->start_time);
    ctx->last_time   = ctx->start_time;
    ctx->initialized = 1;
}

void ts_capture(timestamp_ctx_t *ctx, timestamp_t *ts)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    ts->tv       = now;
    ts->rel_ms   = timespec_diff_ms(&now, &ctx->start_time);
    ts->delta_ms = timespec_diff_ms(&now, &ctx->last_time);

    ctx->last_time = now;
}

char *ts_format(const timestamp_ctx_t *ctx, const timestamp_t *ts,
                char *buf, size_t buf_len)
{
    switch (ctx->format) {
    case TS_FORMAT_RELATIVE:
        snprintf(buf, buf_len, "+%llu ms",
                 (unsigned long long)ts->rel_ms);
        break;

    case TS_FORMAT_ABSOLUTE: {
        /* Convert monotonic to wall-clock approximation */
        struct timespec wall;
        clock_gettime(CLOCK_REALTIME, &wall);
        struct tm *tm_info = localtime(&wall.tv_sec);
        snprintf(buf, buf_len, "%02d:%02d:%02d.%03ld",
                 tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec,
                 wall.tv_nsec / 1000000);
        break;
    }

    case TS_FORMAT_UNIX:
        snprintf(buf, buf_len, "%lld.%03lld",
                 (long long)ts->tv.tv_sec,
                 (long long)(ts->tv.tv_nsec / 1000000));
        break;

    case TS_FORMAT_DELTA:
        snprintf(buf, buf_len, "d+%llu ms",
                 (unsigned long long)ts->delta_ms);
        break;

    default:
        snprintf(buf, buf_len, "?");
        break;
    }

    return buf;
}

void ts_reset_delta(timestamp_ctx_t *ctx)
{
    clock_gettime(CLOCK_MONOTONIC, &ctx->last_time);
}
