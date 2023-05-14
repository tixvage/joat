#ifndef _BUFFER_H
#define _BUFFER_H

#include <enet/enet.h>
#include "utf8.h"

#define DEFAULT_BUFFER_SIZE 256

typedef struct Buffer {
    unsigned char *data;
    int size;
    int count;
} Buffer;

Buffer create_buffer(void);
Buffer create_buffer_with_size(int size);
Buffer create_buffer_with_data(unsigned char *data, int size);

void destroy_buffer(Buffer buffer);

void *get_raw_data(Buffer *buffer, int size);
utf8_t *buffer_get_utf8(Buffer *buffer);

ENetPacket *prepare_packet_from_buffer(Buffer buffer);

#define buffer_push(b, v, t) do { \
    void *__data = get_raw_data(b, sizeof(t)); \
    *(t *)__data = v; \
} while(0)

#define buffer_push_utf8(b, s) do { \
    int __len = utf8_len(s); \
    utf8_t *__data = get_raw_data(b, (__len + 1) * sizeof(utf8_t)); \
    for (int __i = 0; __i < __len; __i++) __data[__i] = s[__i]; \
    __data[__len] = 0; \
} while(0)

#define buffer_get(b, t) *(t *)get_raw_data(b, sizeof(t))

#endif
