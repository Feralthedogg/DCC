#ifndef DCC_REST_THREADS_ARCHIVES_H
#define DCC_REST_THREADS_ARCHIVES_H

#include <dcc/rest/types/threads.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_active_threads(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_public_archived_threads(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_public_archived_threads_page(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    uint64_t before_timestamp,
    uint16_t limit,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_private_archived_threads(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_private_archived_threads_page(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    uint64_t before_timestamp,
    uint16_t limit,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_joined_private_archived_threads(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_joined_private_archived_threads_page(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t before_id,
    uint16_t limit,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
