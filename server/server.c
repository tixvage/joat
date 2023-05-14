#include <enet/enet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "utf8.h"
#include "buffer.h"
#include "commands.h"

#define INFO(str, ...) fprintf(stderr, "info:%s:%d: "str"\n", __FILE__, __LINE__, ##__VA_ARGS__)

typedef struct Peer {
    ENetPeer *raw;
    int id;
    utf8_t *name;
    bool living;
} Peer;

typedef struct Peer_Array {
    Peer *data;
    int len;
} Peer_Array;

static Peer_Array peers;

Peer add_peer(ENetPeer *raw) {
    if (peers.data == NULL) {
        INFO("no users, allocating first memory");
        peers.data = calloc(1, sizeof(Peer));
        peers.len = 1;
    }

    int founded = -1;
    for (int i = 0; i < peers.len; i++) {
        if (!peers.data[i].living) {
            INFO("founded available space for new peer, no need to reallocate memory");
            founded = i;
            break;
        }
    }

    if (founded == -1) {
        peers.data = realloc(peers.data, sizeof(Peer) * (peers.len + 1));
        peers.len += 1;
        INFO("new allocation for peers, current_allocated_peer_number = %d", peers.len);
        founded = peers.len - 1;
    }

    peers.data[founded] = (Peer){raw, founded, NULL, true};

    return peers.data[founded];
}

void remove_peer(int id) {
    char *str = utf8_to_cstr(peers.data[id].name);
    INFO("removed peer named `%s` with id `%d`", str, id);
    free(str);
    peers.data[id].living = false;
    peers.data[id].name = NULL;
}

void send_to_every_peer(Buffer buffer) {
    for (int i = 0; i < peers.len; i++) {
        if (!peers.data[i].living) continue;
        enet_peer_send(peers.data[i].raw, 0, prepare_packet_from_buffer(buffer));
    }
}

int main(void) {
    if (enet_initialize () != 0) {
        fprintf (stderr, "An error occurred while initializing enet.\n");
        return 1;
    }

    ENetAddress address = {
        .host = ENET_HOST_ANY,
        .port = 1234,
    };
    ENetHost *server = enet_host_create (&address, 32, 2, 0, 0);
    if (server == NULL) {
        fprintf (stderr, "An error occurred while trying to create an enet server host.\n");
        return 1;
    }

    INFO("server initialized succesfully");

    while (true) {
        ENetEvent event;
        while (enet_host_service(server, &event, 1000) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT: 
                    {
                        INFO("new connection, new id giving");
                        Peer new_peer = add_peer(event.peer);

                        Buffer buffer = create_buffer();
                        buffer_push(&buffer, Command_GiveId, Command);
                        buffer_push(&buffer, new_peer.id, int);
                        enet_peer_send(new_peer.raw, 0, prepare_packet_from_buffer(buffer));

                        destroy_buffer(buffer);
                    } break;
                case ENET_EVENT_TYPE_RECEIVE:
                    {
                        Buffer buffer = create_buffer_with_data(event.packet->data, event.packet->dataLength);
                        int id = buffer_get(&buffer, int);
                        Command c = buffer_get(&buffer, Command);
                        switch (c) {
                            case Command_ChangeUsername:
                                {
                                    utf8_t *str = buffer_get_utf8(&buffer);
                                    char *name_cstr = utf8_to_cstr(peers.data[id].name);
                                    char *newname_cstr = utf8_to_cstr(str);
                                    INFO("user `%s` with id `%d` changing its name to `%s`", name_cstr, id, newname_cstr);
                                    free(name_cstr);
                                    free(newname_cstr);
                                    peers.data[id].name = str;
                                } break;
                            case Command_SendMessage:
                                {
                                    utf8_t *str = buffer_get_utf8(&buffer);
                                    char *name_cstr = utf8_to_cstr(peers.data[id].name);
                                    char *message_cstr = utf8_to_cstr(str);
                                    INFO("<%s:%d> `%s`", name_cstr, id, message_cstr);
                                    free(name_cstr);
                                    free(message_cstr);
                                    buffer = create_buffer();
                                    buffer_push(&buffer, Command_SendMessage, Command);
                                    buffer_push(&buffer, id, int);
                                    buffer_push_utf8(&buffer, peers.data[id].name);
                                    buffer_push_utf8(&buffer, str);
                                    send_to_every_peer(buffer);
                                    destroy_buffer(buffer);
                                } break;
                            case Command_Leave:
                                {
                                    char *str = utf8_to_cstr(peers.data[id].name);
                                    INFO("user `%s` with id `%d` has left", str, id);
                                    free(str);
                                    remove_peer(id);
                                    //update current users info for users
                                } break;
                            default: assert(false);
                        }
                        enet_packet_destroy(event.packet);
                    } break;
                default: break;
            }
        }
    }

    enet_host_destroy(server);
    enet_deinitialize();

    return 0;
}
