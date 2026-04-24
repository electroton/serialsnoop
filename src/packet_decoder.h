#ifndef PACKET_DECODER_H
#define PACKET_DECODER_H

#include <stdint.h>
#include <stddef.h>

/* Supported protocol types */
typedef enum {
    PROTO_RAW    = 0,
    PROTO_ASCII  = 1,
    PROTO_HEX    = 2,
    PROTO_MODBUS = 3
} protocol_t;

/* Decoded packet representation */
typedef struct {
    protocol_t  protocol;
    uint8_t     data[256];
    size_t      length;
    char        display[512];  /* human-readable decoded string */
    uint32_t    timestamp_ms;
} decoded_packet_t;

/*
 * Initialize the decoder for the given protocol.
 * Returns 0 on success, -1 on unsupported protocol.
 */
int decoder_init(protocol_t proto);

/*
 * Decode raw bytes into a decoded_packet_t.
 * Returns 0 on success, -1 on error.
 */
int decoder_decode(const uint8_t *buf, size_t len,
                   uint32_t timestamp_ms,
                   decoded_packet_t *out);

/*
 * Return a human-readable protocol name string.
 */
const char *decoder_protocol_name(protocol_t proto);

#endif /* PACKET_DECODER_H */
