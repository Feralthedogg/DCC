#include <dcc/app.h>
#include <dcc/client.h>
#include <dcc/error_details.h>
#include <dcc/rest.h>

#include "internal/app/dcc_app_internal.h"
#include "internal/rest/dcc_rest_error_observer_internal.h"
#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "rest_v2_endpoint_smoke_support.h"
#include "rest_v2_task7_preparation_probe.h"
#include "rest_v2_task7_sensitive_probe.h"

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TASK7_SENSITIVE_PATH_MATERIALS \
    (DCC_ENDPOINT_SENSITIVE_MATERIAL_ESCAPED_TOKEN | \
     DCC_ENDPOINT_SENSITIVE_MATERIAL_WIRE_PATH | \
     DCC_ENDPOINT_SENSITIVE_MATERIAL_ROUTE_DIGEST)
#define TASK7_SENSITIVE_AUTH_MATERIALS \
    (DCC_ENDPOINT_SENSITIVE_MATERIAL_BEARER_TOKEN | \
     DCC_ENDPOINT_SENSITIVE_MATERIAL_AUTHORIZATION)

#define DCC_EXPECT_SIGNATURE(name_, ...) \
    typedef dcc_status_t (*name_##_task7_signature_t)(__VA_ARGS__); \
    _Static_assert( \
        _Generic(&(name_), name_##_task7_signature_t: 1, default: 0), \
        #name_ " does not match the exact Task 7 request signature" \
    )

DCC_EXPECT_SIGNATURE(dcc_rest_add_thread_member, dcc_client_t *client, dcc_snowflake_t thread_id, dcc_snowflake_t user_id, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_create_channel_invite, dcc_client_t *client, dcc_snowflake_t channel_id, const dcc_invite_params_t *params, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_create_dm_channel, dcc_client_t *client, const dcc_dm_channel_params_t *params, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_create_forum_thread, dcc_client_t *client, dcc_snowflake_t channel_id, const dcc_thread_params_t *params, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_create_guild_channel, dcc_client_t *client, dcc_snowflake_t guild_id, const dcc_channel_params_t *params, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_create_lobby_channel_invite_for_self, dcc_client_t *client, dcc_snowflake_t lobby_id, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_create_lobby_channel_invite_for_user, dcc_client_t *client, dcc_snowflake_t lobby_id, dcc_snowflake_t user_id, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_create_thread, dcc_client_t *client, dcc_snowflake_t channel_id, const dcc_thread_params_t *params, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_create_thread_from_message, dcc_client_t *client, dcc_snowflake_t channel_id, dcc_snowflake_t message_id, const dcc_thread_params_t *params, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_delete_channel, dcc_client_t *client, dcc_snowflake_t channel_id, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_delete_channel_permission, dcc_client_t *client, dcc_snowflake_t channel_id, dcc_snowflake_t overwrite_id, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_delete_invite, dcc_client_t *client, const char *invite_code, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_follow_news_channel, dcc_client_t *client, dcc_snowflake_t channel_id, const dcc_rest_follow_news_channel_t *follow, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_get_active_threads, dcc_client_t *client, dcc_snowflake_t guild_id, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_get_channel, dcc_client_t *client, dcc_snowflake_t channel_id, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_get_channel_invites, dcc_client_t *client, dcc_snowflake_t channel_id, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_get_guild_channels, dcc_client_t *client, dcc_snowflake_t guild_id, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_get_guild_invites, dcc_client_t *client, dcc_snowflake_t guild_id, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_get_invite, dcc_client_t *client, const char *invite_code, const dcc_rest_invite_query_t *query, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_get_invite_target_users, dcc_client_t *client, const char *invite_code, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_get_invite_target_users_job_status, dcc_client_t *client, const char *invite_code, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_get_joined_private_archived_threads, dcc_client_t *client, dcc_snowflake_t channel_id, const dcc_rest_joined_thread_archive_query_t *query, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_get_private_archived_threads, dcc_client_t *client, dcc_snowflake_t channel_id, const dcc_rest_thread_archive_query_t *query, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_get_public_archived_threads, dcc_client_t *client, dcc_snowflake_t channel_id, const dcc_rest_thread_archive_query_t *query, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_get_thread_member, dcc_client_t *client, dcc_snowflake_t thread_id, dcc_snowflake_t user_id, const dcc_rest_thread_member_query_t *query, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_get_thread_members, dcc_client_t *client, dcc_snowflake_t thread_id, const dcc_rest_thread_member_query_t *query, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_join_thread, dcc_client_t *client, dcc_snowflake_t thread_id, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_leave_thread, dcc_client_t *client, dcc_snowflake_t thread_id, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_modify_channel, dcc_client_t *client, dcc_snowflake_t channel_id, const dcc_channel_params_t *params, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_modify_channel_permission, dcc_client_t *client, dcc_snowflake_t channel_id, dcc_snowflake_t overwrite_id, const dcc_channel_permission_overwrite_t *overwrite, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_modify_guild_channel_positions, dcc_client_t *client, dcc_snowflake_t guild_id, const dcc_channel_positions_params_t *params, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_put_invite_target_users, dcc_client_t *client, const char *invite_code, const dcc_rest_invite_target_users_upload_t *upload, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_remove_thread_member, dcc_client_t *client, dcc_snowflake_t thread_id, dcc_snowflake_t user_id, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_set_channel_voice_status, dcc_client_t *client, dcc_snowflake_t channel_id, const dcc_channel_voice_status_params_t *params, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);
DCC_EXPECT_SIGNATURE(dcc_rest_trigger_channel_typing, dcc_client_t *client, dcc_snowflake_t channel_id, const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

typedef struct task7_historical_call_options {
    size_t size;
    uint32_t version;
    dcc_rest_priority_t priority;
    dcc_rest_result_fn callback;
    void *user_data;
} task7_historical_call_options_t;

_Static_assert(DCC_REST_CALL_OPTIONS_VERSION == 1U, "Task 7 keeps call options at version 1");
_Static_assert(DCC_REST_AUTH_DEFAULT == 0, "DEFAULT must remain the zero auth mode");
_Static_assert(DCC_REST_AUTH_NONE == 1, "NONE auth mode ordinal changed");
_Static_assert(DCC_REST_AUTH_BOT == 2, "BOT auth mode ordinal changed");
_Static_assert(DCC_REST_AUTH_BEARER == 3, "BEARER auth mode ordinal changed");
_Static_assert(
    offsetof(dcc_rest_call_options_t, audit_log_reason) ==
        sizeof(task7_historical_call_options_t),
    "call-options tail no longer follows the exact historical prefix"
);
_Static_assert(
    offsetof(dcc_rest_call_options_t, auth_mode) >
        offsetof(dcc_rest_call_options_t, audit_log_reason),
    "auth mode must follow audit reason"
);
_Static_assert(
    offsetof(dcc_rest_call_options_t, auth_token) >
        offsetof(dcc_rest_call_options_t, auth_mode),
    "auth token must follow auth mode"
);
_Static_assert(
    offsetof(dcc_rest_call_options_t, flags) >
        offsetof(dcc_rest_call_options_t, auth_token),
    "flags must be the final Task 7 call-options field"
);
_Static_assert(
    _Generic(
        ((dcc_rest_request_desc_t *)0)->options,
        const dcc_rest_call_options_t *: 1,
        default: 0
    ),
    "raw descriptor options must be a pointer to independent call options"
);
_Static_assert(
    offsetof(dcc_rest_request_desc_t, options) >
        offsetof(dcc_rest_request_desc_t, body_len),
    "raw descriptor options pointer must be its final field"
);

#define TASK7_EXPECT_CALL(label_, capture_, call_, method_, path_) \
    do { \
        dcc_rest_request_t *request = NULL; \
        endpoint_capture_reset((capture_), 0U); \
        atomic_store_explicit( \
            &(capture_)->capture_release, 1U, memory_order_release \
        ); \
        dcc_status_t submit_status = (call_); \
        if (endpoint_completed_contract( \
                (label_), (capture_), submit_status, request, \
                (method_), (path_), NULL, NULL, 0U \
            ) != 0) { \
            return 1; \
        } \
    } while (0)

#define TASK7_EXPECT_LOCAL_REJECTION( \
    label_, capture_, callback_, observer_, call_ \
) \
    do { \
        unsigned capture_before = atomic_load_explicit( \
            &(capture_)->calls, memory_order_acquire \
        ); \
        unsigned callback_before = atomic_load_explicit( \
            &(callback_)->calls, memory_order_acquire \
        ); \
        unsigned observer_before = atomic_load_explicit( \
            &(observer_)->calls, memory_order_acquire \
        ); \
        dcc_rest_request_t *request = (dcc_rest_request_t *)(uintptr_t)1U; \
        dcc_endpoint_test_allocation_probe_begin(0U); \
        dcc_status_t rejected = (call_); \
        size_t allocations = dcc_endpoint_test_allocation_probe_end(); \
        if (rejected != DCC_ERR_INVALID_ARG || request != NULL || \
            allocations != 0U || \
            atomic_load_explicit( \
                &(capture_)->calls, memory_order_acquire \
            ) != capture_before || \
            atomic_load_explicit( \
                &(callback_)->calls, memory_order_acquire \
            ) != callback_before || \
            atomic_load_explicit( \
                &(observer_)->calls, memory_order_acquire \
            ) != observer_before) { \
            fprintf( \
                stderr, \
                "%s local rejection status=%s request=%p allocations=%zu " \
                "capture=%u/%u callback=%u/%u observer=%u/%u\n", \
                (label_), dcc_status_string(rejected), (void *)request, \
                allocations, capture_before, atomic_load_explicit( \
                    &(capture_)->calls, memory_order_acquire \
                ), callback_before, atomic_load_explicit( \
                    &(callback_)->calls, memory_order_acquire \
                ), observer_before, atomic_load_explicit( \
                    &(observer_)->calls, memory_order_acquire \
                ) \
            ); \
            dcc_rest_request_destroy(request); \
            return 1; \
        } \
    } while (0)

#define TASK7_EXPECT_PREPARED_CALL( \
    label_, capture_, checks_, operation_, audit_, authorization_, \
    call_, method_, path_ \
) \
    do { \
        dcc_endpoint_test_preparation_probe_begin( \
            (checks_), (operation_), (audit_), (authorization_) \
        ); \
        TASK7_EXPECT_CALL( \
            (label_), (capture_), (call_), (method_), (path_) \
        ); \
        dcc_endpoint_preparation_probe_snapshot_t preparation = \
            DCC_ENDPOINT_PREPARATION_PROBE_SNAPSHOT_INIT; \
        dcc_status_t preparation_status = \
            dcc_endpoint_test_preparation_probe_snapshot(&preparation); \
        dcc_endpoint_test_preparation_probe_end(); \
        if (preparation_status != DCC_OK || preparation.preparations != 1U || \
            (((checks_) & DCC_ENDPOINT_PREPARATION_CHECK_OPERATION) != 0U && \
                preparation.operation_matches != 1U) || \
            (((checks_) & DCC_ENDPOINT_PREPARATION_CHECK_AUDIT_HEADER) != 0U && \
                preparation.audit_header_matches != 1U) || \
            (((checks_) & DCC_ENDPOINT_PREPARATION_CHECK_AUTHORIZATION) != 0U && \
                preparation.authorization_matches != 1U)) { \
            fprintf( \
                stderr, \
                "%s prepared request mismatch status=%s count=%u " \
                "operation=%u audit=%u auth=%u\n", \
                (label_), dcc_status_string(preparation_status), \
                preparation.preparations, preparation.operation_matches, \
                preparation.audit_header_matches, \
                preparation.authorization_matches \
            ); \
            return 1; \
        } \
    } while (0)

static int task7_options_layout_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture
) {
    dcc_rest_call_options_t initialized = DCC_REST_CALL_OPTIONS_INIT;
    if (initialized.size != sizeof(initialized) ||
        initialized.version != DCC_REST_CALL_OPTIONS_VERSION ||
        initialized.priority != DCC_REST_PRIORITY_NORMAL ||
        initialized.callback != NULL || initialized.user_data != NULL ||
        initialized.audit_log_reason != NULL ||
        initialized.auth_mode != DCC_REST_AUTH_DEFAULT ||
        initialized.auth_token != NULL || initialized.flags != 0U) {
        fprintf(stderr, "Task 7 call-options initializer is incomplete\n");
        return 1;
    }

    union {
        max_align_t alignment;
        unsigned char bytes[sizeof(dcc_rest_call_options_t) + 32U];
    } storage;
    memset(storage.bytes, 0xa5, sizeof(storage.bytes));
    task7_historical_call_options_t *historical =
        (task7_historical_call_options_t *)storage.bytes;
    historical->size = sizeof(*historical);
    historical->version = DCC_REST_CALL_OPTIONS_VERSION;
    historical->priority = DCC_REST_PRIORITY_NORMAL;
    historical->callback = NULL;
    historical->user_data = NULL;
    TASK7_EXPECT_CALL(
        "historical call-options prefix",
        capture,
        dcc_rest_get_channel(
            client,
            UINT64_C(42),
            (const dcc_rest_call_options_t *)historical,
            &request
        ),
        "GET",
        "/channels/42"
    );

    const size_t partial_sizes[] = {
        offsetof(dcc_rest_call_options_t, audit_log_reason) + 1U,
        offsetof(dcc_rest_call_options_t, auth_mode) + 1U,
        offsetof(dcc_rest_call_options_t, auth_token) + 1U,
        offsetof(dcc_rest_call_options_t, flags) + 1U,
    };
    for (size_t index = 0U; index < sizeof(partial_sizes) / sizeof(partial_sizes[0]); ++index) {
        dcc_rest_call_options_t partial = DCC_REST_CALL_OPTIONS_INIT;
        partial.size = partial_sizes[index];
        dcc_rest_request_t *request = (dcc_rest_request_t *)(uintptr_t)1U;
        if (dcc_rest_get_channel(client, 42U, &partial, &request) !=
                DCC_ERR_INVALID_ARG || request != NULL) {
            fprintf(stderr, "partial call-options tail %zu was accepted\n", index);
            dcc_rest_request_destroy(request);
            return 1;
        }
    }

    struct {
        dcc_rest_call_options_t options;
        uint64_t future[2];
    } larger = { DCC_REST_CALL_OPTIONS_INIT, {UINT64_MAX, UINT64_MAX} };
    larger.options.size = sizeof(larger);
    TASK7_EXPECT_CALL(
        "larger call-options layout",
        capture,
        dcc_rest_get_channel(client, 43U, &larger.options, &request),
        "GET",
        "/channels/43"
    );

    dcc_rest_request_desc_t nested = DCC_REST_REQUEST_DESC_INIT;
    nested.path = "/channels/44";
    nested.options = (const dcc_rest_call_options_t *)historical;
    TASK7_EXPECT_CALL(
        "raw descriptor with independent historical options",
        capture,
        dcc_rest_submit(client, &nested, &request),
        "GET",
        "/channels/44"
    );

    dcc_rest_call_options_t raw_options = DCC_REST_CALL_OPTIONS_INIT;
    nested.path = "/channels/45";
    nested.options = &raw_options;
    nested.size = offsetof(dcc_rest_request_desc_t, options) + 1U;
    dcc_rest_request_t *request = (dcc_rest_request_t *)(uintptr_t)1U;
    if (dcc_rest_submit(client, &nested, &request) != DCC_ERR_INVALID_ARG ||
        request != NULL) {
        fprintf(stderr, "partial raw options pointer field was accepted\n");
        dcc_rest_request_destroy(request);
        return 1;
    }
    return 0;
}

