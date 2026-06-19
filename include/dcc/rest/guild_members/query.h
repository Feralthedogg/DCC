#ifndef DCC_REST_GUILD_MEMBERS_QUERY_H
#define DCC_REST_GUILD_MEMBERS_QUERY_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_guild_member(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_list_guild_members(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_list_guild_members_page(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    uint16_t limit,
    dcc_snowflake_t after,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_search_guild_members(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_search_guild_members_page(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *query,
    uint16_t limit,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
