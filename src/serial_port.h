#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <stdint.h>
#include <termios.h>

#define SERIAL_BUF_SIZE 4096

typedef struct {
    int fd;
    char device[256];
    speed_t baud_rate;
    struct termios orig_termios;
    uint8_t buf[SERIAL_BUF_SIZE];
    size_t buf_len;
} serial_port_t;

typedef enum {
    BAUD_9600   = B9600,
    BAUD_19200  = B19200,
    BAUD_38400  = B38400,
    BAUD_57600  = B57600,
    BAUD_115200 = B115200,
} baud_rate_t;

/**
 * Open and configure a serial port for monitoring (read-only, raw mode).
 * Returns 0 on success, -1 on error.
 */
int serial_open(serial_port_t *port, const char *device, speed_t baud);

/**
 * Read available bytes from the serial port into the internal buffer.
 * Returns the number of bytes read, 0 if none available, -1 on error.
 */
ssize_t serial_read(serial_port_t *port);

/**
 * Restore original terminal settings and close the file descriptor.
 */
void serial_close(serial_port_t *port);

/**
 * Convert a numeric baud rate (e.g. 115200) to a termios speed_t constant.
 * Returns B0 if unsupported.
 */
speed_t serial_parse_baud(int baud);

#endif /* SERIAL_PORT_H */
