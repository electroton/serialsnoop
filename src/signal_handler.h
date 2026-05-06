#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <stdbool.h>

/*
 * signal_handler.h - Graceful shutdown and signal management for serialsnoop
 *
 * Provides a clean interface for catching SIGINT/SIGTERM and coordinating
 * an orderly shutdown of the serial monitor session.
 */

typedef void (*shutdown_callback_t)(void *userdata);

/**
 * Initialize signal handling. Must be called once at startup.
 * Returns 0 on success, -1 on error.
 */
int signal_handler_init(void);

/**
 * Register a callback to be invoked when a shutdown signal is received.
 * Up to 8 callbacks may be registered.
 * Returns 0 on success, -1 if the callback table is full.
 */
int signal_handler_register(shutdown_callback_t cb, void *userdata);

/**
 * Returns true if a shutdown signal has been received.
 */
bool signal_handler_shutdown_requested(void);

/**
 * Block until a shutdown signal is received or the optional timeout
 * (milliseconds) expires. Pass timeout_ms <= 0 to block indefinitely.
 * Returns true if shutdown was requested, false on timeout.
 */
bool signal_handler_wait(int timeout_ms);

/**
 * Tear down signal handling and restore default handlers.
 */
void signal_handler_cleanup(void);

#endif /* SIGNAL_HANDLER_H */
