#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/hex_dump.h"

static int tests_run    = 0;
static int tests_passed = 0;

#define TEST(name) do { tests_run++; printf("  [TEST] %s ... ", name); } while(0)
#define PASS()     do { tests_passed++; printf("PASS\n"); } while(0)
#define FAIL(msg)  do { printf("FAIL: %s\n", msg); } while(0)

static void test_options_init(void)
{
    TEST("options_init defaults");
    HexDumpOptions opts;
    hex_dump_options_init(&opts);
    assert(opts.bytes_per_line == HEX_DUMP_DEFAULT_WIDTH);
    assert(opts.show_ascii  == 1);
    assert(opts.show_offset == 1);
    assert(opts.uppercase   == 0);
    assert(opts.separator   == ' ');
    PASS();
}

static void test_hex_dump_line_basic(void)
{
    TEST("hex_dump_line basic output");
    HexDumpOptions opts;
    hex_dump_options_init(&opts);
    opts.bytes_per_line = 4;

    const uint8_t data[] = { 0xDE, 0xAD, 0xBE, 0xEF };
    char buf[128];
    int n = hex_dump_line(buf, sizeof(buf), data, sizeof(data), 0, &opts);
    assert(n > 0);
    /* Should contain hex values */
    assert(strstr(buf, "de") != NULL);
    assert(strstr(buf, "ad") != NULL);
    assert(strstr(buf, "be") != NULL);
    assert(strstr(buf, "ef") != NULL);
    PASS();
}

static void test_hex_dump_line_uppercase(void)
{
    TEST("hex_dump_line uppercase");
    HexDumpOptions opts;
    hex_dump_options_init(&opts);
    opts.bytes_per_line = 4;
    opts.uppercase      = 1;

    const uint8_t data[] = { 0xAB, 0xCD };
    char buf[128];
    int n = hex_dump_line(buf, sizeof(buf), data, sizeof(data), 0, &opts);
    assert(n > 0);
    assert(strstr(buf, "AB") != NULL);
    assert(strstr(buf, "CD") != NULL);
    PASS();
}

static void test_hex_dump_line_ascii(void)
{
    TEST("hex_dump_line ascii column");
    HexDumpOptions opts;
    hex_dump_options_init(&opts);
    opts.bytes_per_line = 8;

    const uint8_t data[] = "Hello";
    char buf[256];
    int n = hex_dump_line(buf, sizeof(buf), data, 5, 0, &opts);
    assert(n > 0);
    assert(strstr(buf, "Hello") != NULL);
    PASS();
}

static void test_hex_dump_line_non_printable(void)
{
    TEST("hex_dump_line non-printable replaced with dot");
    HexDumpOptions opts;
    hex_dump_options_init(&opts);
    opts.bytes_per_line = 4;

    const uint8_t data[] = { 0x01, 0x02, 0x03, 0x04 };
    char buf[128];
    int n = hex_dump_line(buf, sizeof(buf), data, sizeof(data), 0, &opts);
    assert(n > 0);
    assert(strstr(buf, "....") != NULL);
    PASS();
}

static void test_hex_dump_offset(void)
{
    TEST("hex_dump offset column");
    HexDumpOptions opts;
    hex_dump_options_init(&opts);
    opts.bytes_per_line = 4;

    const uint8_t data[] = { 0x00, 0x01, 0x02, 0x03 };
    char buf[128];
    hex_dump_line(buf, sizeof(buf), data, sizeof(data), 0x10, &opts);
    assert(strstr(buf, "00000010") != NULL);
    PASS();
}

static void test_hex_dump_null_guards(void)
{
    TEST("hex_dump null pointer guards");
    assert(hex_dump(NULL, NULL, 0, NULL) == -1);
    assert(hex_dump_line(NULL, 0, NULL, 0, 0, NULL) == -1);
    PASS();
}

int main(void)
{
    printf("=== hex_dump tests ===\n");
    test_options_init();
    test_hex_dump_line_basic();
    test_hex_dump_line_uppercase();
    test_hex_dump_line_ascii();
    test_hex_dump_line_non_printable();
    test_hex_dump_offset();
    test_hex_dump_null_guards();
    printf("\nResults: %d/%d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