static int task7_auth_policy_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_callback_t *callback,
    endpoint_observer_t *observer
) {
    endpoint_callback_reset(callback);
    dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;

    TASK7_EXPECT_CALL(
        "Bot endpoint default auth",
        capture,
        dcc_rest_get_channel(client, 90U, NULL, &request),
        "GET",
        "/channels/90"
    );
    options.auth_mode = DCC_REST_AUTH_BOT;
    TASK7_EXPECT_CALL(
        "Bot endpoint explicit auth",
        capture,
        dcc_rest_get_channel(client, 91U, &options, &request),
        "GET",
        "/channels/91"
    );
    options.callback = endpoint_result_callback;
    options.user_data = callback;
    options.auth_mode = DCC_REST_AUTH_NONE;
    TASK7_EXPECT_LOCAL_REJECTION(
        "Bot endpoint rejects None",
        capture,
        callback,
        observer,
        dcc_rest_get_channel(client, 92U, &options, &request)
    );
    options.auth_mode = DCC_REST_AUTH_BEARER;
    options.auth_token = "bearer";
    TASK7_EXPECT_LOCAL_REJECTION(
        "Bot endpoint rejects Bearer",
        capture,
        callback,
        observer,
        dcc_rest_get_channel(client, 92U, &options, &request)
    );

    options = (dcc_rest_call_options_t)DCC_REST_CALL_OPTIONS_INIT;
    options.callback = endpoint_result_callback;
    options.user_data = callback;
    TASK7_EXPECT_LOCAL_REJECTION(
        "self lobby rejects inferred default",
        capture,
        callback,
        observer,
        dcc_rest_create_lobby_channel_invite_for_self(
            client, 93U, &options, &request
        )
    );
    options.auth_mode = DCC_REST_AUTH_NONE;
    TASK7_EXPECT_LOCAL_REJECTION(
        "self lobby rejects None",
        capture,
        callback,
        observer,
        dcc_rest_create_lobby_channel_invite_for_self(
            client, 93U, &options, &request
        )
    );
    options.auth_mode = DCC_REST_AUTH_BOT;
    TASK7_EXPECT_LOCAL_REJECTION(
        "self lobby rejects Bot",
        capture,
        callback,
        observer,
        dcc_rest_create_lobby_channel_invite_for_self(
            client, 93U, &options, &request
        )
    );
    options.auth_mode = DCC_REST_AUTH_BEARER;
    options.auth_token = NULL;
    TASK7_EXPECT_LOCAL_REJECTION(
        "self lobby rejects missing Bearer token",
        capture,
        callback,
        observer,
        dcc_rest_create_lobby_channel_invite_for_self(
            client, 93U, &options, &request
        )
    );
    options.auth_token = "";
    TASK7_EXPECT_LOCAL_REJECTION(
        "self lobby rejects empty Bearer token",
        capture,
        callback,
        observer,
        dcc_rest_create_lobby_channel_invite_for_self(
            client, 93U, &options, &request
        )
    );
    options.auth_token = "bearer";
    options.callback = NULL;
    options.user_data = NULL;
    TASK7_EXPECT_CALL(
        "self lobby accepts explicit Bearer",
        capture,
        dcc_rest_create_lobby_channel_invite_for_self(
            client, 93U, &options, &request
        ),
        "POST",
        "/lobbies/93/members/@me/invites"
    );
    options.callback = endpoint_result_callback;
    options.user_data = callback;
    options.size = offsetof(dcc_rest_call_options_t, auth_token);
    TASK7_EXPECT_LOCAL_REJECTION(
        "self lobby rejects uncovered Bearer token",
        capture,
        callback,
        observer,
        dcc_rest_create_lobby_channel_invite_for_self(
            client, 93U, &options, &request
        )
    );

    options = (dcc_rest_call_options_t)DCC_REST_CALL_OPTIONS_INIT;
    TASK7_EXPECT_CALL(
        "user lobby default Bot auth",
        capture,
        dcc_rest_create_lobby_channel_invite_for_user(
            client, 94U, 95U, &options, &request
        ),
        "POST",
        "/lobbies/94/members/95/invites"
    );
    options.auth_mode = DCC_REST_AUTH_BOT;
    TASK7_EXPECT_CALL(
        "user lobby explicit Bot auth",
        capture,
        dcc_rest_create_lobby_channel_invite_for_user(
            client, 94U, 95U, &options, &request
        ),
        "POST",
        "/lobbies/94/members/95/invites"
    );
    options.callback = endpoint_result_callback;
    options.user_data = callback;
    options.auth_mode = DCC_REST_AUTH_BEARER;
    options.auth_token = "bearer";
    TASK7_EXPECT_LOCAL_REJECTION(
        "user lobby rejects Bearer",
        capture,
        callback,
        observer,
        dcc_rest_create_lobby_channel_invite_for_user(
            client, 94U, 95U, &options, &request
        )
    );
    options.auth_mode = DCC_REST_AUTH_NONE;
    options.auth_token = NULL;
    TASK7_EXPECT_LOCAL_REJECTION(
        "user lobby rejects None",
        capture,
        callback,
        observer,
        dcc_rest_create_lobby_channel_invite_for_user(
            client, 94U, 95U, &options, &request
        )
    );

    options = (dcc_rest_call_options_t)DCC_REST_CALL_OPTIONS_INIT;
    TASK7_EXPECT_CALL(
        "Get Invite defaults to None",
        capture,
        dcc_rest_get_invite(client, "auth", NULL, &options, &request),
        "GET",
        "/invites/auth"
    );
    options.auth_mode = DCC_REST_AUTH_NONE;
    TASK7_EXPECT_CALL(
        "Get Invite explicit None",
        capture,
        dcc_rest_get_invite(client, "auth", NULL, &options, &request),
        "GET",
        "/invites/auth"
    );
    options.auth_mode = DCC_REST_AUTH_BOT;
    TASK7_EXPECT_CALL(
        "Get Invite explicit Bot",
        capture,
        dcc_rest_get_invite(client, "auth", NULL, &options, &request),
        "GET",
        "/invites/auth"
    );
    options.callback = endpoint_result_callback;
    options.user_data = callback;
    options.auth_mode = DCC_REST_AUTH_BEARER;
    options.auth_token = "bearer";
    TASK7_EXPECT_LOCAL_REJECTION(
        "Get Invite rejects Bearer",
        capture,
        callback,
        observer,
        dcc_rest_get_invite(client, "auth", NULL, &options, &request)
    );

    const dcc_rest_auth_mode_t non_bearer_modes[] = {
        DCC_REST_AUTH_DEFAULT,
        DCC_REST_AUTH_NONE,
        DCC_REST_AUTH_BOT,
    };
    for (size_t index = 0U;
         index < sizeof(non_bearer_modes) / sizeof(non_bearer_modes[0]);
         ++index) {
        options = (dcc_rest_call_options_t)DCC_REST_CALL_OPTIONS_INIT;
        options.callback = endpoint_result_callback;
        options.user_data = callback;
        options.auth_mode = non_bearer_modes[index];
        options.auth_token = "orphan-token";
        TASK7_EXPECT_LOCAL_REJECTION(
            "non-Bearer mode rejects auth_token",
            capture,
            callback,
            observer,
            dcc_rest_get_channel(client, 96U, &options, &request)
        );
    }
    options = (dcc_rest_call_options_t)DCC_REST_CALL_OPTIONS_INIT;
    options.callback = endpoint_result_callback;
    options.user_data = callback;
    options.auth_mode = (dcc_rest_auth_mode_t)99;
    TASK7_EXPECT_LOCAL_REJECTION(
        "unknown auth mode",
        capture,
        callback,
        observer,
        dcc_rest_get_channel(client, 96U, &options, &request)
    );

    dcc_rest_request_desc_t raw = DCC_REST_REQUEST_DESC_INIT;
    raw.path = "/task7/raw-auth";
    TASK7_EXPECT_CALL(
        "relative raw default Bot",
        capture,
        dcc_rest_submit(client, &raw, &request),
        "GET",
        "/task7/raw-auth"
    );
    options = (dcc_rest_call_options_t)DCC_REST_CALL_OPTIONS_INIT;
    options.auth_mode = DCC_REST_AUTH_NONE;
    raw.options = &options;
    TASK7_EXPECT_CALL(
        "relative raw None",
        capture,
        dcc_rest_submit(client, &raw, &request),
        "GET",
        "/task7/raw-auth"
    );
    options.auth_mode = DCC_REST_AUTH_BEARER;
    options.auth_token = "raw-bearer";
    TASK7_EXPECT_CALL(
        "relative raw Bearer",
        capture,
        dcc_rest_submit(client, &raw, &request),
        "GET",
        "/task7/raw-auth"
    );

    raw = (dcc_rest_request_desc_t)DCC_REST_REQUEST_DESC_INIT;
    raw.path = "https://example.invalid/task7";
    TASK7_EXPECT_PREPARED_CALL(
        "absolute raw empty Discord options",
        capture,
        DCC_ENDPOINT_PREPARATION_CHECK_OPERATION |
            DCC_ENDPOINT_PREPARATION_CHECK_AUDIT_HEADER |
            DCC_ENDPOINT_PREPARATION_CHECK_AUTHORIZATION,
        "dcc_rest_submit",
        NULL,
        NULL,
        dcc_rest_submit(client, &raw, &request),
        "GET",
        "https://example.invalid/task7"
    );
    options = (dcc_rest_call_options_t)DCC_REST_CALL_OPTIONS_INIT;
    options.callback = endpoint_result_callback;
    options.user_data = callback;
    options.auth_mode = DCC_REST_AUTH_NONE;
    raw.options = &options;
    TASK7_EXPECT_LOCAL_REJECTION(
        "absolute raw rejects auth override",
        capture,
        callback,
        observer,
        dcc_rest_submit(client, &raw, &request)
    );
    options.auth_mode = DCC_REST_AUTH_DEFAULT;
    options.audit_log_reason = "reason";
    TASK7_EXPECT_LOCAL_REJECTION(
        "absolute raw rejects audit reason",
        capture,
        callback,
        observer,
        dcc_rest_submit(client, &raw, &request)
    );
    return 0;
}

