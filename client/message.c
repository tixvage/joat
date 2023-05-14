#include "message.h"

#include <stdlib.h>
#include "math_utils.h"

Message_History init_message_history(int limit) {
    Message_History res = { 0 };
    res.limit = limit;
    return res;
}

void push_message(Message_History *history, Message msg) {
    if (history->msgs == NULL) {
        history->msgs = calloc(1, sizeof(Message));
        history->len = 1;
    } else {
        history->msgs = realloc(history->msgs, sizeof(Message) * (history->len + 1));
        history->len += 1;
    }

    history->msgs[history->len - 1] = msg;
    history->index = history->len;
}

void render_history(Message_History *history, Renderer *renderer, int text_size) {
    for (int i = 0; i < min(history->index, history->limit); i++) {
        Message msg = history->msgs[history->index - i - 1];
        utf8_t *final_nick = calloc(utf8_len(msg.from) + 3, sizeof(utf8_t));
        final_nick[0] = '<';
        for (int i = 1; i < utf8_len(msg.from) + 1; i++) {
            final_nick[i] = msg.from[i - 1];
        }
        final_nick[utf8_len(msg.from) + 1] = '>';
        float y = GetScreenHeight() - (i * text_size) - text_size - text_size;
        render_utf8_indep(renderer, final_nick, text_size, 0, y, DARKGREEN);
        int x = measure_text_utf8(renderer, text_size, final_nick).x;
        render_utf8_indep(renderer, msg.msg, text_size, x + 10, y, WHITE);
        free(final_nick);
    }
}
