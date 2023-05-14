#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "utf8.h"
#include "rendering.h"

typedef struct Message {
    utf8_t *from;
    int id;
    utf8_t *msg;
} Message;

typedef struct Message_History {
    Message *msgs;
    int len;
    int index;
    int limit;
} Message_History;

Message_History init_message_history(int limit);
void push_message(Message_History *history, Message msg);
void render_history(Message_History *history, Renderer *renderer, int text_size);

#endif
