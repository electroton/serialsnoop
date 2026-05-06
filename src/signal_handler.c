#include "signal_handler.h"

#include <signal.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_CALLBACKS 8

typedef struct {
    shutdown_callback_t cb;
    void *userdata;
} callback_entry_t;

static volatile sig_atomic_t g_shutdown_requested = 0;
static callback_entry_t g_callbacks[MAX_CALLBACKS];
static int g_callback_count = 0;
static int g_pipe_fds[2] = {-1, -1};

static void handle_signal(int signo) {
    (void)signo;
    g_shutdown_requested = 1;
    /* Wake up any poll() in signal_handler_wait() */
    if (g_pipe_fds[1] != -1) {
        char byte = 1;
        (void)write(g_pipe_fds[1], &byte, 1);
    }
}

int signal_handler_init(void) {
    memset(g_callbacks, 0, sizeof(g_callbacks));
    g_callback_count = 0;
    g_shutdown_requested = 0;

    if (pipe(g_pipe_fds) != 0) {
        perror("signal_handler_init: pipe");
        return -1;
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGINT, &sa, NULL) != 0 ||
        sigaction(SIGTERM, &sa, NULL) != 0) {
        perror("signal_handler_init: sigaction");
        return -1;
    }
    return 0;
}

int signal_handler_register(shutdown_callback_t cb, void *userdata) {
    if (!cb || g_callback_count >= MAX_CALLBACKS) {
        return -1;
    }
    g_callbacks[g_callback_count].cb = cb;
    g_callbacks[g_callback_count].userdata = userdata;
    g_callback_count++;
    return 0;
}

bool signal_handler_shutdown_requested(void) {
    return (bool)g_shutdown_requested;
}

bool signal_handler_wait(int timeout_ms) {
    if (g_shutdown_requested) {
        goto fire_callbacks;
    }

    struct pollfd pfd = { .fd = g_pipe_fds[0], .events = POLLIN };
    int ret = poll(&pfd, 1, timeout_ms <= 0 ? -1 : timeout_ms);

    if (ret <= 0) {
        /* timeout or error */
        return false;
    }

fire_callbacks:
    for (int i = 0; i < g_callback_count; i++) {
        g_callbacks[i].cb(g_callbacks[i].userdata);
    }
    return true;
}

void signal_handler_cleanup(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    if (g_pipe_fds[0] != -1) { close(g_pipe_fds[0]); g_pipe_fds[0] = -1; }
    if (g_pipe_fds[1] != -1) { close(g_pipe_fds[1]); g_pipe_fds[1] = -1; }
    g_callback_count = 0;
    g_shutdown_requested = 0;
}