static int task7_audit_reason_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_callback_t *callback,
    endpoint_observer_t *observer
) {
    static const uint32_t audit_checks =
        DCC_ENDPOINT_PREPARATION_CHECK_OPERATION |
        DCC_ENDPOINT_PREPARATION_CHECK_AUDIT_HEADER;
    static const char reason[] = "R&D 100% \xed\x95\x9c\xea\xb8\x80\r\n";
    static const char encoded_reason[] =
        "R%26D%20100%25%20%ED%95%9C%EA%B8%80%0D%0A";
    dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
    options.audit_log_reason = reason;
    TASK7_EXPECT_PREPARED_CALL(
        "Delete Channel Unicode audit reason",
        capture,
        audit_checks,
        "dcc_rest_delete_channel",
        encoded_reason,
        NULL,
        dcc_rest_delete_channel(client, 101U, &options, &request),
        "DELETE",
        "/channels/101"
    );
    TASK7_EXPECT_PREPARED_CALL(
        "Delete Channel Permission Unicode audit reason",
        capture,
        audit_checks,
        "dcc_rest_delete_channel_permission",
        encoded_reason,
        NULL,
        dcc_rest_delete_channel_permission(
            client, 101U, 102U, &options, &request
        ),
        "DELETE",
        "/channels/101/permissions/102"
    );
    TASK7_EXPECT_PREPARED_CALL(
        "Delete Invite Unicode audit reason",
        capture,
        audit_checks,
        "dcc_rest_delete_invite",
        encoded_reason,
        NULL,
        dcc_rest_delete_invite(client, "audit/code", &options, &request),
        "DELETE",
        "/invites/audit%2Fcode"
    );

    dcc_rest_request_desc_t raw = DCC_REST_REQUEST_DESC_INIT;
    raw.path = "/task7/audit";
    options.audit_log_reason = "100% done";
    raw.options = &options;
    TASK7_EXPECT_PREPARED_CALL(
        "raw audit percent encoded once",
        capture,
        audit_checks,
        "dcc_rest_submit",
        "100%25%20done",
        NULL,
        dcc_rest_submit(client, &raw, &request),
        "GET",
        "/task7/audit"
    );

    char boundary_512[513];
    memset(boundary_512, 'a', sizeof(boundary_512) - 1U);
    boundary_512[sizeof(boundary_512) - 1U] = '\0';
    options.audit_log_reason = boundary_512;
    TASK7_EXPECT_PREPARED_CALL(
        "audit reason 512 scalar boundary",
        capture,
        audit_checks,
        "dcc_rest_submit",
        boundary_512,
        NULL,
        dcc_rest_submit(client, &raw, &request),
        "GET",
        "/task7/audit"
    );

    endpoint_callback_reset(callback);
    options = (dcc_rest_call_options_t)DCC_REST_CALL_OPTIONS_INIT;
    options.callback = endpoint_result_callback;
    options.user_data = callback;
    options.audit_log_reason = "";
    TASK7_EXPECT_LOCAL_REJECTION(
        "empty audit reason",
        capture,
        callback,
        observer,
        dcc_rest_delete_channel(client, 101U, &options, &request)
    );
    options.audit_log_reason = "\xc0\xaf";
    TASK7_EXPECT_LOCAL_REJECTION(
        "noncanonical UTF-8 audit reason",
        capture,
        callback,
        observer,
        dcc_rest_delete_channel(client, 101U, &options, &request)
    );
    char boundary_513[514];
    memset(boundary_513, 'b', sizeof(boundary_513) - 1U);
    boundary_513[sizeof(boundary_513) - 1U] = '\0';
    options.audit_log_reason = boundary_513;
    TASK7_EXPECT_LOCAL_REJECTION(
        "audit reason 513 scalar rejection",
        capture,
        callback,
        observer,
        dcc_rest_delete_channel(client, 101U, &options, &request)
    );
    options.audit_log_reason = "not-capable";
    TASK7_EXPECT_LOCAL_REJECTION(
        "non-audit endpoint rejects reason",
        capture,
        callback,
        observer,
        dcc_rest_get_channel(client, 101U, &options, &request)
    );
    return 0;
}

