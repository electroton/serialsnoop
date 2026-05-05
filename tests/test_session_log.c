/**
 * test_session_log.c - Unit tests for session_log module
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "../src/session_log.h"

#define TMP_TEXT_LOG  "/tmp/ss_test_text.log"
#define TMP_CSV_LOG   "/tmp/ss_test.csv"
#define TMP_JSON_LOG  "/tmp/ss_test.json"

static int file_contains(const char *path, const char *needle)
{
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    char buf[4096] = {0};
    fread(buf, 1, sizeof(buf) - 1, f);
    fclose(f);
    return strstr(buf, needle) != NULL;
}

static void test_open_close_text(void)
{
    session_log_t log;
    assert(session_log_open(&log, TMP_TEXT_LOG, LOG_FORMAT_TEXT) == 0);
    assert(session_log_is_open(&log));
    session_log_close(&log);
    assert(!session_log_is_open(&log));
    assert(file_contains(TMP_TEXT_LOG, "serialsnoop session log"));
    printf("PASS: test_open_close_text\n");
}

static void test_write_raw_text(void)
{
    session_log_t log;
    uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
    session_log_open(&log, TMP_TEXT_LOG, LOG_FORMAT_TEXT);
    assert(session_log_write_raw(&log, data, sizeof(data), "00:00:01.000") == 0);
    session_log_close(&log);
    assert(file_contains(TMP_TEXT_LOG, "DE AD BE EF"));
    assert(file_contains(TMP_TEXT_LOG, "00:00:01.000"));
    printf("PASS: test_write_raw_text\n");
}

static void test_write_packet_text(void)
{
    session_log_t log;
    session_log_open(&log, TMP_TEXT_LOG, LOG_FORMAT_TEXT);
    assert(session_log_write_packet(&log, "MODBUS", "FC=03 REG=0x0100",
                                    "00:00:02.500") == 0);
    session_log_close(&log);
    assert(file_contains(TMP_TEXT_LOG, "MODBUS"));
    assert(file_contains(TMP_TEXT_LOG, "FC=03 REG=0x0100"));
    printf("PASS: test_write_packet_text\n");
}

static void test_csv_format(void)
{
    session_log_t log;
    uint8_t data[] = {0xAB, 0xCD};
    session_log_open(&log, TMP_CSV_LOG, LOG_FORMAT_CSV);
    session_log_write_raw(&log, data, sizeof(data), "00:00:03.000");
    session_log_write_packet(&log, "NMEA", "$GPGGA", "00:00:03.100");
    session_log_close(&log);
    assert(file_contains(TMP_CSV_LOG, "timestamp,type,protocol,data"));
    assert(file_contains(TMP_CSV_LOG, "ABCD"));
    assert(file_contains(TMP_CSV_LOG, "NMEA"));
    printf("PASS: test_csv_format\n");
}

static void test_json_format(void)
{
    session_log_t log;
    uint8_t data[] = {0x01, 0x02};
    session_log_open(&log, TMP_JSON_LOG, LOG_FORMAT_JSON);
    session_log_write_raw(&log, data, sizeof(data), "00:00:04.000");
    session_log_write_packet(&log, "SLIP", "frame=5", "00:00:04.200");
    session_log_close(&log);
    assert(file_contains(TMP_JSON_LOG, "session_start"));
    assert(file_contains(TMP_JSON_LOG, "\"type\":\"raw\""));
    assert(file_contains(TMP_JSON_LOG, "\"type\":\"packet\""));
    assert(file_contains(TMP_JSON_LOG, "total_entries"));
    printf("PASS: test_json_format\n");
}

static void test_null_guard(void)
{
    session_log_t log;
    assert(session_log_open(NULL, TMP_TEXT_LOG, LOG_FORMAT_TEXT) == -1);
    assert(session_log_open(&log, NULL, LOG_FORMAT_TEXT) == -1);
    session_log_open(&log, TMP_TEXT_LOG, LOG_FORMAT_TEXT);
    assert(session_log_write_raw(&log, NULL, 4, "ts") == -1);
    assert(session_log_write_raw(&log, (uint8_t*)"x", 0, "ts") == -1);
    session_log_close(&log);
    printf("PASS: test_null_guard\n");
}

static void test_entries_counter(void)
{
    session_log_t log;
    uint8_t data[] = {0xFF};
    session_log_open(&log, TMP_TEXT_LOG, LOG_FORMAT_TEXT);
    session_log_write_raw(&log, data, 1, "t1");
    session_log_write_raw(&log, data, 1, "t2");
    session_log_write_packet(&log, "P", "d", "t3");
    assert(log.entries_written == 3);
    session_log_close(&log);
    printf("PASS: test_entries_counter\n");
}

int main(void)
{
    printf("=== session_log tests ===\n");
    test_open_close_text();
    test_write_raw_text();
    test_write_packet_text();
    test_csv_format();
    test_json_format();
    test_null_guard();
    test_entries_counter();
    printf("All session_log tests passed.\n");
    return 0;
}
