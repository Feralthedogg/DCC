#ifndef DCC_PACKAGE_CONSUMER_SYMBOLS_VOICE_H
#define DCC_PACKAGE_CONSUMER_SYMBOLS_VOICE_H

#include <dcc/dcc.h>

typedef dcc_status_t (*dcc_package_gateway_voice_state_fn)(
        dcc_client_t *,
        dcc_snowflake_t,
        dcc_snowflake_t,
        uint8_t,
        uint8_t
    );
typedef dcc_status_t (*dcc_package_voice_connect_fn)(dcc_voice_client_t *, dcc_snowflake_t, dcc_snowflake_t, uint8_t, uint8_t, uint8_t);
typedef dcc_status_t (*dcc_package_voice_leave_fn)(dcc_voice_client_t *);
typedef dcc_status_t (*dcc_package_voice_ws_start_fn)(dcc_voice_client_t *, dcc_snowflake_t, uint32_t);
typedef dcc_status_t (*dcc_package_voice_ws_stop_fn)(dcc_voice_client_t *);
typedef uint8_t (*dcc_package_voice_ws_running_fn)(const dcc_voice_client_t *);
typedef dcc_status_t (*dcc_package_voice_ws_status_fn)(const dcc_voice_client_t *, dcc_status_t *);
typedef dcc_status_t (*dcc_package_voice_stats_fn)(const dcc_voice_client_t *, dcc_voice_client_stats_t *);
typedef dcc_status_t (*dcc_package_voice_health_fn)(const dcc_voice_client_t *, dcc_voice_health_snapshot_t *);
typedef dcc_status_t (*dcc_package_voice_wait_fn)(dcc_voice_client_t *, uint32_t, dcc_voice_health_snapshot_t *);
typedef void (*dcc_package_voice_connect_wait_options_init_fn)(dcc_voice_connect_wait_options_t *);
typedef dcc_status_t (*dcc_package_voice_connect_wait_fn)(dcc_voice_client_t *, const dcc_voice_connect_wait_options_t *, dcc_voice_connect_wait_result_t *);
typedef const char *(*dcc_package_voice_health_string_fn)(dcc_voice_health_t);
typedef const char *(*dcc_package_voice_state_string_fn)(dcc_voice_session_state_t);
typedef dcc_status_t (*dcc_package_voice_health_json_fn)(
        const dcc_voice_health_snapshot_t *,
        char *,
        size_t,
        size_t *
    );
typedef dcc_voice_reconnect_action_t (*dcc_package_voice_close_action_fn)(uint16_t);
typedef dcc_voice_reconnect_action_t (*dcc_package_voice_client_action_fn)(const dcc_voice_client_t *);
typedef const char *(*dcc_package_voice_action_string_fn)(dcc_voice_reconnect_action_t);
typedef dcc_status_t (*dcc_package_voice_dave_handler_fn)(
        dcc_voice_client_t *,
        dcc_voice_dave_mls_fn,
        void *
    );
typedef const char *(*dcc_package_voice_dave_opcode_string_fn)(dcc_voice_dave_mls_opcode_t);
typedef dcc_status_t (*dcc_package_voice_dave_parse_fn)(
        const uint8_t *,
        size_t,
        dcc_voice_dave_mls_frame_t *
    );
typedef dcc_status_t (*dcc_package_voice_dave_gateway_build_fn)(
        uint16_t,
        dcc_voice_dave_mls_opcode_t,
        uint16_t,
        const uint8_t *,
        size_t,
        uint8_t *,
        size_t,
        size_t *
    );
typedef dcc_status_t (*dcc_package_voice_dave_binary_fn)(dcc_voice_client_t *, const uint8_t *, size_t);
typedef dcc_status_t (*dcc_package_voice_dave_build_fn)(uint16_t, char *, size_t, size_t *);
typedef dcc_status_t (*dcc_package_voice_dave_transition_fn)(dcc_voice_client_t *, uint16_t);

#endif
