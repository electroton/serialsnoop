#ifndef CONNECTION_MONITOR_H
#define CONNECTION_MONITOR_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/* Connection states */
typedef enum {
    CONN_STATE_DISCONNECTED = 0,
    CONN_STATE_CONNECTED,
    CONN_STATE_RECONNECTING,
    CONN_STATE_ERROR
} conn_state_t;

/* Connection event types */
typedef enum {
    CONN_EVENT_OPENED = 0,
    CONN_EVENT_CLOSED,
    CONN_EVENT_LOST,
    CONN_EVENT_RESTORED,
    CONN_EVENT_ERROR
} conn_event_t;

typedef struct {
    conn_state_t    state;
    char            port[64];
    int             fd;
    time_t          connected_at;
    time_t          last_activity;
    uint32_t        reconnect_attempts;
    uint32_t        max_reconnect_attempts;
    uint32_t        reconnect_delay_ms;
    bool            auto_reconnect;
} connection_monitor_t;

typedef void (*conn_event_cb_t)(conn_event_t event, const connection_monitor_t *mon, void *userdata);

/* Initialize a connection monitor instance */
int  conn_monitor_init(connection_monitor_t *mon, const char *port,
                       bool auto_reconnect, uint32_t max_attempts,
                       uint32_t delay_ms);

/* Register an event callback */
void conn_monitor_set_callback(conn_event_cb_t cb, void *userdata);

/* Update monitor state (call after each read/write attempt) */
void conn_monitor_update(connection_monitor_t *mon, int fd, bool io_ok);

/* Mark activity timestamp */
void conn_monitor_touch(connection_monitor_t *mon);

/* Return human-readable state string */
const char *conn_monitor_state_str(conn_state_t state);

/* Reset reconnect counter */
void conn_monitor_reset(connection_monitor_t *mon);

#endif /* CONNECTION_MONITOR_H */
