#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "../src/stats_tracker.h"

static void test_init(void) {
    stats_tracker_t t;
    stats_tracker_init(&t);
    assert(t.total_bytes == 0);
    assert(t.total_packets == 0);
    assert(t.valid_packets == 0);
    assert(t.error_packets == 0);
    assert(t.min_packet_size == UINT32_MAX);
    assert(t.max_packet_size == 0);
    printf("PASS: test_init\n");
}

static void test_record_bytes(void) {
    stats_tracker_t t;
    stats_tracker_init(&t);
    stats_tracker_record_byte(&t, 0xAA);
    stats_tracker_record_byte(&t, 0xAA);
    stats_tracker_record_byte(&t, 0xFF);
    assert(t.total_bytes == 3);
    assert(t.byte_frequency[0xAA] == 2);
    assert(t.byte_frequency[0xFF] == 1);
    printf("PASS: test_record_bytes\n");
}

static void test_record_packets(void) {
    stats_tracker_t t;
    stats_tracker_init(&t);
    stats_tracker_record_packet(&t, 10, 0);
    stats_tracker_record_packet(&t, 20, 0);
    stats_tracker_record_packet(&t, 5,  1);
    assert(t.total_packets == 3);
    assert(t.valid_packets == 2);
    assert(t.error_packets == 1);
    assert(t.min_packet_size == 5);
    assert(t.max_packet_size == 20);
    assert(t.total_packet_bytes == 35);
    printf("PASS: test_record_packets\n");
}

static void test_avg_packet_size(void) {
    stats_tracker_t t;
    stats_tracker_init(&t);
    stats_tracker_record_packet(&t, 10, 0);
    stats_tracker_record_packet(&t, 30, 0);
    double avg = stats_tracker_avg_packet_size(&t);
    assert(fabs(avg - 20.0) < 0.001);
    printf("PASS: test_avg_packet_size\n");
}

static void test_error_rate(void) {
    stats_tracker_t t;
    stats_tracker_init(&t);
    stats_tracker_record_packet(&t, 8, 0);
    stats_tracker_record_packet(&t, 8, 1);
    double rate = stats_tracker_error_rate(&t);
    assert(fabs(rate - 0.5) < 0.001);
    printf("PASS: test_error_rate\n");
}

static void test_most_frequent_byte(void) {
    stats_tracker_t t;
    stats_tracker_init(&t);
    for (int i = 0; i < 5; i++) stats_tracker_record_byte(&t, 0x55);
    for (int i = 0; i < 3; i++) stats_tracker_record_byte(&t, 0x0A);
    assert(stats_tracker_most_frequent_byte(&t) == 0x55);
    printf("PASS: test_most_frequent_byte\n");
}

static void test_gap_tracking(void) {
    stats_tracker_t t;
    stats_tracker_init(&t);
    stats_tracker_record_gap(&t, 1000);
    stats_tracker_record_gap(&t, 3000);
    assert(t.gap_count == 2);
    assert(t.max_gap_us == 3000);
    assert(t.total_gap_us == 4000);
    printf("PASS: test_gap_tracking\n");
}

static void test_reset(void) {
    stats_tracker_t t;
    stats_tracker_init(&t);
    stats_tracker_record_byte(&t, 0x01);
    stats_tracker_record_packet(&t, 16, 0);
    stats_tracker_reset(&t);
    assert(t.total_bytes == 0);
    assert(t.total_packets == 0);
    assert(t.min_packet_size == UINT32_MAX);
    printf("PASS: test_reset\n");
}

static void test_null_safety(void) {
    stats_tracker_init(NULL);
    stats_tracker_record_byte(NULL, 0x00);
    stats_tracker_record_packet(NULL, 10, 0);
    stats_tracker_record_gap(NULL, 500);
    assert(stats_tracker_avg_packet_size(NULL) == 0.0);
    assert(stats_tracker_error_rate(NULL) == 0.0);
    printf("PASS: test_null_safety\n");
}

int main(void) {
    test_init();
    test_record_bytes();
    test_record_packets();
    test_avg_packet_size();
    test_error_rate();
    test_most_frequent_byte();
    test_gap_tracking();
    test_reset();
    test_null_safety();
    printf("All stats_tracker tests passed.\n");
    return 0;
}
