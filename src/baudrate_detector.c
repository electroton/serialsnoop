/*
 * baudrate_detector.c - Automatic baud rate detection via pulse timing analysis
 */

#include "baudrate_detector.h"
#include <string.h>
#include <math.h>

static const uint32_t common_baudrates[] = {
    1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600
};

#define NUM_BAUDRATES (sizeof(common_baudrates) / sizeof(common_baudrates[0]))

void baudrate_detector_init(baudrate_detector_t *det) {
    if (!det) return;
    memset(det, 0, sizeof(*det));
    det->state = BDET_IDLE;
    det->confidence = 0.0f;
    det->detected_baudrate = 0;
    det->sample_count = 0;
    det->min_pulse_us = UINT64_MAX;
}

static uint64_t gcd(uint64_t a, uint64_t b) {
    while (b) {
        uint64_t t = b;
        b = a % b;
        a = t;
    }
    return a;
}

void baudrate_detector_record_pulse(baudrate_detector_t *det, uint64_t pulse_width_us) {
    if (!det || pulse_width_us == 0) return;

    if (pulse_width_us < det->min_pulse_us)
        det->min_pulse_us = pulse_width_us;

    if (det->sample_count < BDET_MAX_SAMPLES) {
        det->pulse_widths_us[det->sample_count++] = pulse_width_us;
    }

    det->state = BDET_SAMPLING;
}

int baudrate_detector_analyze(baudrate_detector_t *det) {
    if (!det || det->sample_count < BDET_MIN_SAMPLES)
        return -1;

    /* Estimate bit period from GCD of pulse widths */
    uint64_t period = det->pulse_widths_us[0];
    for (size_t i = 1; i < det->sample_count; i++) {
        period = gcd(period, det->pulse_widths_us[i]);
        if (period == 1) break;
    }

    if (period == 0) return -1;

    uint32_t estimated_baud = (uint32_t)(1000000ULL / period);

    /* Match to nearest standard baud rate */
    uint32_t best_match = 0;
    float best_error = 1.0f;

    for (size_t i = 0; i < NUM_BAUDRATES; i++) {
        float error = fabsf((float)estimated_baud - (float)common_baudrates[i])
                      / (float)common_baudrates[i];
        if (error < best_error) {
            best_error = error;
            best_match = common_baudrates[i];
        }
    }

    if (best_error > BDET_MAX_ERROR_RATIO) {
        det->state = BDET_FAILED;
        return -1;
    }

    det->detected_baudrate = best_match;
    det->confidence = 1.0f - (best_error / BDET_MAX_ERROR_RATIO);
    det->state = BDET_DETECTED;
    return 0;
}

void baudrate_detector_reset(baudrate_detector_t *det) {
    if (!det) return;
    baudrate_detector_init(det);
}

const char *baudrate_detector_state_str(bdet_state_t state) {
    switch (state) {
        case BDET_IDLE:     return "idle";
        case BDET_SAMPLING: return "sampling";
        case BDET_DETECTED: return "detected";
        case BDET_FAILED:   return "failed";
        default:            return "unknown";
    }
}
