#ifndef DCC_PACKAGE_CONSUMER_SYMBOLS_INTERACTION_SERVER_H
#define DCC_PACKAGE_CONSUMER_SYMBOLS_INTERACTION_SERVER_H

#include <dcc/dcc.h>

typedef dcc_status_t (*dcc_package_interaction_route_add_fn)(dcc_interaction_server_t *, const dcc_interaction_route_t *);
typedef dcc_status_t (*dcc_package_interaction_health_route_fn)(
        dcc_interaction_server_t *,
        const char *
    );
typedef dcc_status_t (*dcc_package_interaction_route_clear_fn)(dcc_interaction_server_t *);
typedef dcc_status_t (*dcc_package_interaction_stats_fn)(
        const dcc_interaction_server_t *,
        dcc_interaction_server_stats_t *
    );
typedef dcc_status_t (*dcc_package_interaction_state_fn)(
        const dcc_interaction_server_t *,
        dcc_interaction_server_state_t *
    );
typedef dcc_status_t (*dcc_package_interaction_health_fn)(
        const dcc_interaction_server_t *,
        dcc_interaction_server_health_snapshot_t *
    );
typedef const char *(*dcc_package_interaction_health_string_fn)(dcc_interaction_server_health_t);
typedef dcc_status_t (*dcc_package_interaction_health_json_fn)(
        const dcc_interaction_server_health_snapshot_t *,
        char *,
        size_t,
        size_t *
    );
typedef dcc_status_t (*dcc_package_interaction_request_reply_type_fn)(
        dcc_interaction_request_t *,
        dcc_interaction_response_type_t
    );
typedef dcc_status_t (*dcc_package_interaction_request_reply_builder_fn)(
        dcc_interaction_request_t *,
        dcc_interaction_response_type_t,
        const dcc_message_builder_t *
    );
typedef dcc_status_t (*dcc_package_interaction_request_reply_message_fn)(
        dcc_interaction_request_t *,
        const dcc_message_builder_t *
    );
typedef dcc_status_t (*dcc_package_interaction_request_reply_text_fn)(
        dcc_interaction_request_t *,
        const char *
    );
typedef dcc_status_t (*dcc_package_interaction_request_reply_modal_fn)(
        dcc_interaction_request_t *,
        const dcc_modal_builder_t *
    );
typedef dcc_status_t (*dcc_package_interaction_request_reply_autocomplete_fn)(
        dcc_interaction_request_t *,
        const dcc_autocomplete_builder_t *
    );
typedef dcc_status_t (*dcc_package_interaction_request_reply_named_fn)(
        dcc_interaction_request_t *
    );

#endif
