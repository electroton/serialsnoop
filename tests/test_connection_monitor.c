#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/connection_monitor.h"

static int event_count = 0;
static conn_event_t last_event;

static void test_event_cb(conn_event_t event, const connection_monitor_t *mon, void *userdata) {
    (void)mon;
    (void)userdata;
    last_event = event;
    event_count++;
}

static void test_init(void) {
    connection_monitor_t mon;
    int ret = conn_monitor_init(&mon, "/dev/ttyUSB0", true, 5, 1000);
    assert(ret == 0);
    assert(mon.state == CONN_STATE_DISCONNECTED);
    assert(strcmp(mon.port, "/dev/ttyUSB0") == 0);
    assert(mon.auto_reconnect == true);
    assert(mon.max_reconnect_attempts == 5);
    assert(mon.reconnect_delay_ms == 1000);
    assert(mon.reconnect_attempts == 0);
    printf("  [PASS] test_init\n");
}

static void test_state_str(void) {
    assert(strcmp(conn_monitor_state_str(CONN_STATE_DISCONNECTED), "DISCONNECTED") == 0);
    assert(strcmp(conn_monitor_state_str(CONN_STATE_CONNECTED),    "CONNECTED")    == 0);
    assert(strcmp(conn_monitor_state_str(CONN_STATE_RECONNECTING), "RECONNECTING") == 0);
    assert(strcmp(conn_monitor_state_str(CONN_STATE_ERROR),        "ERROR")        == 0);
    printf("  [PASS] test_state_str\n");
}

static void test_update_connected(void) {
    connection_monitor_t mon;
    conn_monitor_init(&mon, "/dev/ttyS0", false, 0, 0);
    conn_monitor_set_callback(test_event_cb, NULL);
    event_count = 0;

    conn_monitor_update(&mon, 3, true);
    assert(mon.state == CONN_STATE_CONNECTED);
    assert(mon.fd == 3);
    assert(event_count == 1);
    assert(last_event == CONN_EVENT_OPENED);
    printf("  [PASS] test_update_connected\n");
}

static void test_update_lost(void) {
    connection_monitor_t mon;
    conn_monitor_init(&mon, "/dev/ttyS0", false, 0, 0);
    conn_monitor_set_callback(test_event_cb, NULL);
    conn_monitor_update(&mon, 3, true);  /* connect first */
    event_count = 0;

    conn_monitor_update(&mon, -1, false);
    assert(mon.state == CONN_STATE_DISCONNECTED || mon.state == CONN_STATE_ERROR);
    assert(event_count >= 1);
    printf("  [PASS] test_update_lost\n");
}

static void test_reconnect_tracking(void) {
    connection_monitor_t mon;
    conn_monitor_init(&mon, "/dev/ttyUSB0", true, 3, 500);
    conn_monitor_set_callback(NULL, NULL);

    /* Simulate repeated failures */
    conn_monitor_update(&mon, 3, true);
    conn_monitor_update(&mon, -1, false);
    conn_monitor_update(&mon, -1, false);
    assert(mon.reconnect_attempts <= mon.max_reconnect_attempts);

    conn_monitor_reset(&mon);
    assert(mon.reconnect_attempts == 0);
    printf("  [PASS] test_reconnect_tracking\n");
}

static void test_touch(void) {
    connection_monitor_t mon;
    conn_monitor_init(&mon, "/dev/ttyS1", false, 0, 0);
    time_t before = time(NULL);
    conn_monitor_touch(&mon);
    assert(mon.last_activity >= before);
    printf("  [PASS] test_touch\n");
}

int main(void) {
    printf("Running connection_monitor tests...\n");
    test_init();
    test_state_str();
    test_update_connected();
    test_update_lost();
    test_reconnect_tracking();
    test_touch();
    printf("All connection_monitor tests passed.\n");
    return 0;
}
