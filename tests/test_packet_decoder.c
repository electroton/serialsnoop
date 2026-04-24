#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "../src/packet_decoder.h"

static int tests_run = 0;
static int tests_passed = 0;

#define CHECK(cond, msg) do { \
    tests_run++; \
    if (cond) { tests_passed++; printf("  PASS: %s\n", msg); } \
    else { printf("  FAIL: %s\n", msg); } \
} while(0)

static void test_init(void)
{
    printf("[test_init]\n");
    CHECK(decoder_init(PROTO_RAW)    == 0, "init RAW");
    CHECK(decoder_init(PROTO_ASCII)  == 0, "init ASCII");
    CHECK(decoder_init(PROTO_MODBUS) == 0, "init MODBUS");
    CHECK(decoder_init((protocol_t)99) == -1, "init invalid protocol");
}

static void test_protocol_name(void)
{
    printf("[test_protocol_name]\n");
    CHECK(strcmp(decoder_protocol_name(PROTO_RAW),    "RAW")    == 0, "name RAW");
    CHECK(strcmp(decoder_protocol_name(PROTO_ASCII),  "ASCII")  == 0, "name ASCII");
    CHECK(strcmp(decoder_protocol_name(PROTO_HEX),    "HEX")    == 0, "name HEX");
    CHECK(strcmp(decoder_protocol_name(PROTO_MODBUS), "MODBUS") == 0, "name MODBUS");
}

static void test_decode_raw(void)
{
    printf("[test_decode_raw]\n");
    decoder_init(PROTO_RAW);
    uint8_t buf[] = { 0xDE, 0xAD, 0xBE };
    decoded_packet_t pkt;
    int rc = decoder_decode(buf, sizeof(buf), 1000, &pkt);
    CHECK(rc == 0,                     "decode returns 0");
    CHECK(pkt.length == 3,             "length correct");
    CHECK(pkt.timestamp_ms == 1000,    "timestamp stored");
    CHECK(strstr(pkt.display, "DE") != NULL, "display contains DE");
    CHECK(strstr(pkt.display, "AD") != NULL, "display contains AD");
}

static void test_decode_ascii(void)
{
    printf("[test_decode_ascii]\n");
    decoder_init(PROTO_ASCII);
    uint8_t buf[] = "Hi\x01";
    decoded_packet_t pkt;
    int rc = decoder_decode(buf, 3, 500, &pkt);
    CHECK(rc == 0,                          "decode returns 0");
    CHECK(pkt.display[0] == 'H',            "first char H");
    CHECK(strstr(pkt.display, "[01]") != NULL, "non-printable escaped");
}

static void test_decode_modbus(void)
{
    printf("[test_decode_modbus]\n");
    decoder_init(PROTO_MODBUS);
    uint8_t buf[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x0A };
    decoded_packet_t pkt;
    int rc = decoder_decode(buf, sizeof(buf), 200, &pkt);
    CHECK(rc == 0,                              "decode returns 0");
    CHECK(strstr(pkt.display, "addr") != NULL,  "display has addr");
    CHECK(strstr(pkt.display, "func") != NULL,  "display has func");
}

static void test_decode_invalid(void)
{
    printf("[test_decode_invalid]\n");
    decoder_init(PROTO_RAW);
    decoded_packet_t pkt;
    CHECK(decoder_decode(NULL, 4, 0, &pkt)  == -1, "null buf rejected");
    CHECK(decoder_decode((uint8_t*)"", 0, 0, &pkt) == -1, "zero len rejected");
}

int main(void)
{
    printf("=== packet_decoder tests ===\n");
    test_init();
    test_protocol_name();
    test_decode_raw();
    test_decode_ascii();
    test_decode_modbus();
    test_decode_invalid();
    printf("\nResults: %d/%d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
