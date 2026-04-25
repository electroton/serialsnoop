#ifndef PROTOCOL_FILTER_H
#define PROTOCOL_FILTER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*
 * protocol_filter.h - Filter incoming serial data by protocol patterns
 *
 * Supports matching packets by start byte, length constraints,
 * and simple byte-pattern masks for protocol-aware capture.
 */

#define FILTER_MAX_PATTERN_LEN  16
#define FILTER_MAX_FILTERS      8

typedef enum {
    FILTER_MODE_ACCEPT,   /* Accept packets matching the filter */
    FILTER_MODE_REJECT    /* Reject packets matching the filter */
} filter_mode_t;

typedef struct {
    uint8_t  pattern[FILTER_MAX_PATTERN_LEN]; /* Byte pattern to match */
    uint8_t  mask[FILTER_MAX_PATTERN_LEN];    /* Mask applied before compare (0xFF = exact) */
    size_t   pattern_len;                     /* Number of bytes in pattern */
    size_t   min_packet_len;                  /* Minimum packet length (0 = no minimum) */
    size_t   max_packet_len;                  /* Maximum packet length (0 = no maximum) */
    filter_mode_t mode;                       /* Accept or reject on match */
    bool     enabled;                         /* Filter active flag */
} protocol_filter_t;

typedef struct {
    protocol_filter_t filters[FILTER_MAX_FILTERS];
    int               count;
    bool              default_accept;         /* Accept packets with no matching filter */
} filter_set_t;

/* Initialize a filter set with default accept policy */
void filter_set_init(filter_set_t *fs, bool default_accept);

/* Add a filter to the set; returns index or -1 on failure */
int  filter_set_add(filter_set_t *fs, const protocol_filter_t *filter);

/* Remove filter by index */
bool filter_set_remove(filter_set_t *fs, int index);

/* Evaluate a packet against the filter set; returns true if packet should be captured */
bool filter_set_evaluate(const filter_set_t *fs, const uint8_t *data, size_t len);

/* Reset all filters */
void filter_set_clear(filter_set_t *fs);

#endif /* PROTOCOL_FILTER_H */
