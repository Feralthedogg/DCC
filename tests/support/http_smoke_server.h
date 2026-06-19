#ifndef DCC_HTTP_SMOKE_SERVER_H
#define DCC_HTTP_SMOKE_SERVER_H

#if !defined(_WIN32)

#include <pthread.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

typedef struct http_server {
    int fd;
    unsigned short port;
    int chunked;
    char method[16];
    char path[512];
    char methods[2][16];
    char paths[2][512];
    char headers[8192];
    char request_headers[2][8192];
    char content_type[256];
    char body[4096];
    char bodies[2][4096];
    size_t body_len;
    size_t body_lens[2];
    unsigned delay_ms;
    unsigned request_count;
    atomic_uint requests_seen;
} http_server_t;

int start_server_mode(http_server_t *server, pthread_t *thread, int chunked);
int start_server(http_server_t *server, pthread_t *thread);
int start_rate_limited_server(http_server_t *server, pthread_t *thread);
int start_bucket_empty_server(http_server_t *server, pthread_t *thread);
int start_direct_message_server(http_server_t *server, pthread_t *thread);
int start_async_retry_queue_server(http_server_t *server, pthread_t *thread);

uint64_t test_now_ms(void);
void set_api_base_for_server(const http_server_t *server);

#endif

#endif
