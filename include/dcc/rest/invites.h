#ifndef DCC_REST_INVITES_H
#define DCC_REST_INVITES_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_invite(
    dcc_client_t *client,
    const char *invite_code,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_invite_full(
    dcc_client_t *client,
    const char *invite_code,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_delete_invite(
    dcc_client_t *client,
    const char *invite_code,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
