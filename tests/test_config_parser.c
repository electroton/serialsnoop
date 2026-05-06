#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/config_parser.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) do { \
    printf("  [ ] %s", name); \
    tests_run++; \
} while(0)

#define PASS() do { \
    printf("\r  [x]\n"); \
    tests_passed++; \
} while(0)

#define FAIL(msg) do { \
    printf("\r  [!] FAIL: %s\n", msg); \
} while(0)

void test_config_init(void) {
    TEST("config_init creates default config");
    config_t cfg;
    config_init(&cfg);
    assert(cfg.baud_rate == 115200);
    assert(cfg.data_bits == 8);
    assert(cfg.stop_bits == 1);
    assert(cfg.verbose == 0);
    PASS();
}

void test_config_parse_valid_file(void) {
    TEST("config_parse_file loads valid key=value pairs");
    const char *tmpfile = "/tmp/serialsnoop_test.conf";
    FILE *f = fopen(tmpfile, "w");
    assert(f != NULL);
    fprintf(f, "baud_rate=9600\n");
    fprintf(f, "device=/dev/ttyUSB0\n");
    fprintf(f, "verbose=1\n");
    fprintf(f, "# this is a comment\n");
    fclose(f);

    config_t cfg;
    config_init(&cfg);
    int ret = config_parse_file(&cfg, tmpfile);
    assert(ret == 0);
    assert(cfg.baud_rate == 9600);
    assert(cfg.verbose == 1);
    assert(strcmp(cfg.device, "/dev/ttyUSB0") == 0);
    remove(tmpfile);
    PASS();
}

void test_config_parse_missing_file(void) {
    TEST("config_parse_file returns error on missing file");
    config_t cfg;
    config_init(&cfg);
    int ret = config_parse_file(&cfg, "/tmp/nonexistent_serialsnoop.conf");
    assert(ret != 0);
    PASS();
}

void test_config_parse_invalid_key(void) {
    TEST("config_parse_file ignores unknown keys");
    const char *tmpfile = "/tmp/serialsnoop_bad.conf";
    FILE *f = fopen(tmpfile, "w");
    assert(f != NULL);
    fprintf(f, "unknown_key=foobar\n");
    fprintf(f, "baud_rate=57600\n");
    fclose(f);

    config_t cfg;
    config_init(&cfg);
    int ret = config_parse_file(&cfg, tmpfile);
    assert(ret == 0);
    assert(cfg.baud_rate == 57600);
    remove(tmpfile);
    PASS();
}

void test_config_validate(void) {
    TEST("config_validate rejects invalid baud rate");
    config_t cfg;
    config_init(&cfg);
    cfg.baud_rate = 0;
    int ret = config_validate(&cfg);
    assert(ret != 0);
    PASS();
}

void test_config_validate_valid(void) {
    TEST("config_validate accepts valid config");
    config_t cfg;
    config_init(&cfg);
    strncpy(cfg.device, "/dev/ttyS0", sizeof(cfg.device) - 1);
    int ret = config_validate(&cfg);
    assert(ret == 0);
    PASS();
}

int main(void) {
    printf("=== config_parser tests ===\n");
    test_config_init();
    test_config_parse_valid_file();
    test_config_parse_missing_file();
    test_config_parse_invalid_key();
    test_config_validate();
    test_config_validate_valid();
    printf("\nResults: %d/%d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
