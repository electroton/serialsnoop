#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "../src/output_writer.h"

#define TMP_FILE "/tmp/test_output_writer.log"

static void cleanup(void) { unlink(TMP_FILE); }

static void test_init_stdout(void)
{
    output_writer_t ow;
    int rc = output_writer_init(&ow, OUTPUT_STDOUT, NULL, false);
    assert(rc == 0);
    assert(output_writer_is_open(&ow));
    output_writer_close(&ow);
    printf("  PASS test_init_stdout\n");
}

static void test_init_file(void)
{
    output_writer_t ow;
    int rc = output_writer_init(&ow, OUTPUT_FILE, TMP_FILE, false);
    assert(rc == 0);
    assert(output_writer_is_open(&ow));
    output_writer_close(&ow);
    assert(access(TMP_FILE, F_OK) == 0);
    cleanup();
    printf("  PASS test_init_file\n");
}

static void test_init_file_null_path(void)
{
    output_writer_t ow;
    int rc = output_writer_init(&ow, OUTPUT_FILE, NULL, false);
    assert(rc == -1);
    printf("  PASS test_init_file_null_path\n");
}

static void test_write_to_file(void)
{
    output_writer_t ow;
    int rc = output_writer_init(&ow, OUTPUT_FILE, TMP_FILE, false);
    assert(rc == 0);

    output_writer_write(&ow, "hello %s\n", "world");
    output_writer_close(&ow);

    FILE *f = fopen(TMP_FILE, "r");
    assert(f);
    char buf[64] = {0};
    fgets(buf, sizeof(buf), f);
    fclose(f);
    assert(strncmp(buf, "hello world", 11) == 0);
    cleanup();
    printf("  PASS test_write_to_file\n");
}

static void test_write_bytes_to_file(void)
{
    output_writer_t ow;
    int rc = output_writer_init(&ow, OUTPUT_FILE, TMP_FILE, false);
    assert(rc == 0);

    const uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
    int n = output_writer_write_bytes(&ow, data, sizeof(data));
    assert(n == 4);
    output_writer_close(&ow);

    FILE *f = fopen(TMP_FILE, "rb");
    assert(f);
    uint8_t rbuf[4];
    size_t rd = fread(rbuf, 1, 4, f);
    fclose(f);
    assert(rd == 4);
    assert(memcmp(rbuf, data, 4) == 0);
    cleanup();
    printf("  PASS test_write_bytes_to_file\n");
}

static void test_is_open_after_close(void)
{
    output_writer_t ow;
    output_writer_init(&ow, OUTPUT_FILE, TMP_FILE, false);
    output_writer_close(&ow);
    assert(!output_writer_is_open(&ow));
    cleanup();
    printf("  PASS test_is_open_after_close\n");
}

static void test_null_safety(void)
{
    assert(output_writer_write(NULL, "x") == -1);
    assert(output_writer_write_bytes(NULL, NULL, 0) == -1);
    assert(!output_writer_is_open(NULL));
    output_writer_close(NULL);  /* should not crash */
    output_writer_flush(NULL);  /* should not crash */
    printf("  PASS test_null_safety\n");
}

int main(void)
{
    printf("Running output_writer tests...\n");
    test_init_stdout();
    test_init_file();
    test_init_file_null_path();
    test_write_to_file();
    test_write_bytes_to_file();
    test_is_open_after_close();
    test_null_safety();
    printf("All output_writer tests passed.\n");
    return 0;
}
