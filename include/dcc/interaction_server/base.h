#ifndef DCC_INTERACTION_SERVER_BASE_H
#define DCC_INTERACTION_SERVER_BASE_H

#include <dcc/error.h>
#include <dcc/events.h>
#include <dcc/export.h>
#include <dcc/objects.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_interaction_server dcc_interaction_server_t;
typedef struct dcc_interaction_request dcc_interaction_request_t;

typedef struct dcc_interaction_http_header {
    const char *name;
    const char *value;
} dcc_interaction_http_header_t;

typedef void (*dcc_interaction_server_cb)(
    dcc_interaction_server_t *server,
    dcc_interaction_request_t *request,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
