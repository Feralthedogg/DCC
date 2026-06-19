#ifndef DCC_GATEWAY_SMOKE_API_SERVER_H
#define DCC_GATEWAY_SMOKE_API_SERVER_H

#if !defined(_WIN32)

#include <pthread.h>

typedef struct api_server {
    int fd;
    unsigned short port;
    unsigned short gateway_port;
    int saw_request;
} api_server_t;

int start_api_server(api_server_t *server, pthread_t *thread, unsigned short gateway_port);

#endif

#endif