static int task7_literal_route_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture
) {
    TASK7_EXPECT_CALL(
        "GET channel without body",
        capture,
        dcc_rest_get_channel(client, UINT64_C(9001), NULL, &request),
        "GET",
        "/channels/9001"
    );

    dcc_rest_thread_archive_query_t archive =
        DCC_REST_THREAD_ARCHIVE_QUERY_INIT;
    archive.present = DCC_REST_THREAD_ARCHIVE_QUERY_PRESENT_BEFORE |
        DCC_REST_THREAD_ARCHIVE_QUERY_PRESENT_LIMIT;
    archive.before = "2026-08-11T10:20:30+09:00";
    archive.limit = 25U;
    TASK7_EXPECT_CALL(
        "public archive query encoding",
        capture,
        dcc_rest_get_public_archived_threads(
            client, 17U, &archive, NULL, &request
        ),
        "GET",
        "/channels/17/threads/archived/public?before=2026-08-11T10%3A20%3A30%2B09%3A00&limit=25"
    );

    dcc_rest_joined_thread_archive_query_t joined =
        DCC_REST_JOINED_THREAD_ARCHIVE_QUERY_INIT;
    joined.present = DCC_REST_JOINED_THREAD_ARCHIVE_QUERY_PRESENT_BEFORE |
        DCC_REST_JOINED_THREAD_ARCHIVE_QUERY_PRESENT_LIMIT;
    joined.before = UINT64_C(123456789);
    joined.limit = 7U;
    TASK7_EXPECT_CALL(
        "joined archive snowflake query",
        capture,
        dcc_rest_get_joined_private_archived_threads(
            client, 18U, &joined, NULL, &request
        ),
        "GET",
        "/channels/18/users/@me/threads/archived/private?before=123456789&limit=7"
    );

    dcc_rest_thread_member_query_t member = DCC_REST_THREAD_MEMBER_QUERY_INIT;
    member.present = DCC_REST_THREAD_MEMBER_QUERY_PRESENT_WITH_MEMBER;
    member.with_member = 0U;
    TASK7_EXPECT_CALL(
        "single thread member explicit false",
        capture,
        dcc_rest_get_thread_member(
            client, 19U, 20U, &member, NULL, &request
        ),
        "GET",
        "/channels/19/thread-members/20?with_member=false"
    );

    member.present = DCC_REST_THREAD_MEMBER_QUERY_PRESENT_WITH_MEMBER |
        DCC_REST_THREAD_MEMBER_QUERY_PRESENT_AFTER |
        DCC_REST_THREAD_MEMBER_QUERY_PRESENT_LIMIT;
    member.with_member = 1U;
    member.after = UINT64_C(21);
    member.limit = 100U;
    TASK7_EXPECT_CALL(
        "thread member list query order",
        capture,
        dcc_rest_get_thread_members(client, 19U, &member, NULL, &request),
        "GET",
        "/channels/19/thread-members?with_member=true&after=21&limit=100"
    );

    dcc_rest_invite_query_t invite = DCC_REST_INVITE_QUERY_INIT;
    invite.present = DCC_REST_INVITE_QUERY_PRESENT_WITH_COUNTS |
        DCC_REST_INVITE_QUERY_PRESENT_GUILD_SCHEDULED_EVENT_ID;
    invite.with_counts = 0U;
    invite.guild_scheduled_event_id = UINT64_C(22);
    TASK7_EXPECT_CALL(
        "invite segment and query encoding",
        capture,
        dcc_rest_get_invite(
            client, "a/b%z", &invite, NULL, &request
        ),
        "GET",
        "/invites/a%2Fb%25z?with_counts=false&guild_scheduled_event_id=22"
    );
    return 0;
}

typedef struct task7_extended_position {
    dcc_channel_position_t value;
    uint64_t future;
} task7_extended_position_t;

typedef struct task7_app_callback {
    atomic_uint calls;
} task7_app_callback_t;

static void task7_app_rest_callback(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    task7_app_callback_t *state = (task7_app_callback_t *)user_data;
    (void)client;
    (void)response;
    atomic_fetch_add_explicit(&state->calls, 1U, memory_order_acq_rel);
}

static int task7_record_validation_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_callback_t *callback,
    endpoint_observer_t *observer
) {
    endpoint_callback_reset(callback);
    dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
    options.callback = endpoint_result_callback;
    options.user_data = callback;

    dcc_invite_params_t invite = DCC_INVITE_PARAMS_INIT;
    invite.size = offsetof(dcc_invite_params_t, max_age) + 1U;
    TASK7_EXPECT_LOCAL_REJECTION(
        "invite rejects absent partially covered field", capture, callback,
        observer, dcc_rest_create_channel_invite(
            client, 710U, &invite, &options, &request
        )
    );

    dcc_channel_permission_overwrite_t overwrite =
        DCC_CHANNEL_PERMISSION_OVERWRITE_INIT;
    overwrite.size = offsetof(dcc_channel_permission_overwrite_t, allow) + 1U;
    TASK7_EXPECT_LOCAL_REJECTION(
        "overwrite rejects absent partially covered field", capture, callback,
        observer, dcc_rest_modify_channel_permission(
            client, 711U, 712U, &overwrite, &options, &request
        )
    );

    dcc_channel_position_t partial_position = DCC_CHANNEL_POSITION_INIT;
    partial_position.channel_id = 713U;
    partial_position.size = offsetof(dcc_channel_position_t, flags) + 1U;
    dcc_channel_positions_params_t positions =
        DCC_CHANNEL_POSITIONS_PARAMS_INIT;
    positions.positions = &partial_position;
    positions.position_count = 1U;
    TASK7_EXPECT_LOCAL_REJECTION(
        "position rejects absent partially covered field", capture, callback,
        observer, dcc_rest_modify_guild_channel_positions(
            client, 714U, &positions, &options, &request
        )
    );

    dcc_channel_params_t channel = DCC_CHANNEL_PARAMS_INIT;
    channel.payload.guild.present = DCC_CHANNEL_GUILD_PRESENT_NAME |
        DCC_CHANNEL_GUILD_PRESENT_TYPE | DCC_CHANNEL_GUILD_PRESENT_FLAGS;
    channel.payload.guild.name = "bad flags";
    channel.payload.guild.type = DCC_CHANNEL_TEXT;
    channel.payload.guild.flags = DCC_CHANNEL_FLAG_HIDE_MEDIA_DOWNLOAD_OPTIONS;
    TASK7_EXPECT_LOCAL_REJECTION(
        "channel rejects type-inappropriate flags", capture, callback,
        observer, dcc_rest_create_guild_channel(
            client, 715U, &channel, &options, &request
        )
    );

    channel = (dcc_channel_params_t)DCC_CHANNEL_PARAMS_INIT;
    channel.payload.guild.present = DCC_CHANNEL_GUILD_PRESENT_NAME |
        DCC_CHANNEL_GUILD_PRESENT_TYPE | DCC_CHANNEL_GUILD_PRESENT_PARENT_ID;
    channel.payload.guild.name = "bad null";
    channel.payload.guild.type = DCC_CHANNEL_TEXT;
    TASK7_EXPECT_LOCAL_REJECTION(
        "channel rejects zero parent without explicit null", capture, callback,
        observer, dcc_rest_create_guild_channel(
            client, 716U, &channel, &options, &request
        )
    );

    task7_extended_position_t extended[2] = {
        {.value = DCC_CHANNEL_POSITION_INIT, .future = 1U},
        {.value = DCC_CHANNEL_POSITION_INIT, .future = 2U},
    };
    extended[0].value.size = sizeof(extended[0]);
    extended[0].value.channel_id = 717U;
    extended[0].value.present = DCC_CHANNEL_POSITION_PRESENT_POSITION;
    extended[0].value.position = 3;
    extended[1].value.size = sizeof(extended[1]);
    extended[1].value.channel_id = 718U;
    extended[1].value.present = DCC_CHANNEL_POSITION_PRESENT_FLAGS;
    extended[1].value.flags = DCC_CHANNEL_FLAG_PINNED;
    positions = (dcc_channel_positions_params_t)
        DCC_CHANNEL_POSITIONS_PARAMS_INIT;
    positions.positions = (const dcc_channel_position_t *)extended;
    positions.position_count = 2U;
    endpoint_capture_reset(capture, 0U);
    atomic_store_explicit(&capture->capture_release, 1U, memory_order_release);
    dcc_rest_request_t *request = NULL;
    dcc_status_t submit = dcc_rest_modify_guild_channel_positions(
        client, 719U, &positions, NULL, &request
    );
    return endpoint_completed_contract(
        "position array uses declared stride", capture, submit, request,
        "PATCH", "/guilds/719/channels", "application/json",
        "[{\"id\":\"717\",\"position\":3},{\"id\":\"718\",\"flags\":2}]",
        sizeof("[{\"id\":\"717\",\"position\":3},{\"id\":\"718\",\"flags\":2}]") - 1U
    );
}

static int task7_app_adapter_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture
) {
    dcc_app_t app_storage;
    memset(&app_storage, 0, sizeof(app_storage));
    app_storage.client = client;
    dcc_app_t *app = &app_storage;
    task7_app_callback_t callback;
    atomic_init(&callback.calls, 0U);

    dcc_channel_params_t channel = DCC_CHANNEL_PARAMS_INIT;
    channel.payload.guild.present = DCC_CHANNEL_GUILD_PRESENT_NAME |
        DCC_CHANNEL_GUILD_PRESENT_TYPE;
    channel.payload.guild.name = "adapter";
    channel.payload.guild.type = DCC_CHANNEL_TEXT;
    endpoint_capture_reset(capture, 0U);
    dcc_status_t status = dcc_app_create_guild_channel_params(
        app, 801U, &channel, task7_app_rest_callback, &callback
    );
    int returned_before_transport = status == DCC_OK &&
        atomic_load_explicit(&capture->calls, memory_order_acquire) == 0U;
    atomic_store_explicit(&capture->capture_release, 1U, memory_order_release);
    int first_ok = returned_before_transport &&
        endpoint_wait_for_atomic(&callback.calls, 1U, 3000U) &&
        atomic_load_explicit(&capture->calls, memory_order_acquire) == 1U &&
        strcmp(capture->method, "POST") == 0 &&
        strcmp(capture->path, "/guilds/801/channels") == 0 &&
        endpoint_contains(
            capture->body, capture->body_len,
            "{\"name\":\"adapter\",\"type\":0}",
            sizeof("{\"name\":\"adapter\",\"type\":0}") - 1U
        );

    dcc_thread_params_t thread = DCC_THREAD_PARAMS_INIT;
    thread.present = DCC_THREAD_PARAMS_PRESENT_NAME |
        DCC_THREAD_PARAMS_PRESENT_RATE_LIMIT_PER_USER;
    thread.name = "renamed";
    thread.rate_limit_per_user = 9U;
    endpoint_capture_reset(capture, 0U);
    status = dcc_app_modify_thread(
        app, 802U, &thread, task7_app_rest_callback, &callback
    );
    returned_before_transport = status == DCC_OK &&
        atomic_load_explicit(&capture->calls, memory_order_acquire) == 0U;
    atomic_store_explicit(&capture->capture_release, 1U, memory_order_release);
    int second_ok = returned_before_transport &&
        endpoint_wait_for_atomic(&callback.calls, 2U, 3000U) &&
        atomic_load_explicit(&capture->calls, memory_order_acquire) == 1U &&
        strcmp(capture->method, "PATCH") == 0 &&
        strcmp(capture->path, "/channels/802") == 0 &&
        endpoint_contains(
            capture->body, capture->body_len,
            "{\"name\":\"renamed\",\"rate_limit_per_user\":9}",
            sizeof("{\"name\":\"renamed\",\"rate_limit_per_user\":9}") - 1U
        );

    endpoint_capture_reset(capture, 0U);
    status = dcc_app_archive_thread(
        app, 803U, task7_app_rest_callback, &callback
    );
    returned_before_transport = status == DCC_OK &&
        atomic_load_explicit(&capture->calls, memory_order_acquire) == 0U;
    atomic_store_explicit(&capture->capture_release, 1U, memory_order_release);
    int third_ok = returned_before_transport &&
        endpoint_wait_for_atomic(&callback.calls, 3U, 3000U) &&
        atomic_load_explicit(&capture->calls, memory_order_acquire) == 1U &&
        strcmp(capture->method, "PATCH") == 0 &&
        strcmp(capture->path, "/channels/803") == 0 &&
        endpoint_contains(
            capture->body, capture->body_len,
            "{\"archived\":true}",
            sizeof("{\"archived\":true}") - 1U
        );

    if (!first_ok || !second_ok || !third_ok) {
        fprintf(
            stderr,
            "Task 7 App adapter mismatch first=%d second=%d third=%d\n",
            first_ok, second_ok, third_ok
        );
        return 1;
    }
    return 0;
}

