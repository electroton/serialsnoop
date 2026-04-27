#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "../src/stats_tracker.h"

static int tests_run    = 0;
static int tests_passed = 0;

#define TEST(name) do { \
    printf("  [TEST] %s ... ", #name); \
    tests_run++; \
} while (0)

#define PASS() do { \
    printf("PASS\n"); \
    tests_passed++; \
} while (0)

#define FAIL(msg) do { \
    printf("FAIL: %s\n", msg); \
    exit(1); \
} while (0)

static void test_init(void) {
    TEST(init_zeroes_counters);
    stats_tracker_t st;
    stats_init(&st, "/dev/ttyUSB0", 115200);
    assert(st.rx.bytes_total   == 0);
    assert(st.tx.bytes_total   == 0);
    assert(st.rx.errors_total  == 0);
    assert(st.baud_rate        == 115200);
    assert(strcmp(st.port_name, "/dev/ttyUSB0") == 0);
    PASS();
}

static void test_record_rx(void) {
    TEST(record_rx_accumulates_bytes);
    stats_tracker_t st;
    stats_init(&st, "/dev/ttyS0", 9600);
    stats_record_rx(&st, 64, 0);
    stats_record_rx(&st, 32, 0);
    assert(st.rx.bytes_total == 96);
    assert(st.rx.errors_total == 0);
    PASS();

    TEST(record_rx_counts_errors);
    stats_record_rx(&st, 1, 1);
    assert(st.rx.errors_total == 1);
    assert(st.rx.bytes_total  == 97);
    PASS();
}

static void test_record_tx(void) {
    TEST(record_tx_accumulates_bytes);
    stats_tracker_t st;
    stats_init(&st, "/dev/ttyS0", 9600);
    stats_record_tx(&st, 128, 0);
    stats_record_tx(&st, 16,  0);
    assert(st.tx.bytes_total == 144);
    PASS();
}

static void test_packet_counting(void) {
    TEST(packet_count_increments);
    stats_tracker_t st;
    stats_init(&st, "/dev/ttyS1", 57600);
    stats_record_packet(&st, 0); /* rx */
    stats_record_packet(&st, 0);
    stats_record_packet(&st, 1); /* tx */
    assert(st.rx.packets_total == 2);
    assert(st.tx.packets_total == 1);
    PASS();
}

static void test_avg_packet_size(void) {
    TEST(avg_packet_size_computed);
    stats_tracker_t st;
    stats_init(&st, "/dev/ttyS2", 115200);
    stats_record_rx(&st, 100, 0);
    stats_record_packet(&st, 0);
    stats_record_rx(&st, 200, 0);
    stats_record_packet(&st, 0);
    /* avg = 300 / 2 = 150 */
    assert(st.rx.avg_packet_size == 150.0);
    PASS();
}

static void test_reset(void) {
    TEST(reset_clears_counters);
    stats_tracker_t st;
    stats_init(&st, "/dev/ttyUSB1", 19200);
    stats_record_rx(&st, 512, 0);
    stats_record_tx(&st, 256, 1);
    stats_reset(&st);
    assert(st.rx.bytes_total  == 0);
    assert(st.tx.bytes_total  == 0);
    assert(st.tx.errors_total == 0);
    assert(st.baud_rate       == 19200); /* config preserved */
    PASS();
}

static void test_update_duration(void) {
    TEST(update_sets_duration);
    stats_tracker_t st;
    stats_init(&st, "/dev/ttyUSB0", 115200);
    sleep(1);
    stats_update(&st);
    assert(st.duration_sec >= 1);
    PASS();
}

int main(void) {
    printf("=== stats_tracker tests ===\n");
    test_init();
    test_record_rx();
    test_record_tx();
    test_packet_counting();
    test_avg_packet_size();
    test_reset();
    test_update_duration();
    printf("\nResults: %d/%d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
