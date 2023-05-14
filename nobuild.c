#define NOBUILD_IMPLEMENTATION
#include "./nobuild.h"

#define COMMON_CFLAGS "-Wall", "-Wextra", "-Wno-gnu-zero-variadic-macro-arguments", "-pedantic", "-std=c11", "-ggdb", "-I./common"
#define CC "clang"
#define CLIENT_SRCS "client/client.c", "client/hsv.c", "client/lerp.c", "client/rendering.c", "client/networking.c"
#define SERVER_SRCS "server/server.c"
#define COMMON_SRCS "common/buffer.c", "common/utf8.c"

void build_client(void) {
    MKDIRS("build");
    CMD(CC, COMMON_CFLAGS, "-o", "./build/joat-client", CLIENT_SRCS, COMMON_SRCS, "-lenet", "-lm", "-lraylib");
}

void build_server(void) {
    MKDIRS("build");
    CMD(CC, COMMON_CFLAGS, "-o", "./build/joat-server", SERVER_SRCS, COMMON_SRCS, "-lenet", "-lm");
}

void usage(const char *program) {
    INFO("Usage: %s [<subcommand>]", program);
    INFO("Subcommands:");
    INFO("    client");
    INFO("        Build client");
    INFO("    server");
    INFO("        Build server");
    INFO("    build");
    INFO("        Build server and client");
}

void run_two_client_and_server(void) {
    CMD("gnome-terminal", "--", "./build/joat-server");
    CMD("gnome-terminal", "--", "./build/joat-client");
    CMD("gnome-terminal", "--", "./build/joat-client");
}

int main(int argc, char **argv) {
    GO_REBUILD_URSELF(argc, argv);

    const char *program = shift_args(&argc, &argv);
    if (argc > 0) {
        const char *subcmd = shift_args(&argc, &argv);
        if (strcmp(subcmd, "client") == 0) {
            build_client();
        } else if (strcmp(subcmd, "server") == 0) {
            build_server();
        } else if (strcmp(subcmd, "build") == 0) {
            build_client();
            build_server();
        } else {
            usage(program);
            PANIC("Unknown command `%s`", subcmd);
        }
    } else {
        build_client();
        build_server();
        run_two_client_and_server();
    }

    return 0;
}