typedef struct task7_error_capture {
    atomic_uint calls;
    const char *secret;
    char operation[128];
    atomic_uint raw_secret_hits;
} task7_error_capture_t;

static void task7_error_capture_callback(
    dcc_client_t *client,
    const dcc_error_t *error,
    void *user_data
) {
    task7_error_capture_t *capture = (task7_error_capture_t *)user_data;
    (void)client;
    const char *operation = error != NULL && error->operation != NULL
        ? error->operation
        : "";
    (void)snprintf(capture->operation, sizeof(capture->operation), "%s", operation);
    if ((capture->secret != NULL && strstr(operation, capture->secret) != NULL) ||
        strchr(operation, '/') != NULL || strchr(operation, '?') != NULL) {
        atomic_fetch_add_explicit(
            &capture->raw_secret_hits, 1U, memory_order_acq_rel
        );
    }
    atomic_fetch_add_explicit(&capture->calls, 1U, memory_order_acq_rel);
}

static void task7_counting_observer(
    dcc_client_t *client,
    const dcc_error_t *error,
    void *user_data
) {
    endpoint_observer_t *observer = (endpoint_observer_t *)user_data;
    (void)client;
    (void)error;
    atomic_fetch_add_explicit(&observer->calls, 1U, memory_order_acq_rel);
}

static int task7_sensitive_transport_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_observer_t *observer
) {
    static const char encoded_token[] = "tok%2Fen";
    static const char expected_fingerprint[] =
        "86e135246b6d7e217bdd84701d256b11057f4e5c502a31eef54117c396f4da57";
    char fingerprint[65] = {0};
    if (dcc_endpoint_sensitive_route_fingerprint(
            encoded_token,
            sizeof(encoded_token) - 1U,
            fingerprint
        ) != DCC_OK || strcmp(fingerprint, expected_fingerprint) != 0) {
        fprintf(stderr, "sensitive route fingerprint contract failed\n");
        return 1;
    }

    unsigned char zero_probe[] = {1U, 2U, 3U, 4U, 5U};
    dcc_endpoint_secure_zero(zero_probe, sizeof(zero_probe));
    static const unsigned char zeroes[sizeof(zero_probe)] = {0};
    if (memcmp(zero_probe, zeroes, sizeof(zero_probe)) != 0) {
        fprintf(stderr, "secure-zero primitive left bytes behind\n");
        return 1;
    }

    static const char interaction_secret[] =
        "task7-interaction/raw?secret=owned-before-return";
    char *interaction_token = (char *)malloc(sizeof(interaction_secret));
    if (interaction_token == NULL) {
        return 1;
    }
    memcpy(interaction_token, interaction_secret, sizeof(interaction_secret));
    task7_error_capture_t public_error = {.secret = interaction_secret};
    task7_error_capture_t app_error = {.secret = interaction_secret};
    atomic_init(&public_error.calls, 0U);
    atomic_init(&public_error.raw_secret_hits, 0U);
    atomic_init(&app_error.calls, 0U);
    atomic_init(&app_error.raw_secret_hits, 0U);
    if (dcc_client_on_error(
            client, task7_error_capture_callback, &public_error
        ) != DCC_OK ||
        dcc_rest_set_app_error_sink(
            client, task7_error_capture_callback, &app_error
        ) != DCC_OK) {
        free(interaction_token);
        return 1;
    }

    dcc_endpoint_test_sensitive_probe_begin(
        interaction_token,
        strlen(interaction_token)
    );
    endpoint_capture_reset(capture, 0U);
    atomic_store_explicit(&capture->capture_release, 1U, memory_order_release);
    atomic_store_explicit(&capture->response_status, 0U, memory_order_release);
    atomic_store_explicit(
        &capture->response_error, DCC_ERR_NETWORK, memory_order_release
    );
    dcc_rest_request_t *request = NULL;
    dcc_status_t submit = dcc_rest_interaction_original_response_get(
        client,
        UINT64_C(123),
        interaction_token,
        NULL,
        &request
    );
    memset(interaction_token, 'x', strlen(interaction_token));
    free(interaction_token);
    interaction_token = NULL;
    const dcc_rest_result_t *result = NULL;
    dcc_status_t waited = request != NULL
        ? dcc_rest_request_wait(request, 3000U, &result)
        : DCC_ERR_STATE;
    dcc_rest_request_destroy(request);

    dcc_endpoint_sensitive_probe_snapshot_t snapshot =
        DCC_ENDPOINT_SENSITIVE_PROBE_SNAPSHOT_INIT;
    dcc_status_t snapshot_status =
        dcc_endpoint_test_sensitive_probe_snapshot(&snapshot);
    dcc_endpoint_test_sensitive_probe_end();
    (void)dcc_rest_set_app_error_sink(client, NULL, NULL);
    (void)dcc_client_on_error(client, task7_counting_observer, observer);

    uint32_t raw_hits = snapshot.raw_log_hits |
        snapshot.raw_public_observer_hits |
        snapshot.raw_app_observer_hits |
        snapshot.raw_operation_hits |
        snapshot.raw_rate_limit_key_hits |
        snapshot.raw_async_route_hits |
        snapshot.raw_authorization_retention_hits;
    if (submit != DCC_OK || waited != DCC_OK || result == NULL ||
        snapshot_status != DCC_OK || raw_hits != 0U ||
        snapshot.opaque_route_key_hits == 0U ||
        snapshot.secure_wipe_calls == 0U || snapshot.secure_wipe_bytes == 0U ||
        (snapshot.wiped_material_mask & TASK7_SENSITIVE_PATH_MATERIALS) !=
            TASK7_SENSITIVE_PATH_MATERIALS ||
        (snapshot.wiped_lifecycle_mask &
            DCC_ENDPOINT_SENSITIVE_WIPED_COMPLETION) == 0U ||
        atomic_load_explicit(&public_error.calls, memory_order_acquire) != 1U ||
        atomic_load_explicit(&app_error.calls, memory_order_acquire) != 1U ||
        atomic_load_explicit(&public_error.raw_secret_hits, memory_order_acquire) != 0U ||
        atomic_load_explicit(&app_error.raw_secret_hits, memory_order_acquire) != 0U ||
        strcmp(public_error.operation,
            "dcc_rest_interaction_original_response_get") != 0 ||
        strcmp(app_error.operation,
            "dcc_rest_interaction_original_response_get") != 0) {
        fprintf(
            stderr,
            "sensitive transport detail submit=%s wait=%s snapshot=%s raw=%u "
            "opaque=%u wipes=%zu/%zu material=%x lifecycle=%x public=%u:%s app=%u:%s\n",
            dcc_status_string(submit),
            dcc_status_string(waited),
            dcc_status_string(snapshot_status),
            raw_hits,
            snapshot.opaque_route_key_hits,
            snapshot.secure_wipe_calls,
            snapshot.secure_wipe_bytes,
            snapshot.wiped_material_mask,
            snapshot.wiped_lifecycle_mask,
            atomic_load_explicit(&public_error.calls, memory_order_acquire),
            public_error.operation,
            atomic_load_explicit(&app_error.calls, memory_order_acquire),
            app_error.operation
        );
        return 1;
    }

    /* Raw submission has a fixed nonsecret observer identity too. */
    static const char raw_secret[] = "raw-webhook-token-secret";
    public_error.secret = raw_secret;
    app_error.secret = raw_secret;
    memset(public_error.operation, 0, sizeof(public_error.operation));
    memset(app_error.operation, 0, sizeof(app_error.operation));
    atomic_store_explicit(&public_error.calls, 0U, memory_order_release);
    atomic_store_explicit(&public_error.raw_secret_hits, 0U, memory_order_release);
    atomic_store_explicit(&app_error.calls, 0U, memory_order_release);
    atomic_store_explicit(&app_error.raw_secret_hits, 0U, memory_order_release);
    if (dcc_client_on_error(
            client, task7_error_capture_callback, &public_error
        ) != DCC_OK ||
        dcc_rest_set_app_error_sink(
            client, task7_error_capture_callback, &app_error
        ) != DCC_OK) {
        return 1;
    }
    endpoint_capture_reset(capture, 0U);
    atomic_store_explicit(&capture->capture_release, 1U, memory_order_release);
    atomic_store_explicit(&capture->response_status, 0U, memory_order_release);
    atomic_store_explicit(
        &capture->response_error, DCC_ERR_NETWORK, memory_order_release
    );
    dcc_rest_request_desc_t raw = DCC_REST_REQUEST_DESC_INIT;
    raw.path = "/webhooks/123/raw-webhook-token-secret";
    request = NULL;
    submit = dcc_rest_submit(client, &raw, &request);
    result = NULL;
    waited = request != NULL
        ? dcc_rest_request_wait(request, 3000U, &result)
        : DCC_ERR_STATE;
    dcc_rest_request_destroy(request);
    (void)dcc_rest_set_app_error_sink(client, NULL, NULL);
    (void)dcc_client_on_error(client, task7_counting_observer, observer);
    if (submit != DCC_OK || waited != DCC_OK || result == NULL ||
        atomic_load_explicit(&public_error.calls, memory_order_acquire) != 1U ||
        atomic_load_explicit(&app_error.calls, memory_order_acquire) != 1U ||
        atomic_load_explicit(&public_error.raw_secret_hits, memory_order_acquire) != 0U ||
        atomic_load_explicit(&app_error.raw_secret_hits, memory_order_acquire) != 0U ||
        strcmp(public_error.operation, "dcc_rest_submit") != 0 ||
        strcmp(app_error.operation, "dcc_rest_submit") != 0) {
        fprintf(stderr, "raw submit leaked its path as observer operation\n");
        return 1;
    }
    return 0;
}

