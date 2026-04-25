#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/ring_buffer.h"

static int tests_run    = 0;
static int tests_passed = 0;

#define TEST(name) static void name(void)
#define RUN(name)  do { tests_run++; name(); tests_passed++; \
                        printf("  PASS  %s\n", #name); } while(0)

TEST(test_init_and_free)
{
    ring_buffer_t rb;
    assert(ring_buffer_init(&rb, 64) == 0);
    assert(rb.capacity == 64);
    assert(ring_buffer_is_empty(&rb));
    assert(!ring_buffer_is_full(&rb));
    ring_buffer_free(&rb);
    assert(rb.data == NULL);
}

TEST(test_write_and_read)
{
    ring_buffer_t rb;
    assert(ring_buffer_init(&rb, 16) == 0);

    const uint8_t src[] = {0x01, 0x02, 0x03, 0x04};
    assert(ring_buffer_write(&rb, src, 4) == 4);
    assert(ring_buffer_count(&rb) == 4);

    uint8_t dst[4] = {0};
    assert(ring_buffer_read(&rb, dst, 4) == 4);
    assert(memcmp(src, dst, 4) == 0);
    assert(ring_buffer_is_empty(&rb));

    ring_buffer_free(&rb);
}

TEST(test_peek_does_not_consume)
{
    ring_buffer_t rb;
    assert(ring_buffer_init(&rb, 8) == 0);

    const uint8_t src[] = {0xAA, 0xBB, 0xCC};
    ring_buffer_write(&rb, src, 3);

    uint8_t peek[3] = {0};
    assert(ring_buffer_peek(&rb, peek, 3) == 3);
    assert(memcmp(src, peek, 3) == 0);
    assert(ring_buffer_count(&rb) == 3);  /* still 3 */

    ring_buffer_free(&rb);
}

TEST(test_overflow_protection)
{
    ring_buffer_t rb;
    assert(ring_buffer_init(&rb, 4) == 0);

    const uint8_t src[] = {1, 2, 3, 4, 5, 6};
    size_t written = ring_buffer_write(&rb, src, 6);
    assert(written == 4);
    assert(ring_buffer_is_full(&rb));
    /* Further writes should return 0 */
    assert(ring_buffer_write(&rb, src, 1) == 0);

    ring_buffer_free(&rb);
}

TEST(test_wrap_around)
{
    ring_buffer_t rb;
    assert(ring_buffer_init(&rb, 4) == 0);

    uint8_t tmp[4];
    const uint8_t a[] = {0x10, 0x20};
    const uint8_t b[] = {0x30, 0x40};

    ring_buffer_write(&rb, a, 2);
    ring_buffer_read(&rb, tmp, 2);  /* consume, advance tail */
    ring_buffer_write(&rb, b, 2);  /* wraps around internal array */
    assert(ring_buffer_count(&rb) == 2);
    ring_buffer_read(&rb, tmp, 2);
    assert(tmp[0] == 0x30 && tmp[1] == 0x40);

    ring_buffer_free(&rb);
}

TEST(test_clear)
{
    ring_buffer_t rb;
    assert(ring_buffer_init(&rb, 8) == 0);
    const uint8_t src[] = {1, 2, 3};
    ring_buffer_write(&rb, src, 3);
    ring_buffer_clear(&rb);
    assert(ring_buffer_is_empty(&rb));
    assert(ring_buffer_count(&rb) == 0);
    ring_buffer_free(&rb);
}

int main(void)
{
    printf("=== ring_buffer tests ===\n");
    RUN(test_init_and_free);
    RUN(test_write_and_read);
    RUN(test_peek_does_not_consume);
    RUN(test_overflow_protection);
    RUN(test_wrap_around);
    RUN(test_clear);
    printf("\n%d/%d tests passed.\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
