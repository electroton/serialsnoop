/*
 * protocol_filter.c - Protocol-based packet filtering for serialsnoop
 */

#include "protocol_filter.h"
#include <string.h>
#include <stdlib.h>

static bool match_pattern(const uint8_t *data, size_t len,
                          const uint8_t *pattern, size_t pat_len) {
    if (pat_len == 0 || pat_len > len) return false;
    for (size_t i = 0; i <= len - pat_len; i++) {
        if (memcmp(data + i, pattern, pat_len) == 0) return true;
    }
    return false;
}

void protocol_filter_init(ProtocolFilter *filter) {
    if (!filter) return;
    memset(filter, 0, sizeof(ProtocolFilter));
    filter->mode = FILTER_MODE_PASSTHROUGH;
    filter->rule_count = 0;
}

bool protocol_filter_add_rule(ProtocolFilter *filter, const FilterRule *rule) {
    if (!filter || !rule) return false;
    if (filter->rule_count >= FILTER_MAX_RULES) return false;

    filter->rules[filter->rule_count] = *rule;
    filter->rule_count++;
    return true;
}

void protocol_filter_clear(ProtocolFilter *filter) {
    if (!filter) return;
    filter->rule_count = 0;
    filter->mode = FILTER_MODE_PASSTHROUGH;
}

bool protocol_filter_apply(const ProtocolFilter *filter,
                           const uint8_t *data, size_t len) {
    if (!filter || !data || len == 0) return false;

    if (filter->mode == FILTER_MODE_PASSTHROUGH) return true;

    for (size_t i = 0; i < filter->rule_count; i++) {
        const FilterRule *rule = &filter->rules[i];
        bool matched = false;

        switch (rule->type) {
            case FILTER_RULE_START_BYTE:
                matched = (len >= 1 && data[0] == rule->pattern[0]);
                break;
            case FILTER_RULE_PATTERN:
                matched = match_pattern(data, len, rule->pattern,
                                        rule->pattern_len);
                break;
            case FILTER_RULE_MIN_LENGTH:
                matched = (len >= rule->min_length);
                break;
            case FILTER_RULE_MAX_LENGTH:
                matched = (len <= rule->max_length);
                break;
            default:
                break;
        }

        if (filter->mode == FILTER_MODE_INCLUDE && matched) return true;
        if (filter->mode == FILTER_MODE_EXCLUDE && matched) return false;
    }

    return (filter->mode == FILTER_MODE_EXCLUDE);
}

size_t protocol_filter_rule_count(const ProtocolFilter *filter) {
    if (!filter) return 0;
    return filter->rule_count;
}
