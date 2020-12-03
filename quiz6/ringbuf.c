#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#define RINGBUF_DECL(T, NAME) \
    typedef struct {          \
        int size;             \
        int start, end;       \
        T *elements;          \
    } NAME

#define RINGBUF_INIT(BUF, S, T)             \
    {                                       \
        static T static_ringbuf_mem[S + 1]; \
        BUF.elements = static_ringbuf_mem;  \
    }                                       \
    BUF.size = S;                           \
    BUF.start = 0;                          \
    BUF.end = 0;

#define NEXT_START_INDEX(BUF) \
    (((BUF)->start != (BUF)->size) ? ((BUF)->start + 1) : 0)
#define NEXT_END_INDEX(BUF) (((BUF)->end != (BUF)->size) ? ((BUF)->end + 1) : 0)

#define is_ringbuf_empty(BUF) ((BUF)->end == (BUF)->start)
#define is_ringbuf_full(BUF) (NEXT_END_INDEX(BUF) == (BUF)->start)

#define ringbuf_write_peek(BUF) (BUF)->elements[(BUF)->end]
#define ringbuf_write_skip(BUF)                   \
    do {                                          \
        if (is_ringbuf_full(BUF))                 \
            (BUF)->start = NEXT_START_INDEX(BUF); \
        (BUF)->end = NEXT_END_INDEX(BUF);         \
    } while (0)

#define ringbuf_read_peek(BUF) (BUF)->elements[(BUF)->start]
#define ringbuf_read_skip(BUF) (BUF)->start = NEXT_START_INDEX(BUF);

#define ringbuf_write(BUF, ELEMENT)        \
    do {                                   \
        ringbuf_write_peek(BUF) = ELEMENT; \
        ringbuf_write_skip(BUF);           \
    } while (0)

#define ringbuf_read(BUF, ELEMENT)        \
    do {                                  \
        ELEMENT = ringbuf_read_peek(BUF); \
        ringbuf_read_skip(BUF);           \
    } while (0)


#define min(a, b) (a) > (b) ? (b) : (a)

#define min(a, b) (a) > (b) ? (b) : (a)
#define ringbuf_write_many(BUF, SRC, SRC_SIZE)                              \
    do {                                                                    \
        size_t part1 = min(SRC_SIZE, ((BUF)->size - (BUF)->end));           \
        size_t part1_sz = part1 * sizeof((BUF)->elements[0]);               \
        size_t part2_sz = SRC_SIZE * sizeof((BUF)->elements[0]) - part1_sz; \
        memcpy((BUF)->elements + (BUF)->end, SRC, part1_sz);                \
        memcpy((BUF)->elements, SRC + part1, part2_sz);                     \
        (BUF)->end = ((BUF)->end + SRC_SIZE) % (BUF)->size;                 \
    } while (0);

#define V_ringbuf_write_many(BUF, SRC, SRC_SIZE)            \
    do {                                                    \
        memcpy((BUF)->elements + (BUF)->end, SRC,           \
               SRC_SIZE * sizeof((BUF)->elements[0]));      \
        (BUF)->end = ((BUF)->end + SRC_SIZE) % (BUF)->size; \
    } while (0);

RINGBUF_DECL(int, int_buf);
RINGBUF_DECL(int, int_v_buf);

#define FAIL -1
int create_ring_buf(int_v_buf *buffer, size_t size, size_t type_size)
{
    void *address;
    const size_t pagesize = getpagesize();

    /* adjust size to a closely and suitable one */
    size = (ceil((float) (size * type_size) / pagesize) * pagesize) / type_size;

    const int fd = fileno(tmpfile());
    if (fd == -1)
        return FAIL;

    if (ftruncate(fd, type_size * size))
        return FAIL;


    buffer->elements = mmap(NULL, 2 * type_size * size, PROT_NONE,
                            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (buffer->elements == MAP_FAILED)
        return FAIL;

    address = mmap(buffer->elements, type_size * size, PROT_READ | PROT_WRITE,
                   MAP_SHARED | MAP_FIXED, fd, 0);

    if (address != buffer->elements)
        return FAIL;

    address = mmap(buffer->elements + size, type_size * size,
                   PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);

    if (address != buffer->elements + size)
        return FAIL;

    if (close(fd))
        return FAIL;
    return size;
}
#define V_RINGBUF_INIT(BUF, S, T)                       \
    do {                                                \
        int size = create_ring_buf(&BUF, S, sizeof(T)); \
        assert(size != FAIL);                           \
        BUF.size = size;                                \
        BUF.start = 0;                                  \
        BUF.end = 0;                                    \
    } while (0);


int main()
{
    int_buf my_buf;
    int_v_buf my_v_buf;

    RINGBUF_INIT(my_buf, 1024, int);
    V_RINGBUF_INIT(my_v_buf, 1024, int);

    int tmp[1000];
    for (int i = 0; i < 1000; i++)
        tmp[i] = rand() % 1000;


    for (int i = 0; i < 1000; i++) {
        struct timespec tt1, tt2;

        clock_gettime(CLOCK_MONOTONIC, &tt1);
        V_ringbuf_write_many(&my_v_buf, tmp, 900);
        clock_gettime(CLOCK_MONOTONIC, &tt2);
        long long time1 = (long long) (tt2.tv_sec * 1e9 + tt2.tv_nsec) -
                          (long long) (tt1.tv_sec * 1e9 + tt1.tv_nsec);

        clock_gettime(CLOCK_MONOTONIC, &tt1);
        ringbuf_write_many(&my_buf, tmp, 900);
        clock_gettime(CLOCK_MONOTONIC, &tt2);
        long long time2 = (long long) (tt2.tv_sec * 1e9 + tt2.tv_nsec) -
                          (long long) (tt1.tv_sec * 1e9 + tt1.tv_nsec);


        printf("%d, %lld, %lld\n", i, time1, time2);
    }

    /*
    for (int i = 0; i < 10; i++) {
        int read;
        ringbuf_read(&my_buf, read);
        printf("%d, ", read);
        ringbuf_read(&my_v_buf, read);
        printf("%d, \n", read);
    }
    */
    printf("\n");

    return 0;
}
