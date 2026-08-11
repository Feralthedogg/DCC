#ifndef DCC_REST_INVITES_H
#define DCC_REST_INVITES_H

#include <dcc/rest/types.h>
#include <dcc/rest/request.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_invite(
    dcc_client_t *client,
    const char *invite_code,
    const dcc_rest_invite_query_t *query,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_delete_invite(
    dcc_client_t *client,
    const char *invite_code,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
