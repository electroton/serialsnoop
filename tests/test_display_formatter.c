#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/display_formatter.h"

static void test_config_init(void) {
    display_config_t cfg;
    display_config_init(&cfg);
    assert(cfg.format == FMT_MIXED);
    assert(cfg.show_timestamp == 1);
    assert(cfg.show_offsets == 1);
    assert(cfg.colorize == 0);
    assert(cfg.cols == DISPLAY_DEFAULT_COLS);
    printf("PASS: test_config_init\n");
}

static void test_format_hex(void) {
    display_config_t cfg;
    display_config_init(&cfg);
    cfg.format = FMT_HEX;
    cfg.show_timestamp = 0;
    cfg.show_offsets = 0;

    uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
    char out[128] = {0};
    int n = display_format_bytes(&cfg, data, sizeof(data), out, sizeof(out));
    assert(n > 0);
    assert(strstr(out, "DE") != NULL);
    assert(strstr(out, "AD") != NULL);
    assert(strstr(out, "BE") != NULL);
    assert(strstr(out, "EF") != NULL);
    printf("PASS: test_format_hex\n");
}

static void test_format_ascii(void) {
    display_config_t cfg;
    display_config_init(&cfg);
    cfg.format = FMT_ASCII;
    cfg.show_timestamp = 0;
    cfg.show_offsets = 0;

    uint8_t data[] = {'H', 'e', 'l', 'l', 'o', 0x01};
    char out[64] = {0};
    int n = display_format_bytes(&cfg, data, sizeof(data), out, sizeof(out));
    assert(n > 0);
    assert(out[0] == 'H');
    assert(out[4] == 'o');
    assert(out[5] == '.');   /* non-printable replaced */
    printf("PASS: test_format_ascii\n");
}

static void test_format_decimal(void) {
    display_config_t cfg;
    display_config_init(&cfg);
    cfg.format = FMT_DECIMAL;
    cfg.show_timestamp = 0;
    cfg.show_offsets = 0;

    uint8_t data[] = {0, 255, 128};
    char out[64] = {0};
    int n = display_format_bytes(&cfg, data, sizeof(data), out, sizeof(out));
    assert(n > 0);
    assert(strstr(out, "255") != NULL);
    assert(strstr(out, "128") != NULL);
    printf("PASS: test_format_decimal\n");
}

static void test_format_prefix_no_color(void) {
    display_config_t cfg;
    display_config_init(&cfg);
    cfg.show_timestamp = 1;
    cfg.show_offsets = 1;
    cfg.colorize = 0;

    char out[128] = {0};
    int n = display_format_prefix(&cfg, 1500000ULL, 0x10, out, sizeof(out));
    assert(n > 0);
    assert(strstr(out, "1.500000") != NULL);
    assert(strstr(out, "0010") != NULL);
    printf("PASS: test_format_prefix_no_color\n");
}

static void test_null_safety(void) {
    int r;
    r = display_format_bytes(NULL, NULL, 0, NULL, 0);
    assert(r == -1);
    r = display_format_prefix(NULL, 0, 0, NULL, 0);
    assert(r == -1);
    printf("PASS: test_null_safety\n");
}

static void test_mixed_format(void) {
    display_config_t cfg;
    display_config_init(&cfg);
    cfg.format = FMT_MIXED;
    cfg.show_timestamp = 0;
    cfg.show_offsets = 0;
    cfg.cols = 8;

    uint8_t data[16];
    for (int i = 0; i < 16; i++) data[i] = (uint8_t)('A' + i);
    char out[DISPLAY_MAX_LINE_LEN] = {0};
    int n = display_format_bytes(&cfg, data, sizeof(data), out, sizeof(out));
    assert(n > 0);
    assert(strstr(out, "41") != NULL);  /* 'A' in hex */
    assert(strstr(out, "A") != NULL);
    printf("PASS: test_mixed_format\n");
}

int main(void) {
    test_config_init();
    test_format_hex();
    test_format_ascii();
    test_format_decimal();
    test_format_prefix_no_color();
    test_null_safety();
    test_mixed_format();
    printf("All display_formatter tests passed.\n");
    return 0;
}