static uint32_t task7_sensitive_raw_hits(
    const dcc_endpoint_sensitive_probe_snapshot_t *snapshot
);

#define TASK7_EXPECT_SENSITIVE_FAILURE(operation_, call_) \
    do { \
        for (unsigned failure_kind = 0U; failure_kind < 2U; ++failure_kind) { \
            char token[sizeof(sensitive_secret)]; \
            memcpy(token, sensitive_secret, sizeof(token)); \
            public_error.secret = sensitive_secret; \
            app_error.secret = sensitive_secret; \
            memset(public_error.operation, 0, sizeof(public_error.operation)); \
            memset(app_error.operation, 0, sizeof(app_error.operation)); \
            atomic_store_explicit( \
                &public_error.calls, 0U, memory_order_release \
            ); \
            atomic_store_explicit( \
                &public_error.raw_secret_hits, 0U, memory_order_release \
            ); \
            atomic_store_explicit( \
                &app_error.calls, 0U, memory_order_release \
            ); \
            atomic_store_explicit( \
                &app_error.raw_secret_hits, 0U, memory_order_release \
            ); \
            endpoint_capture_reset(capture, 0U); \
            atomic_store_explicit( \
                &capture->capture_release, 1U, memory_order_release \
            ); \
            atomic_store_explicit( \
                &capture->response_status, \
                failure_kind == 0U ? 0U : 500U, \
                memory_order_release \
            ); \
            atomic_store_explicit( \
                &capture->response_error, \
                failure_kind == 0U ? DCC_ERR_NETWORK : DCC_OK, \
                memory_order_release \
            ); \
            dcc_endpoint_test_sensitive_probe_begin( \
                sensitive_secret, sizeof(sensitive_secret) - 1U \
            ); \
            dcc_rest_request_t *request = NULL; \
            dcc_status_t submit = (call_); \
            memset(token, 'x', sizeof(token) - 1U); \
            const dcc_rest_result_t *result = NULL; \
            dcc_status_t waited = request != NULL \
                ? dcc_rest_request_wait(request, 3000U, &result) \
                : DCC_ERR_STATE; \
            int result_present = result != NULL; \
            dcc_rest_request_destroy(request); \
            dcc_endpoint_sensitive_probe_snapshot_t snapshot = \
                DCC_ENDPOINT_SENSITIVE_PROBE_SNAPSHOT_INIT; \
            dcc_status_t snapshot_status = \
                dcc_endpoint_test_sensitive_probe_snapshot(&snapshot); \
            dcc_endpoint_test_sensitive_probe_end(); \
            uint32_t raw_hits = snapshot.raw_log_hits | \
                snapshot.raw_public_observer_hits | \
                snapshot.raw_app_observer_hits | \
                snapshot.raw_operation_hits | \
                snapshot.raw_rate_limit_key_hits | \
                snapshot.raw_async_route_hits | \
                snapshot.raw_authorization_retention_hits; \
            if (submit != DCC_OK || waited != DCC_OK || !result_present || \
                snapshot_status != DCC_OK || raw_hits != 0U || \
                snapshot.opaque_route_key_hits == 0U || \
                snapshot.secure_wipe_calls == 0U || \
                snapshot.secure_wipe_bytes == 0U || \
                (snapshot.wiped_material_mask & \
                    TASK7_SENSITIVE_PATH_MATERIALS) != \
                    TASK7_SENSITIVE_PATH_MATERIALS || \
                (snapshot.wiped_lifecycle_mask & \
                    DCC_ENDPOINT_SENSITIVE_WIPED_COMPLETION) == 0U || \
                atomic_load_explicit( \
                    &public_error.calls, memory_order_acquire \
                ) != 1U || \
                atomic_load_explicit( \
                    &app_error.calls, memory_order_acquire \
                ) != 1U || \
                atomic_load_explicit( \
                    &public_error.raw_secret_hits, memory_order_acquire \
                ) != 0U || \
                atomic_load_explicit( \
                    &app_error.raw_secret_hits, memory_order_acquire \
                ) != 0U || \
                strcmp(public_error.operation, (operation_)) != 0 || \
                strcmp(app_error.operation, (operation_)) != 0) { \
                fprintf( \
                    stderr, \
                    "%s sensitive %s failure leaked identity/state: " \
                    "submit=%s wait=%s snapshot=%s raw=%u opaque=%u " \
                    "wipes=%zu/%zu material=%x lifecycle=%x public=%u:%s app=%u:%s\n", \
                    (operation_), \
                    failure_kind == 0U ? "transport" : "HTTP", \
                    dcc_status_string(submit), \
                    dcc_status_string(waited), \
                    dcc_status_string(snapshot_status), \
                    raw_hits, \
                    snapshot.opaque_route_key_hits, \
                    snapshot.secure_wipe_calls, \
                    snapshot.secure_wipe_bytes, \
                    snapshot.wiped_material_mask, \
                    snapshot.wiped_lifecycle_mask, \
                    atomic_load_explicit( \
                        &public_error.calls, memory_order_acquire \
                    ), \
                    public_error.operation, \
                    atomic_load_explicit( \
                        &app_error.calls, memory_order_acquire \
                    ), \
                    app_error.operation \
                ); \
                return 1; \
            } \
        } \
    } while (0)

static int task7_all_sensitive_endpoints_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_observer_t *observer
) {
    static const char sensitive_secret[] =
        "task7-all-sensitive/raw?secret=owned-before-return";
    task7_error_capture_t public_error = {.secret = sensitive_secret};
    task7_error_capture_t app_error = {.secret = sensitive_secret};
    atomic_init(&public_error.calls, 0U);
    atomic_init(&public_error.raw_secret_hits, 0U);
    atomic_init(&app_error.calls, 0U);
    atomic_init(&app_error.raw_secret_hits, 0U);
    if (dcc_client_on_error(
            client, task7_error_capture_callback, &public_error
        ) != DCC_OK ||
        dcc_rest_set_app_error_sink(
            client, task7_error_capture_callback, &app_error
        ) != DCC_OK) {
        return 1;
    }

    dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
    if (dcc_message_builder_set_content(&message, "sensitive matrix") != DCC_OK) {
        return 1;
    }
    dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
    dcc_rest_message_payload_init(&payload, &message);
    dcc_rest_webhook_execute_t execute = DCC_REST_WEBHOOK_EXECUTE_INIT;
    dcc_rest_webhook_execute_init(&execute, &message);
    dcc_rest_webhook_compat_payload_t compat =
        DCC_REST_WEBHOOK_COMPAT_PAYLOAD_INIT;
    compat.body = "{}";
    compat.body_len = 2U;
    dcc_rest_webhook_builder_t webhook = DCC_REST_WEBHOOK_BUILDER_INIT;
    webhook.present = DCC_REST_WEBHOOK_BUILDER_PRESENT_NAME;
    webhook.name = "sensitive webhook";
    dcc_rest_webhook_message_edit_t edit =
        DCC_REST_WEBHOOK_MESSAGE_EDIT_INIT;
    dcc_rest_webhook_message_edit_init(&edit, &payload);
    dcc_rest_interaction_response_t response =
        DCC_REST_INTERACTION_RESPONSE_INIT;
    if (dcc_rest_interaction_response_set_pong(&response) != DCC_OK) {
        return 1;
    }

    TASK7_EXPECT_SENSITIVE_FAILURE(
        "dcc_rest_delete_webhook",
        dcc_rest_delete_webhook(client, 600U, token, NULL, &request)
    );
    TASK7_EXPECT_SENSITIVE_FAILURE(
        "dcc_rest_delete_webhook_message",
        dcc_rest_delete_webhook_message(
            client, 600U, token, 601U, NULL, NULL, &request
        )
    );
    TASK7_EXPECT_SENSITIVE_FAILURE(
        "dcc_rest_execute_webhook",
        dcc_rest_execute_webhook(
            client, 600U, token, &execute, NULL, &request
        )
    );
    TASK7_EXPECT_SENSITIVE_FAILURE(
        "dcc_rest_execute_webhook_github",
        dcc_rest_execute_webhook_github(
            client, 600U, token, &compat, NULL, &request
        )
    );
    TASK7_EXPECT_SENSITIVE_FAILURE(
        "dcc_rest_execute_webhook_slack",
        dcc_rest_execute_webhook_slack(
            client, 600U, token, &compat, NULL, &request
        )
    );
    TASK7_EXPECT_SENSITIVE_FAILURE(
        "dcc_rest_get_webhook",
        dcc_rest_get_webhook(client, 600U, token, NULL, &request)
    );
    TASK7_EXPECT_SENSITIVE_FAILURE(
        "dcc_rest_get_webhook_message",
        dcc_rest_get_webhook_message(
            client, 600U, token, 601U, NULL, NULL, &request
        )
    );
    TASK7_EXPECT_SENSITIVE_FAILURE(
        "dcc_rest_interaction_followup_create",
        dcc_rest_interaction_followup_create(
            client, 700U, token, &payload, NULL, &request
        )
    );
    TASK7_EXPECT_SENSITIVE_FAILURE(
        "dcc_rest_interaction_followup_delete",
        dcc_rest_interaction_followup_delete(
            client, 700U, token, 701U, NULL, &request
        )
    );
    TASK7_EXPECT_SENSITIVE_FAILURE(
        "dcc_rest_interaction_followup_edit",
        dcc_rest_interaction_followup_edit(
            client, 700U, token, 701U, &payload, NULL, &request
        )
    );
    TASK7_EXPECT_SENSITIVE_FAILURE(
        "dcc_rest_interaction_followup_get",
        dcc_rest_interaction_followup_get(
            client, 700U, token, 701U, NULL, &request
        )
    );
    TASK7_EXPECT_SENSITIVE_FAILURE(
        "dcc_rest_interaction_original_response_delete",
        dcc_rest_interaction_original_response_delete(
            client, 700U, token, NULL, &request
        )
    );
    TASK7_EXPECT_SENSITIVE_FAILURE(
        "dcc_rest_interaction_original_response_edit",
        dcc_rest_interaction_original_response_edit(
            client, 700U, token, &payload, NULL, &request
        )
    );
    TASK7_EXPECT_SENSITIVE_FAILURE(
        "dcc_rest_interaction_original_response_get",
        dcc_rest_interaction_original_response_get(
            client, 700U, token, NULL, &request
        )
    );
    TASK7_EXPECT_SENSITIVE_FAILURE(
        "dcc_rest_interaction_response_create",
        dcc_rest_interaction_response_create(
            client, 700U, token, &response, NULL, &request
        )
    );
    TASK7_EXPECT_SENSITIVE_FAILURE(
        "dcc_rest_modify_webhook",
        dcc_rest_modify_webhook(
            client, 600U, token, &webhook, NULL, &request
        )
    );
    TASK7_EXPECT_SENSITIVE_FAILURE(
        "dcc_rest_modify_webhook_message",
        dcc_rest_modify_webhook_message(
            client, 600U, token, 601U, &edit, NULL, &request
        )
    );

    (void)dcc_rest_set_app_error_sink(client, NULL, NULL);
    (void)dcc_client_on_error(client, task7_counting_observer, observer);
    return 0;
}

