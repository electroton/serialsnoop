#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>

#include "../src/serial_port.h"

static void test_parse_baud_known_rates(void) {
    assert(serial_parse_baud(9600)   == B9600);
    assert(serial_parse_baud(19200)  == B19200);
    assert(serial_parse_baud(38400)  == B38400);
    assert(serial_parse_baud(57600)  == B57600);
    assert(serial_parse_baud(115200) == B115200);
    printf("[PASS] test_parse_baud_known_rates\n");
}

static void test_parse_baud_unknown_returns_b0(void) {
    assert(serial_parse_baud(1234) == B0);
    assert(serial_parse_baud(0)    == B0);
    assert(serial_parse_baud(-1)   == B0);
    printf("[PASS] test_parse_baud_unknown_returns_b0\n");
}

static void test_open_invalid_device(void) {
    serial_port_t port;
    int ret = serial_open(&port, "/dev/nonexistent_serial_device_xyz", B115200);
    assert(ret == -1);
    printf("[PASS] test_open_invalid_device\n");
}

static void test_close_uninitialised_port(void) {
    serial_port_t port;
    memset(&port, 0, sizeof(port));
    port.fd = -1;
    /* Should not crash or assert */
    serial_close(&port);
    assert(port.fd == -1);
    printf("[PASS] test_close_uninitialised_port\n");
}

static void test_buf_size_constant(void) {
    assert(SERIAL_BUF_SIZE == 4096);
    printf("[PASS] test_buf_size_constant\n");
}

int main(void) {
    printf("=== serial_port tests ===\n");
    test_parse_baud_known_rates();
    test_parse_baud_unknown_returns_b0();
    test_open_invalid_device();
    test_close_uninitialised_port();
    test_buf_size_constant();
    printf("All tests passed.\n");
    return 0;
}
