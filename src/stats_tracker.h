#ifndef STATS_TRACKER_H
#define STATS_TRACKER_H

#include <stdint.h>
#include <stddef.h>
#include <time.h>

/* Per-direction statistics */
typedef struct {
    uint64_t bytes_total;
    uint64_t packets_total;
    uint64_t errors_total;
    uint64_t bytes_per_sec;   /* rolling 1s average */
    double   avg_packet_size;
} stats_direction_t;

/* Aggregate session statistics */
typedef struct {
    stats_direction_t rx;
    stats_direction_t tx;
    time_t            session_start;
    time_t            last_update;
    uint64_t          duration_sec;
    uint32_t          baud_rate;
    char              port_name[64];
} stats_tracker_t;

/**
 * Initialize a stats_tracker instance.
 * @param st        Pointer to tracker to initialize.
 * @param port_name Name of the serial port being monitored.
 * @param baud_rate Configured baud rate.
 */
void stats_init(stats_tracker_t *st, const char *port_name, uint32_t baud_rate);

/** Record received bytes (may include framing errors). */
void stats_record_rx(stats_tracker_t *st, size_t byte_count, int is_error);

/** Record transmitted bytes. */
void stats_record_tx(stats_tracker_t *st, size_t byte_count, int is_error);

/** Mark a complete packet boundary for RX or TX (direction: 0=rx, 1=tx). */
void stats_record_packet(stats_tracker_t *st, int direction);

/** Refresh time-based fields (duration, bytes_per_sec). Call periodically. */
void stats_update(stats_tracker_t *st);

/** Print a formatted summary to stdout. */
void stats_print(const stats_tracker_t *st);

/** Reset all counters but keep configuration. */
void stats_reset(stats_tracker_t *st);

#endif /* STATS_TRACKER_H */