#undef TASK7_EXPECT_SENSITIVE_FAILURE

static int task7_bearer_lifetime_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_observer_t *observer
) {
    static const char bearer_secret[] =
        "task7-bearer/raw?secret=must-be-owned-and-wiped";
    static const char bearer_authorization[] =
        "Bearer task7-bearer/raw?secret=must-be-owned-and-wiped";
    task7_error_capture_t public_error = {.secret = bearer_secret};
    task7_error_capture_t app_error = {.secret = bearer_secret};
    atomic_init(&public_error.calls, 0U);
    atomic_init(&public_error.raw_secret_hits, 0U);
    atomic_init(&app_error.calls, 0U);
    atomic_init(&app_error.raw_secret_hits, 0U);
    if (dcc_client_on_error(
            client, task7_error_capture_callback, &public_error
        ) != DCC_OK ||
        dcc_rest_set_app_error_sink(
            client, task7_error_capture_callback, &app_error
        ) != DCC_OK) {
        return 1;
    }

    char *bearer = (char *)malloc(sizeof(bearer_secret));
    if (bearer == NULL) {
        return 1;
    }
    memcpy(bearer, bearer_secret, sizeof(bearer_secret));
    dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
    options.auth_mode = DCC_REST_AUTH_BEARER;
    options.auth_token = bearer;
    endpoint_capture_reset(capture, 0U);
    atomic_store_explicit(&capture->capture_release, 1U, memory_order_release);
    atomic_store_explicit(&capture->response_status, 0U, memory_order_release);
    atomic_store_explicit(
        &capture->response_error, DCC_ERR_NETWORK, memory_order_release
    );
    dcc_endpoint_test_sensitive_probe_begin(
        bearer_secret, sizeof(bearer_secret) - 1U
    );
    dcc_endpoint_test_preparation_probe_begin(
        DCC_ENDPOINT_PREPARATION_CHECK_OPERATION |
            DCC_ENDPOINT_PREPARATION_CHECK_AUTHORIZATION,
        "dcc_rest_create_lobby_channel_invite_for_self",
        NULL,
        bearer_authorization
    );
    dcc_rest_request_t *request = NULL;
    dcc_status_t submit = dcc_rest_create_lobby_channel_invite_for_self(
        client, UINT64_C(800), &options, &request
    );
    memset(bearer, 'x', sizeof(bearer_secret) - 1U);
    free(bearer);
    const dcc_rest_result_t *result = NULL;
    dcc_status_t waited = request != NULL
        ? dcc_rest_request_wait(request, 3000U, &result)
        : DCC_ERR_STATE;
    int result_present = result != NULL;
    dcc_rest_request_destroy(request);
    dcc_endpoint_sensitive_probe_snapshot_t snapshot =
        DCC_ENDPOINT_SENSITIVE_PROBE_SNAPSHOT_INIT;
    dcc_status_t snapshot_status =
        dcc_endpoint_test_sensitive_probe_snapshot(&snapshot);
    dcc_endpoint_test_sensitive_probe_end();
    dcc_endpoint_preparation_probe_snapshot_t preparation =
        DCC_ENDPOINT_PREPARATION_PROBE_SNAPSHOT_INIT;
    dcc_status_t preparation_status =
        dcc_endpoint_test_preparation_probe_snapshot(&preparation);
    dcc_endpoint_test_preparation_probe_end();
    uint32_t raw_hits = task7_sensitive_raw_hits(&snapshot);
    int completion_ok = submit == DCC_OK && waited == DCC_OK && result_present &&
        snapshot_status == DCC_OK && raw_hits == 0U &&
        preparation_status == DCC_OK && preparation.preparations == 1U &&
        preparation.operation_matches == 1U &&
        preparation.authorization_matches == 1U &&
        (snapshot.wiped_material_mask & TASK7_SENSITIVE_AUTH_MATERIALS) ==
            TASK7_SENSITIVE_AUTH_MATERIALS &&
        (snapshot.wiped_lifecycle_mask &
            DCC_ENDPOINT_SENSITIVE_WIPED_COMPLETION) != 0U &&
        snapshot.secure_wipe_calls != 0U && snapshot.secure_wipe_bytes != 0U &&
        atomic_load_explicit(&public_error.calls, memory_order_acquire) == 1U &&
        atomic_load_explicit(&app_error.calls, memory_order_acquire) == 1U &&
        atomic_load_explicit(
            &public_error.raw_secret_hits, memory_order_acquire
        ) == 0U &&
        atomic_load_explicit(
            &app_error.raw_secret_hits, memory_order_acquire
        ) == 0U &&
        strcmp(
            public_error.operation,
            "dcc_rest_create_lobby_channel_invite_for_self"
        ) == 0 &&
        strcmp(
            app_error.operation,
            "dcc_rest_create_lobby_channel_invite_for_self"
        ) == 0;
    if (!completion_ok) {
        fprintf(
            stderr,
            "Bearer lifetime submit=%s wait=%s snapshot=%s prep=%s:%u/%u/%u raw=%u "
            "wipes=%zu/%zu material=%x lifecycle=%x public=%u:%s app=%u:%s\n",
            dcc_status_string(submit),
            dcc_status_string(waited),
            dcc_status_string(snapshot_status),
            dcc_status_string(preparation_status),
            preparation.preparations,
            preparation.operation_matches,
            preparation.authorization_matches,
            raw_hits,
            snapshot.secure_wipe_calls,
            snapshot.secure_wipe_bytes,
            snapshot.wiped_material_mask,
            snapshot.wiped_lifecycle_mask,
            atomic_load_explicit(&public_error.calls, memory_order_acquire),
            public_error.operation,
            atomic_load_explicit(&app_error.calls, memory_order_acquire),
            app_error.operation
        );
        return 1;
    }

    struct task7_bearer_failure_case {
        dcc_endpoint_sensitive_failure_point_t point;
        dcc_status_t status;
        uint32_t lifecycle;
    } cases[] = {
        {
            DCC_ENDPOINT_SENSITIVE_FAILURE_ADMISSION,
            DCC_ERR_STATE,
            DCC_ENDPOINT_SENSITIVE_WIPED_ADMISSION_FAILURE,
        },
        {
            DCC_ENDPOINT_SENSITIVE_FAILURE_ALLOCATION_ROLLBACK,
            DCC_ERR_NOMEM,
            DCC_ENDPOINT_SENSITIVE_WIPED_ROLLBACK,
        },
    };
    for (size_t index = 0U; index < sizeof(cases) / sizeof(cases[0]); ++index) {
        bearer = (char *)malloc(sizeof(bearer_secret));
        if (bearer == NULL) {
            return 1;
        }
        memcpy(bearer, bearer_secret, sizeof(bearer_secret));
        options = (dcc_rest_call_options_t)DCC_REST_CALL_OPTIONS_INIT;
        options.auth_mode = DCC_REST_AUTH_BEARER;
        options.auth_token = bearer;
        atomic_store_explicit(&public_error.calls, 0U, memory_order_release);
        atomic_store_explicit(&app_error.calls, 0U, memory_order_release);
        endpoint_capture_reset(capture, 0U);
        atomic_store_explicit(
            &capture->capture_release, 1U, memory_order_release
        );
        dcc_endpoint_test_sensitive_probe_begin(
            bearer_secret, sizeof(bearer_secret) - 1U
        );
        dcc_endpoint_test_sensitive_force_failure_once(cases[index].point);
        request = (dcc_rest_request_t *)(uintptr_t)1U;
        submit = dcc_rest_create_lobby_channel_invite_for_self(
            client, UINT64_C(800), &options, &request
        );
        memset(bearer, 'x', sizeof(bearer_secret) - 1U);
        free(bearer);
        snapshot = (dcc_endpoint_sensitive_probe_snapshot_t)
            DCC_ENDPOINT_SENSITIVE_PROBE_SNAPSHOT_INIT;
        snapshot_status = dcc_endpoint_test_sensitive_probe_snapshot(&snapshot);
        dcc_endpoint_test_sensitive_probe_end();
        if (submit != cases[index].status || request != NULL ||
            snapshot_status != DCC_OK ||
            task7_sensitive_raw_hits(&snapshot) != 0U ||
            (snapshot.wiped_material_mask & TASK7_SENSITIVE_AUTH_MATERIALS) !=
                TASK7_SENSITIVE_AUTH_MATERIALS ||
            (snapshot.wiped_lifecycle_mask & cases[index].lifecycle) == 0U ||
            snapshot.secure_wipe_calls == 0U ||
            snapshot.secure_wipe_bytes == 0U ||
            atomic_load_explicit(&capture->calls, memory_order_acquire) != 0U ||
            atomic_load_explicit(&public_error.calls, memory_order_acquire) != 0U ||
            atomic_load_explicit(&app_error.calls, memory_order_acquire) != 0U) {
            fprintf(
                stderr,
                "Bearer forced failure %zu status=%s request=%p snapshot=%s "
                "raw=%u wipes=%zu/%zu material=%x lifecycle=%x\n",
                index,
                dcc_status_string(submit),
                (void *)request,
                dcc_status_string(snapshot_status),
                task7_sensitive_raw_hits(&snapshot),
                snapshot.secure_wipe_calls,
                snapshot.secure_wipe_bytes,
                snapshot.wiped_material_mask,
                snapshot.wiped_lifecycle_mask
            );
            dcc_rest_request_destroy(request);
            return 1;
        }
    }
    (void)dcc_rest_set_app_error_sink(client, NULL, NULL);
    (void)dcc_client_on_error(client, task7_counting_observer, observer);
    return 0;
}

