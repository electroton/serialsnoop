/*
 * test_protocol_filter.c - Unit tests for protocol_filter module
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/protocol_filter.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) static void name(void)
#define RUN_TEST(name) do { tests_run++; name(); tests_passed++; \
    printf("  PASS: " #name "\n"); } while(0)

TEST(test_filter_init) {
    ProtocolFilter f;
    protocol_filter_init(&f);
    assert(f.mode == FILTER_MODE_PASSTHROUGH);
    assert(f.rule_count == 0);
}

TEST(test_passthrough_allows_all) {
    ProtocolFilter f;
    protocol_filter_init(&f);
    uint8_t data[] = {0x01, 0x02, 0x03};
    assert(protocol_filter_apply(&f, data, sizeof(data)) == true);
}

TEST(test_include_start_byte) {
    ProtocolFilter f;
    protocol_filter_init(&f);
    f.mode = FILTER_MODE_INCLUDE;

    FilterRule rule = {0};
    rule.type = FILTER_RULE_START_BYTE;
    rule.pattern[0] = 0xAA;
    rule.pattern_len = 1;
    protocol_filter_add_rule(&f, &rule);

    uint8_t match[]    = {0xAA, 0x01, 0x02};
    uint8_t no_match[] = {0xBB, 0x01, 0x02};
    assert(protocol_filter_apply(&f, match,    sizeof(match))    == true);
    assert(protocol_filter_apply(&f, no_match, sizeof(no_match)) == false);
}

TEST(test_exclude_pattern) {
    ProtocolFilter f;
    protocol_filter_init(&f);
    f.mode = FILTER_MODE_EXCLUDE;

    FilterRule rule = {0};
    rule.type = FILTER_RULE_PATTERN;
    rule.pattern[0] = 0xDE;
    rule.pattern[1] = 0xAD;
    rule.pattern_len = 2;
    protocol_filter_add_rule(&f, &rule);

    uint8_t blocked[] = {0x01, 0xDE, 0xAD, 0x04};
    uint8_t allowed[] = {0x01, 0x02, 0x03, 0x04};
    assert(protocol_filter_apply(&f, blocked, sizeof(blocked)) == false);
    assert(protocol_filter_apply(&f, allowed, sizeof(allowed)) == true);
}

TEST(test_min_length_filter) {
    ProtocolFilter f;
    protocol_filter_init(&f);
    f.mode = FILTER_MODE_INCLUDE;

    FilterRule rule = {0};
    rule.type = FILTER_RULE_MIN_LENGTH;
    rule.min_length = 4;
    protocol_filter_add_rule(&f, &rule);

    uint8_t short_pkt[] = {0x01, 0x02};
    uint8_t long_pkt[]  = {0x01, 0x02, 0x03, 0x04, 0x05};
    assert(protocol_filter_apply(&f, short_pkt, sizeof(short_pkt)) == false);
    assert(protocol_filter_apply(&f, long_pkt,  sizeof(long_pkt))  == true);
}

TEST(test_max_rules_limit) {
    ProtocolFilter f;
    protocol_filter_init(&f);
    FilterRule rule = {0};
    rule.type = FILTER_RULE_START_BYTE;

    for (int i = 0; i < FILTER_MAX_RULES; i++) {
        assert(protocol_filter_add_rule(&f, &rule) == true);
    }
    assert(protocol_filter_add_rule(&f, &rule) == false);
    assert(protocol_filter_rule_count(&f) == FILTER_MAX_RULES);
}

TEST(test_clear_resets_filter) {
    ProtocolFilter f;
    protocol_filter_init(&f);
    f.mode = FILTER_MODE_INCLUDE;
    FilterRule rule = {0};
    rule.type = FILTER_RULE_START_BYTE;
    protocol_filter_add_rule(&f, &rule);

    protocol_filter_clear(&f);
    assert(f.rule_count == 0);
    assert(f.mode == FILTER_MODE_PASSTHROUGH);
}

TEST(test_null_safety) {
    uint8_t data[] = {0x01};
    assert(protocol_filter_apply(NULL, data, 1)  == false);
    assert(protocol_filter_apply(NULL, NULL, 0)  == false);
    assert(protocol_filter_rule_count(NULL)       == 0);
    protocol_filter_init(NULL);  /* should not crash */
    protocol_filter_clear(NULL); /* should not crash */
}

int main(void) {
    printf("Running protocol_filter tests...\n");
    RUN_TEST(test_filter_init);
    RUN_TEST(test_passthrough_allows_all);
    RUN_TEST(test_include_start_byte);
    RUN_TEST(test_exclude_pattern);
    RUN_TEST(test_min_length_filter);
    RUN_TEST(test_max_rules_limit);
    RUN_TEST(test_clear_resets_filter);
    RUN_TEST(test_null_safety);
    printf("Results: %d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
