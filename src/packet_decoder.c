#include "packet_decoder.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

static protocol_t active_protocol = PROTO_RAW;

int decoder_init(protocol_t proto)
{
    if (proto < PROTO_RAW || proto > PROTO_MODBUS) {
        return -1;
    }
    active_protocol = proto;
    return 0;
}

const char *decoder_protocol_name(protocol_t proto)
{
    switch (proto) {
        case PROTO_RAW:    return "RAW";
        case PROTO_ASCII:  return "ASCII";
        case PROTO_HEX:    return "HEX";
        case PROTO_MODBUS: return "MODBUS";
        default:           return "UNKNOWN";
    }
}

static void decode_raw(const uint8_t *buf, size_t len, decoded_packet_t *out)
{
    size_t pos = 0;
    for (size_t i = 0; i < len && pos < sizeof(out->display) - 4; i++) {
        pos += snprintf(out->display + pos, sizeof(out->display) - pos,
                        "%02X ", buf[i]);
    }
    if (pos > 0) out->display[pos - 1] = '\0'; /* trim trailing space */
}

static void decode_ascii(const uint8_t *buf, size_t len, decoded_packet_t *out)
{
    size_t pos = 0;
    for (size_t i = 0; i < len && pos < sizeof(out->display) - 5; i++) {
        if (isprint(buf[i])) {
            out->display[pos++] = (char)buf[i];
        } else {
            pos += snprintf(out->display + pos, sizeof(out->display) - pos,
                            "[%02X]", buf[i]);
        }
    }
    out->display[pos] = '\0';
}

static void decode_modbus(const uint8_t *buf, size_t len, decoded_packet_t *out)
{
    if (len < 2) {
        snprintf(out->display, sizeof(out->display), "<short frame>");
        return;
    }
    snprintf(out->display, sizeof(out->display),
             "MODBUS addr=0x%02X func=0x%02X len=%zu",
             buf[0], buf[1], len);
}

int decoder_decode(const uint8_t *buf, size_t len,
                   uint32_t timestamp_ms,
                   decoded_packet_t *out)
{
    if (!buf || !out || len == 0 || len > sizeof(out->data)) {
        return -1;
    }

    memset(out, 0, sizeof(*out));
    out->protocol    = active_protocol;
    out->length      = len;
    out->timestamp_ms = timestamp_ms;
    memcpy(out->data, buf, len);

    switch (active_protocol) {
        case PROTO_RAW:    decode_raw(buf, len, out);    break;
        case PROTO_ASCII:  decode_ascii(buf, len, out);  break;
        case PROTO_HEX:    decode_raw(buf, len, out);    break; /* same as raw */
        case PROTO_MODBUS: decode_modbus(buf, len, out); break;
        default: return -1;
    }

    return 0;
}
