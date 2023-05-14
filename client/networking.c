#include "networking.h"

#include <stdlib.h>
#include <stdio.h>

Client client = { 0 };

void init_network(void) {
    if (enet_initialize () != 0) {
        fprintf(stderr, "An error occurred while initializing enet.\n");
        exit(1);
    }

    ENetHost *host = enet_host_create(NULL, 1, 2, 0, 0);
    if (host == NULL) {
        fprintf(stderr, "An error occurred while trying to create an ENet client host.\n");
        exit(1);
    }

    ENetAddress address;
    enet_address_set_host(&address, "localhost");
    address.port = 1234;
    ENetPeer *peer = enet_host_connect(host, &address, 2, 0);    
    if (peer == NULL) {
        fprintf(stderr, "No available peers for initiating an ENet connection.\n");
        exit(1);
    }

    client.host = host;
    client.peer = peer;
}

void deinit_network(void) {
    enet_host_destroy(client.host);
    enet_deinitialize();
}
