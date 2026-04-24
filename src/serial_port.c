#include "serial_port.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int serial_open(serial_port_t *port, const char *device, speed_t baud) {
    memset(port, 0, sizeof(*port));
    strncpy(port->device, device, sizeof(port->device) - 1);
    port->baud_rate = baud;

    port->fd = open(device, O_RDONLY | O_NOCTTY | O_NONBLOCK);
    if (port->fd < 0) {
        perror("serial_open: open");
        return -1;
    }

    if (tcgetattr(port->fd, &port->orig_termios) < 0) {
        perror("serial_open: tcgetattr");
        close(port->fd);
        return -1;
    }

    struct termios tty;
    memcpy(&tty, &port->orig_termios, sizeof(tty));

    cfmakeraw(&tty);
    cfsetispeed(&tty, baud);
    cfsetospeed(&tty, baud);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(port->fd, TCSANOW, &tty) < 0) {
        perror("serial_open: tcsetattr");
        close(port->fd);
        return -1;
    }

    return 0;
}

ssize_t serial_read(serial_port_t *port) {
    ssize_t n = read(port->fd, port->buf, SERIAL_BUF_SIZE);
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        perror("serial_read: read");
        return -1;
    }
    port->buf_len = (size_t)n;
    return n;
}

void serial_close(serial_port_t *port) {
    if (port->fd >= 0) {
        tcsetattr(port->fd, TCSANOW, &port->orig_termios);
        close(port->fd);
        port->fd = -1;
    }
}

speed_t serial_parse_baud(int baud) {
    switch (baud) {
        case 9600:   return B9600;
        case 19200:  return B19200;
        case 38400:  return B38400;
        case 57600:  return B57600;
        case 115200: return B115200;
        default:
            fprintf(stderr, "serial_parse_baud: unsupported baud rate %d\n", baud);
            return B0;
    }
}
