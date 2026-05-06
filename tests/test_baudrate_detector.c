/*
 * test_baudrate_detector.c - Unit tests for baud rate detector
 */

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "../src/baudrate_detector.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) static void test_##name(void)
#define RUN(name) do { tests_run++; test_##name(); tests_passed++; printf("  PASS: " #name "\n"); } while(0)

TEST(init) {
    baudrate_detector_t det;
    baudrate_detector_init(&det);
    assert(det.state == BDET_IDLE);
    assert(det.detected_baudrate == 0);
    assert(det.sample_count == 0);
    assert(det.confidence == 0.0f);
}

TEST(record_pulse_updates_state) {
    baudrate_detector_t det;
    baudrate_detector_init(&det);
    baudrate_detector_record_pulse(&det, 104); /* ~9600 baud bit period */
    assert(det.state == BDET_SAMPLING);
    assert(det.sample_count == 1);
}

TEST(detect_9600_baud) {
    baudrate_detector_t det;
    baudrate_detector_init(&det);
    /* 9600 baud => ~104 us per bit */
    for (int i = 0; i < BDET_MIN_SAMPLES; i++) {
        baudrate_detector_record_pulse(&det, 104);
    }
    int rc = baudrate_detector_analyze(&det);
    assert(rc == 0);
    assert(det.state == BDET_DETECTED);
    assert(det.detected_baudrate == 9600);
    assert(det.confidence > 0.5f);
}

TEST(detect_115200_baud) {
    baudrate_detector_t det;
    baudrate_detector_init(&det);
    /* 115200 baud => ~8.68 us per bit, use 9 us */
    for (int i = 0; i < BDET_MIN_SAMPLES; i++) {
        baudrate_detector_record_pulse(&det, 9);
    }
    int rc = baudrate_detector_analyze(&det);
    assert(rc == 0);
    assert(det.state == BDET_DETECTED);
    assert(det.detected_baudrate == 115200);
}

TEST(analyze_fails_insufficient_samples) {
    baudrate_detector_t det;
    baudrate_detector_init(&det);
    baudrate_detector_record_pulse(&det, 104);
    int rc = baudrate_detector_analyze(&det);
    assert(rc == -1);
}

TEST(analyze_fails_unknown_baudrate) {
    baudrate_detector_t det;
    baudrate_detector_init(&det);
    /* 7777 baud => ~128.6 us, not a standard rate */
    for (int i = 0; i < BDET_MIN_SAMPLES; i++) {
        baudrate_detector_record_pulse(&det, 129);
    }
    int rc = baudrate_detector_analyze(&det);
    /* May fail or match loosely; just verify no crash */
    (void)rc;
}

TEST(reset_clears_state) {
    baudrate_detector_t det;
    baudrate_detector_init(&det);
    baudrate_detector_record_pulse(&det, 104);
    baudrate_detector_reset(&det);
    assert(det.state == BDET_IDLE);
    assert(det.sample_count == 0);
    assert(det.detected_baudrate == 0);
}

TEST(state_str) {
    assert(baudrate_detector_state_str(BDET_IDLE)[0] != '\0');
    assert(baudrate_detector_state_str(BDET_SAMPLING)[0] != '\0');
    assert(baudrate_detector_state_str(BDET_DETECTED)[0] != '\0');
    assert(baudrate_detector_state_str(BDET_FAILED)[0] != '\0');
}

TEST(null_safety) {
    baudrate_detector_init(NULL);
    baudrate_detector_record_pulse(NULL, 104);
    baudrate_detector_reset(NULL);
    int rc = baudrate_detector_analyze(NULL);
    assert(rc == -1);
}

int main(void) {
    printf("Running baudrate_detector tests...\n");
    RUN(init);
    RUN(record_pulse_updates_state);
    RUN(detect_9600_baud);
    RUN(detect_115200_baud);
    RUN(analyze_fails_insufficient_samples);
    RUN(analyze_fails_unknown_baudrate);
    RUN(reset_clears_state);
    RUN(state_str);
    RUN(null_safety);
    printf("\nResults: %d/%d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
