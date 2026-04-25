#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RING_BUFFER_DEFAULT_CAPACITY 256

typedef struct {
    uint8_t  *data;
    size_t    capacity;
    size_t    head;      /* write index */
    size_t    tail;      /* read index  */
    size_t    count;
} ring_buffer_t;

/**
 * Initialise a ring buffer with the given capacity (bytes).
 * Returns 0 on success, -1 on allocation failure.
 */
int  ring_buffer_init(ring_buffer_t *rb, size_t capacity);

/** Release memory owned by the ring buffer. */
void ring_buffer_free(ring_buffer_t *rb);

/** Remove all data without freeing memory. */
void ring_buffer_clear(ring_buffer_t *rb);

/** Number of bytes currently stored. */
size_t ring_buffer_count(const ring_buffer_t *rb);

/** Free space remaining. */
size_t ring_buffer_free_space(const ring_buffer_t *rb);

bool ring_buffer_is_empty(const ring_buffer_t *rb);
bool ring_buffer_is_full(const ring_buffer_t *rb);

/**
 * Write up to `len` bytes from `src`.
 * Returns the number of bytes actually written.
 */
size_t ring_buffer_write(ring_buffer_t *rb, const uint8_t *src, size_t len);

/**
 * Read up to `len` bytes into `dst` (consuming them).
 * Returns the number of bytes actually read.
 */
size_t ring_buffer_read(ring_buffer_t *rb, uint8_t *dst, size_t len);

/**
 * Peek at up to `len` bytes without consuming them.
 * Returns the number of bytes copied.
 */
size_t ring_buffer_peek(const ring_buffer_t *rb, uint8_t *dst, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* RING_BUFFER_H */
