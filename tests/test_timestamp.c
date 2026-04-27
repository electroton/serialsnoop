#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../src/timestamp.h"

static void test_init(void)
{
    timestamp_ctx_t ctx;
    ts_init(&ctx, TS_FORMAT_RELATIVE);
    assert(ctx.initialized == 1);
    assert(ctx.format == TS_FORMAT_RELATIVE);
    printf("[PASS] ts_init\n");
}

static void test_capture_relative(void)
{
    timestamp_ctx_t ctx;
    timestamp_t ts;
    ts_init(&ctx, TS_FORMAT_RELATIVE);

    usleep(20000); /* ~20 ms */
    ts_capture(&ctx, &ts);

    assert(ts.rel_ms >= 15 && ts.rel_ms <= 200);
    assert(ts.delta_ms >= 15 && ts.delta_ms <= 200);
    printf("[PASS] ts_capture relative: rel=%llu ms delta=%llu ms\n",
           (unsigned long long)ts.rel_ms,
           (unsigned long long)ts.delta_ms);
}

static void test_capture_delta(void)
{
    timestamp_ctx_t ctx;
    timestamp_t ts1, ts2;
    ts_init(&ctx, TS_FORMAT_DELTA);

    usleep(10000);
    ts_capture(&ctx, &ts1);
    usleep(30000);
    ts_capture(&ctx, &ts2);

    assert(ts2.delta_ms >= 20);
    assert(ts2.rel_ms > ts1.rel_ms);
    printf("[PASS] ts_capture delta: ts1.rel=%llu ts2.delta=%llu\n",
           (unsigned long long)ts1.rel_ms,
           (unsigned long long)ts2.delta_ms);
}

static void test_format_relative(void)
{
    timestamp_ctx_t ctx;
    timestamp_t ts;
    char buf[64];

    ts_init(&ctx, TS_FORMAT_RELATIVE);
    usleep(5000);
    ts_capture(&ctx, &ts);
    ts_format(&ctx, &ts, buf, sizeof(buf));

    assert(strstr(buf, "ms") != NULL);
    printf("[PASS] ts_format relative: \"%s\"\n", buf);
}

static void test_format_unix(void)
{
    timestamp_ctx_t ctx;
    timestamp_t ts;
    char buf[64];

    ts_init(&ctx, TS_FORMAT_UNIX);
    ts_capture(&ctx, &ts);
    ts_format(&ctx, &ts, buf, sizeof(buf));

    /* Should contain a dot separating seconds.ms */
    assert(strchr(buf, '.') != NULL);
    printf("[PASS] ts_format unix: \"%s\"\n", buf);
}

static void test_reset_delta(void)
{
    timestamp_ctx_t ctx;
    timestamp_t ts;
    ts_init(&ctx, TS_FORMAT_DELTA);

    usleep(50000);
    ts_reset_delta(&ctx);
    usleep(5000);
    ts_capture(&ctx, &ts);

    /* delta should reflect only the time after reset */
    assert(ts.delta_ms < 40);
    printf("[PASS] ts_reset_delta: delta=%llu ms\n",
           (unsigned long long)ts.delta_ms);
}

int main(void)
{
    printf("=== timestamp tests ===\n");
    test_init();
    test_capture_relative();
    test_capture_delta();
    test_format_relative();
    test_format_unix();
    test_reset_delta();
    printf("All timestamp tests passed.\n");
    return 0;
}
