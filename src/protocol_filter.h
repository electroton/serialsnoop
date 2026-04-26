/*
 * protocol_filter.h - Protocol-based packet filtering for serialsnoop
 */

#ifndef PROTOCOL_FILTER_H
#define PROTOCOL_FILTER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define FILTER_MAX_RULES    16
#define FILTER_PATTERN_MAX  32

typedef enum {
    FILTER_MODE_PASSTHROUGH = 0,  /* Allow all packets */
    FILTER_MODE_INCLUDE,          /* Allow only matching packets */
    FILTER_MODE_EXCLUDE           /* Block matching packets */
} FilterMode;

typedef enum {
    FILTER_RULE_START_BYTE = 0,   /* Match on first byte */
    FILTER_RULE_PATTERN,          /* Match byte pattern anywhere */
    FILTER_RULE_MIN_LENGTH,       /* Minimum packet length */
    FILTER_RULE_MAX_LENGTH        /* Maximum packet length */
} FilterRuleType;

typedef struct {
    FilterRuleType  type;
    uint8_t         pattern[FILTER_PATTERN_MAX];
    size_t          pattern_len;
    size_t          min_length;
    size_t          max_length;
} FilterRule;

typedef struct {
    FilterMode  mode;
    FilterRule  rules[FILTER_MAX_RULES];
    size_t      rule_count;
} ProtocolFilter;

void   protocol_filter_init(ProtocolFilter *filter);
bool   protocol_filter_add_rule(ProtocolFilter *filter, const FilterRule *rule);
void   protocol_filter_clear(ProtocolFilter *filter);
bool   protocol_filter_apply(const ProtocolFilter *filter,
                             const uint8_t *data, size_t len);
size_t protocol_filter_rule_count(const ProtocolFilter *filter);

#endif /* PROTOCOL_FILTER_H */
