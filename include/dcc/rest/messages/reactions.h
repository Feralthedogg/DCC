#ifndef DCC_REST_MESSAGES_REACTIONS_H
#define DCC_REST_MESSAGES_REACTIONS_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Submits `PUT .../reactions/{emoji}/@me`.
 * @details `reaction` is borrowed for the call. Uses the
 * @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_add_message_reaction(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

/**
 * @brief Submits `DELETE .../reactions/{emoji}/@me`.
 * @details Uses the @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_delete_own_message_reaction(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

/**
 * @brief Submits `DELETE .../reactions/{emoji}/{user_id}`.
 * @details Uses the @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_delete_user_message_reaction(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    dcc_snowflake_t user_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

/**
 * @brief Submits `GET .../reactions/{emoji}`.
 * @details `query` may be NULL. Uses the @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_get_message_reactions(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    const dcc_rest_reaction_query_t *query,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

/**
 * @brief Submits `DELETE /channels/{channel_id}/messages/{message_id}/reactions`.
 * @details Uses the @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_delete_all_message_reactions(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

/**
 * @brief Submits `DELETE .../reactions/{emoji}`.
 * @details Uses the @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_delete_all_message_reactions_for_emoji(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