static uint32_t task7_sensitive_raw_hits(
    const dcc_endpoint_sensitive_probe_snapshot_t *snapshot
) {
    return snapshot->raw_log_hits |
        snapshot->raw_public_observer_hits |
        snapshot->raw_app_observer_hits |
        snapshot->raw_operation_hits |
        snapshot->raw_rate_limit_key_hits |
        snapshot->raw_async_route_hits |
        snapshot->raw_authorization_retention_hits;
}

static int task7_sensitive_forced_failure_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_callback_t *callback,
    endpoint_observer_t *observer,
    dcc_endpoint_sensitive_failure_point_t failure_point,
    dcc_status_t expected_status,
    uint32_t expected_lifecycle
) {
    static const char secret[] = "task7-forced-sensitive/failure?token";
    endpoint_callback_reset(callback);
    dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
    options.callback = endpoint_result_callback;
    options.user_data = callback;
    endpoint_capture_reset(capture, 0U);
    atomic_store_explicit(&capture->capture_release, 1U, memory_order_release);
    unsigned observer_before = atomic_load_explicit(
        &observer->calls, memory_order_acquire
    );
    dcc_endpoint_test_sensitive_probe_begin(secret, sizeof(secret) - 1U);
    dcc_endpoint_test_sensitive_force_failure_once(failure_point);
    dcc_rest_request_t *request = (dcc_rest_request_t *)(uintptr_t)1U;
    dcc_status_t status = dcc_rest_interaction_original_response_get(
        client, UINT64_C(5150), secret, &options, &request
    );
    dcc_endpoint_sensitive_probe_snapshot_t snapshot =
        DCC_ENDPOINT_SENSITIVE_PROBE_SNAPSHOT_INIT;
    dcc_status_t snapshot_status =
        dcc_endpoint_test_sensitive_probe_snapshot(&snapshot);
    dcc_endpoint_test_sensitive_probe_end();
    if (status != expected_status || request != NULL ||
        snapshot_status != DCC_OK || task7_sensitive_raw_hits(&snapshot) != 0U ||
        snapshot.secure_wipe_calls == 0U || snapshot.secure_wipe_bytes == 0U ||
        (snapshot.wiped_material_mask & TASK7_SENSITIVE_PATH_MATERIALS) !=
            TASK7_SENSITIVE_PATH_MATERIALS ||
        (snapshot.wiped_lifecycle_mask & expected_lifecycle) == 0U ||
        atomic_load_explicit(&capture->calls, memory_order_acquire) != 0U ||
        atomic_load_explicit(&callback->calls, memory_order_acquire) != 0U ||
        atomic_load_explicit(&observer->calls, memory_order_acquire) !=
            observer_before) {
        fprintf(
            stderr,
            "forced sensitive failure point=%d status=%s request=%p "
            "snapshot=%s raw=%u wipes=%zu/%zu material=%x lifecycle=%x capture=%u "
            "callback=%u observer=%u/%u\n",
            (int)failure_point,
            dcc_status_string(status),
            (void *)request,
            dcc_status_string(snapshot_status),
            task7_sensitive_raw_hits(&snapshot),
            snapshot.secure_wipe_calls,
            snapshot.secure_wipe_bytes,
            snapshot.wiped_material_mask,
            snapshot.wiped_lifecycle_mask,
            atomic_load_explicit(&capture->calls, memory_order_acquire),
            atomic_load_explicit(&callback->calls, memory_order_acquire),
            observer_before,
            atomic_load_explicit(&observer->calls, memory_order_acquire)
        );
        dcc_rest_request_destroy(request);
        return 1;
    }
    return 0;
}

static int task7_sensitive_cancel_or_destroy_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_callback_t *callback,
    int destroy_without_wait
) {
    char secret[] = "task7-sensitive-cancel-or-destroy";
    char original[sizeof(secret)];
    memcpy(original, secret, sizeof(secret));
    endpoint_callback_reset(callback);
    dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
    options.callback = endpoint_result_callback;
    options.user_data = callback;
    endpoint_capture_reset(capture, 0U);
    dcc_endpoint_test_sensitive_probe_begin(original, sizeof(original) - 1U);
    dcc_rest_request_t *request = NULL;
    callback->request_slot = &request;
    dcc_status_t submit = dcc_rest_interaction_original_response_get(
        client, UINT64_C(5151), secret, &options, &request
    );
    memset(secret, 'x', sizeof(secret) - 1U);
    int entered = submit == DCC_OK && request != NULL &&
        endpoint_wait_for_atomic(&capture->entered, 1U, 1000U);
    dcc_status_t cancel = DCC_ERR_STATE;
    dcc_status_t waited = DCC_ERR_STATE;
    const dcc_rest_result_t *result = NULL;
    int waited_result_present = 0;
    if (entered && destroy_without_wait) {
        dcc_rest_request_destroy(request);
        request = NULL;
    } else if (entered) {
        cancel = dcc_rest_request_cancel(request);
    }
    atomic_store_explicit(&capture->capture_release, 1U, memory_order_release);
    if (entered && !destroy_without_wait) {
        waited = dcc_rest_request_wait(request, 3000U, &result);
        waited_result_present = result != NULL;
        dcc_rest_request_destroy(request);
        request = NULL;
    } else if (entered) {
        (void)endpoint_wait_for_atomic(&callback->calls, 1U, 3000U);
        (void)dcc_rest_async_wait(client, 3000U);
    }

    dcc_endpoint_sensitive_probe_snapshot_t snapshot =
        DCC_ENDPOINT_SENSITIVE_PROBE_SNAPSHOT_INIT;
    dcc_status_t snapshot_status =
        dcc_endpoint_test_sensitive_probe_snapshot(&snapshot);
    dcc_endpoint_test_sensitive_probe_end();
    callback->request_slot = NULL;
    uint32_t lifecycle = destroy_without_wait
        ? DCC_ENDPOINT_SENSITIVE_WIPED_DESTROY
        : DCC_ENDPOINT_SENSITIVE_WIPED_CANCEL;
    int terminal_ok = destroy_without_wait
        ? callback->result != NULL &&
            dcc_rest_result_status(callback->result) == DCC_ERR_CANCELED
        : cancel == DCC_OK && waited == DCC_OK && waited_result_present &&
            callback->result != NULL &&
            dcc_rest_result_status(callback->result) == DCC_ERR_CANCELED;
    if (submit != DCC_OK || !entered || !terminal_ok ||
        snapshot_status != DCC_OK || task7_sensitive_raw_hits(&snapshot) != 0U ||
        snapshot.secure_wipe_calls == 0U || snapshot.secure_wipe_bytes == 0U ||
        (snapshot.wiped_material_mask & TASK7_SENSITIVE_PATH_MATERIALS) !=
            TASK7_SENSITIVE_PATH_MATERIALS ||
        (snapshot.wiped_lifecycle_mask & lifecycle) == 0U ||
        atomic_load_explicit(&callback->calls, memory_order_acquire) != 1U) {
        fprintf(
            stderr,
            "sensitive %s submit=%s entered=%d cancel=%s wait=%s "
            "snapshot=%s raw=%u wipes=%zu/%zu material=%x lifecycle=%x callback=%u\n",
            destroy_without_wait ? "destroy" : "cancel",
            dcc_status_string(submit),
            entered,
            dcc_status_string(cancel),
            dcc_status_string(waited),
            dcc_status_string(snapshot_status),
            task7_sensitive_raw_hits(&snapshot),
            snapshot.secure_wipe_calls,
            snapshot.secure_wipe_bytes,
            snapshot.wiped_material_mask,
            snapshot.wiped_lifecycle_mask,
            atomic_load_explicit(&callback->calls, memory_order_acquire)
        );
        dcc_rest_request_destroy(request);
        return 1;
    }
    return 0;
}

int endpoint_task7_contract(
    dcc_client_t *client,
    endpoint_capture_t *capture,
    endpoint_callback_t *callback,
    endpoint_observer_t *observer
) {
    if (task7_options_layout_contract(client, capture) != 0 ||
        task7_auth_policy_contract(
            client, capture, callback, observer
        ) != 0 ||
        task7_audit_reason_contract(
            client, capture, callback, observer
        ) != 0 ||
        task7_literal_route_contract(client, capture) != 0 ||
        endpoint_task7_forum_multipart_contract(client, capture) != 0 ||
        task7_record_validation_contract(
            client, capture, callback, observer
        ) != 0 ||
        task7_app_adapter_contract(client, capture) != 0 ||
        task7_sensitive_transport_contract(client, capture, observer) != 0 ||
        task7_all_sensitive_endpoints_contract(
            client, capture, observer
        ) != 0 ||
        task7_bearer_lifetime_contract(
            client, capture, observer
        ) != 0 ||
        task7_sensitive_forced_failure_contract(
            client,
            capture,
            callback,
            observer,
            DCC_ENDPOINT_SENSITIVE_FAILURE_ADMISSION,
            DCC_ERR_STATE,
            DCC_ENDPOINT_SENSITIVE_WIPED_ADMISSION_FAILURE
        ) != 0 ||
        task7_sensitive_forced_failure_contract(
            client,
            capture,
            callback,
            observer,
            DCC_ENDPOINT_SENSITIVE_FAILURE_ALLOCATION_ROLLBACK,
            DCC_ERR_NOMEM,
            DCC_ENDPOINT_SENSITIVE_WIPED_ROLLBACK
        ) != 0 ||
        task7_sensitive_cancel_or_destroy_contract(
            client, capture, callback, 0
        ) != 0 ||
        task7_sensitive_cancel_or_destroy_contract(
            client, capture, callback, 1
        ) != 0) {
        return 1;
    }
    return 0;
}
