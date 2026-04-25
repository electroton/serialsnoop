#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/protocol_filter.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) do { printf("  [TEST] %s\n", name); tests_run++; } while(0)
#define PASS()     do { tests_passed++; } while(0)
#define FAIL(msg)  do { printf("  [FAIL] %s\n", msg); } while(0)

static void test_filter_set_init(void) {
    TEST("filter_set_init sets default_accept and zero count");
    filter_set_t fs;
    filter_set_init(&fs, true);
    assert(fs.count == 0);
    assert(fs.default_accept == true);
    PASS();
}

static void test_filter_set_add_and_remove(void) {
    TEST("filter_set_add returns valid index and remove clears it");
    filter_set_t fs;
    filter_set_init(&fs, true);

    protocol_filter_t f = {0};
    f.pattern[0] = 0xAA;
    f.mask[0]    = 0xFF;
    f.pattern_len = 1;
    f.mode = FILTER_MODE_ACCEPT;
    f.enabled = true;

    int idx = filter_set_add(&fs, &f);
    assert(idx == 0);
    assert(fs.count == 1);

    bool removed = filter_set_remove(&fs, idx);
    assert(removed == true);
    assert(fs.filters[idx].enabled == false);
    PASS();
}

static void test_evaluate_accept_on_match(void) {
    TEST("evaluate returns true when ACCEPT filter matches");
    filter_set_t fs;
    filter_set_init(&fs, false); /* default reject */

    protocol_filter_t f = {0};
    f.pattern[0] = 0x02;
    f.mask[0]    = 0xFF;
    f.pattern_len = 1;
    f.mode = FILTER_MODE_ACCEPT;
    f.enabled = true;

    filter_set_add(&fs, &f);

    uint8_t pkt[] = {0x02, 0x10, 0x20, 0x03};
    assert(filter_set_evaluate(&fs, pkt, sizeof(pkt)) == true);
    PASS();
}

static void test_evaluate_reject_on_match(void) {
    TEST("evaluate returns false when REJECT filter matches");
    filter_set_t fs;
    filter_set_init(&fs, true); /* default accept */

    protocol_filter_t f = {0};
    f.pattern[0] = 0xFF;
    f.mask[0]    = 0xFF;
    f.pattern_len = 1;
    f.mode = FILTER_MODE_REJECT;
    f.enabled = true;

    filter_set_add(&fs, &f);

    uint8_t pkt[] = {0xFF, 0x00};
    assert(filter_set_evaluate(&fs, pkt, sizeof(pkt)) == false);
    PASS();
}

static void test_evaluate_length_constraints(void) {
    TEST("evaluate respects min/max packet length constraints");
    filter_set_t fs;
    filter_set_init(&fs, false);

    protocol_filter_t f = {0};
    f.pattern[0]    = 0x01;
    f.mask[0]       = 0xFF;
    f.pattern_len   = 1;
    f.min_packet_len = 4;
    f.max_packet_len = 16;
    f.mode   = FILTER_MODE_ACCEPT;
    f.enabled = true;

    filter_set_add(&fs, &f);

    uint8_t short_pkt[] = {0x01, 0x02};          /* too short */
    uint8_t ok_pkt[]    = {0x01, 0x02, 0x03, 0x04};

    assert(filter_set_evaluate(&fs, short_pkt, sizeof(short_pkt)) == false);
    assert(filter_set_evaluate(&fs, ok_pkt, sizeof(ok_pkt)) == true);
    PASS();
}

static void test_evaluate_default_policy(void) {
    TEST("evaluate falls back to default_accept when no filter matches");
    filter_set_t fs;
    filter_set_init(&fs, true);

    uint8_t pkt[] = {0xAB, 0xCD};
    assert(filter_set_evaluate(&fs, pkt, sizeof(pkt)) == true);

    filter_set_init(&fs, false);
    assert(filter_set_evaluate(&fs, pkt, sizeof(pkt)) == false);
    PASS();
}

int main(void) {
    printf("=== protocol_filter tests ===\n");
    test_filter_set_init();
    test_filter_set_add_and_remove();
    test_evaluate_accept_on_match();
    test_evaluate_reject_on_match();
    test_evaluate_length_constraints();
    test_evaluate_default_policy();
    printf("Results: %d/%d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
