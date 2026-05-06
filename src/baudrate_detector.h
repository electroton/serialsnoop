#ifndef BAUDRATE_DETECTOR_H
#define BAUDRATE_DETECTOR_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Standard baud rates to probe */
static const uint32_t BAUDRATE_STANDARD[] = {
    1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600
};
#define BAUDRATE_STANDARD_COUNT 11

/* Confidence threshold (0-100) to accept a detected baud rate */
#define BAUDRATE_CONFIDENCE_THRESHOLD 70

typedef struct {
    uint32_t baud_rate;    /* Detected or candidate baud rate */
    int      confidence;   /* Confidence score 0-100 */
} BaudrateCandidate;

typedef struct {
    uint64_t pulse_widths_ns[64]; /* Captured pulse widths in nanoseconds */
    size_t   pulse_count;         /* Number of captured pulses */
    uint32_t detected_baud;       /* Best detected baud rate (0 = unknown) */
    int      confidence;          /* Confidence of detection */
} BaudrateDetector;

/**
 * Initialize the baud rate detector state.
 */
void baudrate_detector_init(BaudrateDetector *det);

/**
 * Record a pulse width (time between signal transitions) in nanoseconds.
 * Returns 0 on success, -1 if buffer is full.
 */
int baudrate_detector_record_pulse(BaudrateDetector *det, uint64_t width_ns);

/**
 * Analyze recorded pulses and populate detected_baud and confidence.
 * Returns the best BaudrateCandidate found.
 */
BaudrateCandidate baudrate_detector_analyze(BaudrateDetector *det);

/**
 * Reset detector state for a new detection session.
 */
void baudrate_detector_reset(BaudrateDetector *det);

/**
 * Return a human-readable string for a baud rate (e.g. "115200").
 * Returns "unknown" if baud_rate is 0.
 */
const char *baudrate_to_string(uint32_t baud_rate);

#ifdef __cplusplus
}
#endif

#endif /* BAUDRATE_DETECTOR_H */
