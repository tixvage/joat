#ifndef _NETWORKING_H
#define _NETWORKING_H

#include <enet/enet.h>

typedef struct Client {
    int id;
    char *name;
    ENetHost *host;
    ENetPeer *peer;
} Client;

extern Client client;

void init_network(void);
void deinit_network(void);

#endif
