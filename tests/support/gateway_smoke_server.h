#ifndef DCC_GATEWAY_SMOKE_SERVER_H
#define DCC_GATEWAY_SMOKE_SERVER_H

#if !defined(_WIN32)

#include <pthread.h>

typedef struct gateway_server {
    int fd;
    unsigned short port;
    int saw_identify;
    int saw_heartbeat;
    int saw_resume;
    int saw_voice_state_update;
} gateway_server_t;

int start_server(gateway_server_t *server, pthread_t *thread);
int gateway_server_write_core_dispatches(int client);
int gateway_server_write_extra_dispatches(int client, unsigned seq);

#endif

#endif
