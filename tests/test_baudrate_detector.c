#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "../src/baudrate_detector.h"

/* One bit period in ns for a given baud rate */
static uint64_t bit_period_ns(uint32_t baud) {
    return (uint64_t)(1000000000ULL / baud);
}

static void test_init(void) {
    BaudrateDetector det;
    baudrate_detector_init(&det);
    assert(det.pulse_count == 0);
    assert(det.detected_baud == 0);
    assert(det.confidence == 0);
    printf("PASS test_init\n");
}

static void test_record_pulse(void) {
    BaudrateDetector det;
    baudrate_detector_init(&det);
    assert(baudrate_detector_record_pulse(&det, 8680) == 0);
    assert(det.pulse_count == 1);
    assert(det.pulse_widths_ns[0] == 8680);
    printf("PASS test_record_pulse\n");
}

static void test_buffer_full(void) {
    BaudrateDetector det;
    baudrate_detector_init(&det);
    for (int i = 0; i < 64; i++) {
        assert(baudrate_detector_record_pulse(&det, 8680) == 0);
    }
    assert(baudrate_detector_record_pulse(&det, 8680) == -1);
    printf("PASS test_buffer_full\n");
}

static void test_analyze_115200(void) {
    BaudrateDetector det;
    baudrate_detector_init(&det);
    uint64_t period = bit_period_ns(115200); /* ~8680 ns */
    /* Feed multiples of the bit period to simulate real pulses */
    baudrate_detector_record_pulse(&det, period * 1);
    baudrate_detector_record_pulse(&det, period * 2);
    baudrate_detector_record_pulse(&det, period * 1);
    baudrate_detector_record_pulse(&det, period * 3);
    baudrate_detector_record_pulse(&det, period * 1);
    BaudrateCandidate c = baudrate_detector_analyze(&det);
    assert(c.baud_rate == 115200);
    assert(c.confidence >= BAUDRATE_CONFIDENCE_THRESHOLD);
    printf("PASS test_analyze_115200 (confidence=%d)\n", c.confidence);
}

static void test_analyze_9600(void) {
    BaudrateDetector det;
    baudrate_detector_init(&det);
    uint64_t period = bit_period_ns(9600); /* ~104167 ns */
    baudrate_detector_record_pulse(&det, period * 1);
    baudrate_detector_record_pulse(&det, period * 2);
    baudrate_detector_record_pulse(&det, period * 1);
    baudrate_detector_record_pulse(&det, period * 4);
    BaudrateCandidate c = baudrate_detector_analyze(&det);
    assert(c.baud_rate == 9600);
    assert(c.confidence >= BAUDRATE_CONFIDENCE_THRESHOLD);
    printf("PASS test_analyze_9600 (confidence=%d)\n", c.confidence);
}

static void test_reset(void) {
    BaudrateDetector det;
    baudrate_detector_init(&det);
    baudrate_detector_record_pulse(&det, 8680);
    baudrate_detector_reset(&det);
    assert(det.pulse_count == 0);
    assert(det.detected_baud == 0);
    printf("PASS test_reset\n");
}

static void test_baudrate_to_string(void) {
    assert(strcmp(baudrate_to_string(115200), "115200") == 0);
    assert(strcmp(baudrate_to_string(9600),   "9600")   == 0);
    assert(strcmp(baudrate_to_string(0),      "unknown") == 0);
    printf("PASS test_baudrate_to_string\n");
}

int main(void) {
    test_init();
    test_record_pulse();
    test_buffer_full();
    test_analyze_115200();
    test_analyze_9600();
    test_reset();
    test_baudrate_to_string();
    printf("All baudrate_detector tests passed.\n");
    return 0;
}
