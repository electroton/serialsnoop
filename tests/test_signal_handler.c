#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

#include "../src/signal_handler.h"

static int g_pass = 0;
static int g_fail = 0;

#define ASSERT(cond, msg) \
    do { \
        if (cond) { printf("  PASS: %s\n", msg); g_pass++; } \
        else       { printf("  FAIL: %s (line %d)\n", msg, __LINE__); g_fail++; } \
    } while (0)

static int g_cb_count = 0;
static void test_callback(void *userdata) {
    int *flag = (int *)userdata;
    *flag = 1;
    g_cb_count++;
}

static void test_init_cleanup(void) {
    printf("[test_init_cleanup]\n");
    int ret = signal_handler_init();
    ASSERT(ret == 0, "init returns 0");
    ASSERT(!signal_handler_shutdown_requested(), "no shutdown after init");
    signal_handler_cleanup();
    ASSERT(!signal_handler_shutdown_requested(), "no shutdown after cleanup");
}

static void test_register_callback(void) {
    printf("[test_register_callback]\n");
    signal_handler_init();
    int flag = 0;
    int ret = signal_handler_register(test_callback, &flag);
    ASSERT(ret == 0, "register returns 0");
    signal_handler_cleanup();
}

static void test_null_callback_rejected(void) {
    printf("[test_null_callback_rejected]\n");
    signal_handler_init();
    int ret = signal_handler_register(NULL, NULL);
    ASSERT(ret == -1, "null callback rejected");
    signal_handler_cleanup();
}

static void test_max_callbacks(void) {
    printf("[test_max_callbacks]\n");
    signal_handler_init();
    int dummy = 0;
    int i;
    for (i = 0; i < 8; i++) {
        ASSERT(signal_handler_register(test_callback, &dummy) == 0,
               "register within limit");
    }
    ASSERT(signal_handler_register(test_callback, &dummy) == -1,
           "register beyond limit rejected");
    signal_handler_cleanup();
}

static void test_sigint_triggers_shutdown(void) {
    printf("[test_sigint_triggers_shutdown]\n");
    g_cb_count = 0;
    int flag = 0;
    signal_handler_init();
    signal_handler_register(test_callback, &flag);

    /* Send SIGINT to ourselves */
    raise(SIGINT);

    ASSERT(signal_handler_shutdown_requested(), "shutdown requested after SIGINT");
    /* wait with short timeout — should fire callbacks immediately */
    bool result = signal_handler_wait(100);
    ASSERT(result, "wait returns true on shutdown");
    ASSERT(flag == 1, "callback was invoked");
    signal_handler_cleanup();
}

static void test_wait_timeout(void) {
    printf("[test_wait_timeout]\n");
    signal_handler_init();
    bool result = signal_handler_wait(50); /* 50 ms, no signal */
    ASSERT(!result, "wait returns false on timeout");
    signal_handler_cleanup();
}

int main(void) {
    printf("=== signal_handler tests ===\n");
    test_init_cleanup();
    test_register_callback();
    test_null_callback_rejected();
    test_max_callbacks();
    test_sigint_triggers_shutdown();
    test_wait_timeout();
    printf("\nResults: %d passed, %d failed\n", g_pass, g_fail);
    return g_fail ? EXIT_FAILURE : EXIT_SUCCESS;
}
