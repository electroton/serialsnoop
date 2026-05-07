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
 * buf          - pointer to raw input bytes (must not be NULL)
 * len          - number of bytes in buf (must be > 0 and <= 256)
 * timestamp_ms - capture timestamp in milliseconds
 * out          - pointer to caller-allocated decoded_packet_t (must not be NULL)
 * Returns 0 on success, -1 on error.
 */
int decoder_decode(const uint8_t *buf, size_t len,
                   uint32_t timestamp_ms,
                   decoded_packet_t *out);

/*
 * Return a human-readable protocol name string.
 * Returns "unknown" if proto is not a recognised protocol_t value.
 */
const char *decoder_protocol_name(protocol_t proto);

/*
 * Reset the decoder state without re-initialising the protocol.
 * Useful for clearing any accumulated framing state between captures.
 * Returns 0 on success, -1 if the decoder has not been initialised.
 */
int decoder_reset(void);

#endif /* PACKET_DECODER_H */
