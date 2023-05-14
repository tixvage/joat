#include "buffer.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

Buffer create_buffer(void) {
    Buffer res;
    res.data = calloc(DEFAULT_BUFFER_SIZE, sizeof(char));
    res.size = DEFAULT_BUFFER_SIZE;
    res.count = DEFAULT_BUFFER_SIZE;

    return res;
}

Buffer create_buffer_with_size(int size) {
    Buffer res;
    res.data = calloc(size, sizeof(char));
    res.size = size;
    res.count = size;

    return res;
}

Buffer create_buffer_with_data(unsigned char *data, int size) {
    Buffer res;
    res.data = data;
    res.size = size;
    res.count = size;

    return res;
}

void destroy_buffer(Buffer buffer) {
    int used = buffer.size - buffer.count;
    free(buffer.data - used);
}

void *get_raw_data(Buffer *buffer, int size) {
    void *res = buffer->data;
    assert(buffer->count >= size);
    buffer->data += size;
    buffer->count -= size;

    return res;
}

utf8_t *buffer_get_utf8(Buffer *buffer) {
    utf8_t *str = get_raw_data(buffer, 0);
    int len = utf8_len(str);
    buffer->data += (len + 1) * sizeof(utf8_t);
    buffer->count -= (len + 1) * sizeof(utf8_t);

    utf8_t *final_str = calloc(len + 1, sizeof(utf8_t));
    memcpy(final_str, str, (len + 1) * sizeof(utf8_t));

    return final_str;
}

ENetPacket *prepare_packet_from_buffer(Buffer buffer) {
    int used = buffer.size - buffer.count;
    return enet_packet_create(buffer.data - used, used, ENET_PACKET_FLAG_RELIABLE);
}
