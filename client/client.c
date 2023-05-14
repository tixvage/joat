#include <enet/enet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <raylib.h>

#include "buffer.h"
#include "commands.h"
#include "networking.h"
#include "utf8.h"
#include "lerp.h"
#include "rendering.h"
#include "message.h"
#include "math_utils.h"

#define MESSAGE_DISPLAY_LIMIT 20

int text_size = DEFAULT_FONT_SIZE;

static Renderer renderer;
static Message_History history;

int main(void) {
    init_network();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(1000, 600, "joat-client");
    SetTargetFPS(60);

    renderer = init_renderer(800, 600);
    history = init_message_history(MESSAGE_DISPLAY_LIMIT);

    utf8_t *str = NULL;

    Lerp textbox_l = new_lerp(DARKBLUE, DARKGREEN, 5.f);
    Lerp textbox_r = new_lerp(DARKGREEN, DARKBLUE, 5.f);
    Lerp cursor = new_lerp(WHITE, GREEN, 0.8f);

    while (!WindowShouldClose()) {
        handle_resize_event(&renderer);

        utf8_t key = GetCharPressed();
        if (key != 0) {
            str = utf8_add_char(str, key);
        }

        if (IsKeyDown(KEY_BACKSPACE)) {
            str = utf8_remove_last(str);
        }

        if (IsKeyPressed(KEY_ENTER) && str != NULL) {
            if (str[0] == '/') {
                if (utf8_compare_subcstr(str, "nick ", 1)) {
                    utf8_t *nick = &str[6];

                    Buffer send = create_buffer();
                    buffer_push(&send, client.id, int);
                    buffer_push(&send, Command_ChangeUsername, Command);
                    buffer_push_utf8(&send, nick);
                    enet_peer_send(client.peer, 0, prepare_packet_from_buffer(send));
                    destroy_buffer(send);
                }
            } else {
                Buffer send = create_buffer();
                buffer_push(&send, client.id, int);
                buffer_push(&send, Command_SendMessage, Command);
                buffer_push_utf8(&send, str);
                enet_peer_send(client.peer, 0, prepare_packet_from_buffer(send));
                destroy_buffer(send);
            }
            free(str);
            str = NULL;
        }
        
        int scroll = GetMouseWheelMove();
        history.index = clamp(history.index - scroll, MESSAGE_DISPLAY_LIMIT, history.len);

        BeginDrawing();
        ClearBackground((Color){51, 12, 12, 255});
        {
            Rectangle textbox_rect = { 0 };
            textbox_rect.width = GetScreenWidth() - 20;
            textbox_rect.height = text_size;
            textbox_rect.x = 10;
            textbox_rect.y = GetScreenHeight() - text_size;
            Color l = update_lerp(&textbox_l), r = update_lerp(&textbox_r);
            DrawRectangleGradientEx(textbox_rect, l, l, r, r);
            DrawCircle(10, textbox_rect.y + (textbox_rect.height / 2), textbox_rect.height / 2, l);
            DrawCircle(GetScreenWidth() - 10, textbox_rect.y + (textbox_rect.height / 2), textbox_rect.height / 2, r);
        }
        render_history(&history, &renderer, text_size);
        Rectangle cursor_rect = { 0 };
        cursor_rect.width = text_size / 3;
        cursor_rect.height = text_size / 3 * 2;
        cursor_rect.y = GetScreenHeight() -  cursor_rect.height - (text_size / 6);
        cursor_rect.x = 0;
        if (str != NULL && utf8_len(str) > 0) {
            cursor_rect.x = measure_text_utf8(&renderer, text_size, str).x;
            render_utf8_indep(&renderer, str, text_size, 0, GetScreenHeight() - text_size, WHITE);
        }
        DrawRectangleRec(cursor_rect, update_lerp(&cursor));
        DrawRectangleRounded((Rectangle){GetScreenWidth() - 40, 20, 20, GetScreenHeight() - 70}, 0.9f, 0, (Color){80, 150, 130, 50});
        DrawRectangleRounded((Rectangle){GetScreenWidth() - 35, 50, 10, 70}, 0.9f, 0, (Color){80, 150, 130, 50});
        EndDrawing();

        ENetEvent event = { 0 };
        while (enet_host_service(client.host, &event, 0) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_RECEIVE:
                    {
                        Buffer buffer = create_buffer_with_data(event.packet->data, event.packet->dataLength);
                        Command c = buffer_get(&buffer, Command);
                        if (c == Command_GiveId) {
                            int id = buffer_get(&buffer, int);
                            client.id = id;
                        } else if (c == Command_SendMessage) {
                            int from_id = buffer_get(&buffer, int);
                            utf8_t* from_str = buffer_get_utf8(&buffer);
                            utf8_t* msg_str = buffer_get_utf8(&buffer);
                            printf("<%ls:%d> \"%ls\"\n", from_str, from_id, msg_str);
                            Message msg = {from_str, from_id, msg_str};
                            push_message(&history, msg);
                        }
                    } break;
                default: break;
            }
        }
    }

    //Sending message before we leave
    Buffer buffer = create_buffer();
    buffer_push(&buffer, client.id, int);
    buffer_push(&buffer, Command_Leave, Command);
    enet_peer_send(client.peer, 0, prepare_packet_from_buffer(buffer));
    destroy_buffer(buffer);
    enet_host_service(client.host, NULL, 0);

    CloseWindow();
    deinit_network();

    return 0;
}
