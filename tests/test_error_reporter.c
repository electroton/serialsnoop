#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/error_reporter.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) do { tests_run++; printf("  %-40s", #name);
static void pass() { tests_passed++; printf("PASS\n"); }
#define ENDTEST } while(0)

void test_init_zeroes_reporter() {
    TEST(init_zeroes_reporter)
    error_reporter_t r;
    r.count = 99;
    error_reporter_init(&r);
    assert(r.count == 0);
    assert(r.total_errors == 0);
    assert(r.total_warnings == 0);
    pass();
    ENDTEST;
}

void test_record_single_error() {
    TEST(record_single_error)
    error_reporter_t r;
    error_reporter_init(&r);
    REPORT_ERROR(&r, ERR_SERIAL_OPEN, ERR_SEVERITY_ERROR, "Failed to open %s", "/dev/ttyUSB0");
    assert(r.count == 1);
    assert(r.total_errors == 1);
    assert(r.total_warnings == 0);
    const error_entry_t *e = error_reporter_last(&r);
    assert(e != NULL);
    assert(e->code == ERR_SERIAL_OPEN);
    assert(e->severity == ERR_SEVERITY_ERROR);
    assert(strstr(e->message, "/dev/ttyUSB0") != NULL);
    pass();
    ENDTEST;
}

void test_record_warning_counts() {
    TEST(record_warning_counts)
    error_reporter_t r;
    error_reporter_init(&r);
    REPORT_ERROR(&r, ERR_BUFFER_OVERFLOW, ERR_SEVERITY_WARNING, "Buffer near full");
    assert(r.total_warnings == 1);
    assert(r.total_errors == 0);
    pass();
    ENDTEST;
}

void test_history_retrieval() {
    TEST(history_retrieval)
    error_reporter_t r;
    error_reporter_init(&r);
    REPORT_ERROR(&r, ERR_SERIAL_READ,   ERR_SEVERITY_ERROR,   "read err");
    REPORT_ERROR(&r, ERR_DECODE_FAILED, ERR_SEVERITY_WARNING, "decode warn");
    REPORT_ERROR(&r, ERR_LOG_WRITE,     ERR_SEVERITY_ERROR,   "log err");
    error_entry_t out[8];
    int n = error_reporter_history(&r, out, 8);
    assert(n == 3);
    assert(out[0].code == ERR_SERIAL_READ);
    assert(out[2].code == ERR_LOG_WRITE);
    pass();
    ENDTEST;
}

void test_history_capped_by_max_count() {
    TEST(history_capped_by_max_count)
    error_reporter_t r;
    error_reporter_init(&r);
    for (int i = 0; i < 10; i++)
        REPORT_ERROR(&r, ERR_SERIAL_READ, ERR_SEVERITY_ERROR, "err %d", i);
    error_entry_t out[4];
    int n = error_reporter_history(&r, out, 4);
    assert(n == 4);
    pass();
    ENDTEST;
}

void test_ring_wraps_at_max_history() {
    TEST(ring_wraps_at_max_history)
    error_reporter_t r;
    error_reporter_init(&r);
    for (int i = 0; i < MAX_ERROR_HISTORY + 5; i++)
        REPORT_ERROR(&r, ERR_INVALID_PARAM, ERR_SEVERITY_WARNING, "msg %d", i);
    assert(r.count == MAX_ERROR_HISTORY);
    pass();
    ENDTEST;
}

void test_clear_resets_state() {
    TEST(clear_resets_state)
    error_reporter_t r;
    error_reporter_init(&r);
    REPORT_ERROR(&r, ERR_SERIAL_OPEN, ERR_SEVERITY_FATAL, "fatal");
    error_reporter_clear(&r);
    assert(r.count == 0);
    assert(r.total_errors == 0);
    assert(error_reporter_last(&r) == NULL);
    pass();
    ENDTEST;
}

void test_severity_and_code_strings() {
    TEST(severity_and_code_strings)
    assert(strcmp(error_reporter_severity_str(ERR_SEVERITY_FATAL),   "FATAL")   == 0);
    assert(strcmp(error_reporter_severity_str(ERR_SEVERITY_WARNING), "WARNING") == 0);
    assert(strcmp(error_reporter_code_str(ERR_BUFFER_OVERFLOW), "BUFFER_OVERFLOW") == 0);
    assert(strcmp(error_reporter_code_str(ERR_NONE),            "NONE")            == 0);
    pass();
    ENDTEST;
}

int main(void) {
    printf("=== error_reporter tests ===\n");
    test_init_zeroes_reporter();
    test_record_single_error();
    test_record_warning_counts();
    test_history_retrieval();
    test_history_capped_by_max_count();
    test_ring_wraps_at_max_history();
    test_clear_resets_state();
    test_severity_and_code_strings();
    printf("Results: %d/%d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
