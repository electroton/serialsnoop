#include "stats_tracker.h"
#include <string.h>
#include <stdio.h>

void stats_tracker_init(stats_tracker_t *tracker) {
    if (!tracker) return;
    memset(tracker, 0, sizeof(stats_tracker_t));
    tracker->min_packet_size = UINT32_MAX;
}

void stats_tracker_record_byte(stats_tracker_t *tracker, uint8_t byte) {
    if (!tracker) return;
    tracker->total_bytes++;
    tracker->byte_frequency[byte]++;
}

void stats_tracker_record_packet(stats_tracker_t *tracker, uint32_t size, int had_error) {
    if (!tracker) return;
    tracker->total_packets++;
    if (had_error) {
        tracker->error_packets++;
    } else {
        tracker->valid_packets++;
    }
    if (size < tracker->min_packet_size) {
        tracker->min_packet_size = size;
    }
    if (size > tracker->max_packet_size) {
        tracker->max_packet_size = size;
    }
    tracker->total_packet_bytes += size;
}

void stats_tracker_record_gap(stats_tracker_t *tracker, uint64_t gap_us) {
    if (!tracker) return;
    tracker->gap_count++;
    tracker->total_gap_us += gap_us;
    if (gap_us > tracker->max_gap_us) {
        tracker->max_gap_us = gap_us;
    }
}

double stats_tracker_avg_packet_size(const stats_tracker_t *tracker) {
    if (!tracker || tracker->total_packets == 0) return 0.0;
    return (double)tracker->total_packet_bytes / tracker->total_packets;
}

double stats_tracker_error_rate(const stats_tracker_t *tracker) {
    if (!tracker || tracker->total_packets == 0) return 0.0;
    return (double)tracker->error_packets / tracker->total_packets;
}

uint8_t stats_tracker_most_frequent_byte(const stats_tracker_t *tracker) {
    if (!tracker) return 0;
    uint8_t best = 0;
    for (int i = 1; i < 256; i++) {
        if (tracker->byte_frequency[i] > tracker->byte_frequency[best]) {
            best = (uint8_t)i;
        }
    }
    return best;
}

void stats_tracker_print(const stats_tracker_t *tracker, FILE *out) {
    if (!tracker || !out) return;
    fprintf(out, "=== Serial Stats ===\n");
    fprintf(out, "Total bytes     : %lu\n", (unsigned long)tracker->total_bytes);
    fprintf(out, "Total packets   : %lu\n", (unsigned long)tracker->total_packets);
    fprintf(out, "Valid packets   : %lu\n", (unsigned long)tracker->valid_packets);
    fprintf(out, "Error packets   : %lu\n", (unsigned long)tracker->error_packets);
    fprintf(out, "Error rate      : %.2f%%\n", stats_tracker_error_rate(tracker) * 100.0);
    if (tracker->total_packets > 0) {
        fprintf(out, "Avg packet size : %.2f bytes\n", stats_tracker_avg_packet_size(tracker));
        fprintf(out, "Min packet size : %u bytes\n", tracker->min_packet_size);
        fprintf(out, "Max packet size : %u bytes\n", tracker->max_packet_size);
    }
    if (tracker->gap_count > 0) {
        fprintf(out, "Gaps detected   : %lu\n", (unsigned long)tracker->gap_count);
        fprintf(out, "Max gap         : %lu us\n", (unsigned long)tracker->max_gap_us);
        fprintf(out, "Avg gap         : %.1f us\n",
                (double)tracker->total_gap_us / tracker->gap_count);
    }
    fprintf(out, "Most freq byte  : 0x%02X\n", stats_tracker_most_frequent_byte(tracker));
}

void stats_tracker_reset(stats_tracker_t *tracker) {
    stats_tracker_init(tracker);
}
