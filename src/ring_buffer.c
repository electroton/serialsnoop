#include "ring_buffer.h"

#include <stdlib.h>
#include <string.h>

int ring_buffer_init(ring_buffer_t *rb, size_t capacity)
{
    if (!rb || capacity == 0)
        return -1;

    rb->data = (uint8_t *)malloc(capacity);
    if (!rb->data)
        return -1;

    rb->capacity = capacity;
    rb->head     = 0;
    rb->tail     = 0;
    rb->count    = 0;
    return 0;
}

void ring_buffer_free(ring_buffer_t *rb)
{
    if (!rb) return;
    free(rb->data);
    rb->data     = NULL;
    rb->capacity = 0;
    rb->head     = 0;
    rb->tail     = 0;
    rb->count    = 0;
}

void ring_buffer_clear(ring_buffer_t *rb)
{
    if (!rb) return;
    rb->head  = 0;
    rb->tail  = 0;
    rb->count = 0;
}

size_t ring_buffer_count(const ring_buffer_t *rb)
{
    return rb ? rb->count : 0;
}

size_t ring_buffer_free_space(const ring_buffer_t *rb)
{
    return rb ? (rb->capacity - rb->count) : 0;
}

bool ring_buffer_is_empty(const ring_buffer_t *rb)
{
    return rb ? (rb->count == 0) : true;
}

bool ring_buffer_is_full(const ring_buffer_t *rb)
{
    return rb ? (rb->count == rb->capacity) : true;
}

size_t ring_buffer_write(ring_buffer_t *rb, const uint8_t *src, size_t len)
{
    if (!rb || !src || len == 0) return 0;

    size_t to_write = len;
    if (to_write > ring_buffer_free_space(rb))
        to_write = ring_buffer_free_space(rb);

    for (size_t i = 0; i < to_write; i++) {
        rb->data[rb->head] = src[i];
        rb->head = (rb->head + 1) % rb->capacity;
    }
    rb->count += to_write;
    return to_write;
}

size_t ring_buffer_read(ring_buffer_t *rb, uint8_t *dst, size_t len)
{
    if (!rb || !dst || len == 0) return 0;

    size_t to_read = len;
    if (to_read > rb->count)
        to_read = rb->count;

    for (size_t i = 0; i < to_read; i++) {
        dst[i] = rb->data[rb->tail];
        rb->tail = (rb->tail + 1) % rb->capacity;
    }
    rb->count -= to_read;
    return to_read;
}

size_t ring_buffer_peek(const ring_buffer_t *rb, uint8_t *dst, size_t len)
{
    if (!rb || !dst || len == 0) return 0;

    size_t to_peek = len;
    if (to_peek > rb->count)
        to_peek = rb->count;

    for (size_t i = 0; i < to_peek; i++)
        dst[i] = rb->data[(rb->tail + i) % rb->capacity];

    return to_peek;
}
