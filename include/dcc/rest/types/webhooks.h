#ifndef DCC_REST_TYPES_WEBHOOKS_H
#define DCC_REST_TYPES_WEBHOOKS_H

#include <dcc/rest/base.h>
#include <dcc/rest/types/messages.h>

#define DCC_REST_WEBHOOK_BUILDER_VERSION 1U
#define DCC_REST_WEBHOOK_EXECUTE_VERSION 1U
#define DCC_REST_WEBHOOK_MESSAGE_QUERY_VERSION 1U
#define DCC_REST_WEBHOOK_MESSAGE_EDIT_VERSION 1U
#define DCC_REST_WEBHOOK_COMPAT_PAYLOAD_VERSION 1U

#define DCC_REST_WEBHOOK_BUILDER_PRESENT_NAME UINT64_C(1)
#define DCC_REST_WEBHOOK_BUILDER_PRESENT_AVATAR (UINT64_C(1) << 1U)
#define DCC_REST_WEBHOOK_BUILDER_PRESENT_CHANNEL_ID (UINT64_C(1) << 2U)

#define DCC_REST_WEBHOOK_EXECUTE_PRESENT_USERNAME UINT64_C(1)
#define DCC_REST_WEBHOOK_EXECUTE_PRESENT_AVATAR_URL (UINT64_C(1) << 1U)
#define DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_NAME (UINT64_C(1) << 2U)
#define DCC_REST_WEBHOOK_EXECUTE_PRESENT_APPLIED_TAG_IDS (UINT64_C(1) << 3U)
#define DCC_REST_WEBHOOK_EXECUTE_PRESENT_WAIT (UINT64_C(1) << 4U)
#define DCC_REST_WEBHOOK_EXECUTE_PRESENT_THREAD_ID (UINT64_C(1) << 5U)
#define DCC_REST_WEBHOOK_EXECUTE_PRESENT_WITH_COMPONENTS (UINT64_C(1) << 6U)

#define DCC_REST_WEBHOOK_MESSAGE_QUERY_PRESENT_THREAD_ID UINT64_C(1)

#define DCC_REST_WEBHOOK_MESSAGE_EDIT_PRESENT_THREAD_ID UINT64_C(1)
#define DCC_REST_WEBHOOK_MESSAGE_EDIT_PRESENT_WITH_COMPONENTS (UINT64_C(1) << 1U)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Presence-gated webhook create/modify input.
 *
 * The header through `present` is mandatory. Strings are borrowed for the
 * endpoint call. Every present field must be covered by `size`; uncovered
 * suffix fields are absent, and larger version-1 records are accepted.
 */
typedef struct dcc_rest_webhook_builder {
    size_t size;
    uint32_t version;
    uint64_t present;
    const char *name;
    const char *avatar;
    dcc_snowflake_t channel_id;
} dcc_rest_webhook_builder_t;

#define DCC_REST_WEBHOOK_BUILDER_INIT \
    { sizeof(dcc_rest_webhook_builder_t), DCC_REST_WEBHOOK_BUILDER_VERSION, \
      UINT64_C(0), NULL, NULL, UINT64_C(0) }

static inline void dcc_rest_webhook_builder_init(
    dcc_rest_webhook_builder_t *builder
) {
    if (builder != NULL) {
        dcc_rest_webhook_builder_t value = DCC_REST_WEBHOOK_BUILDER_INIT;
        *builder = value;
    }
}

/**
 * @brief Typed webhook execution body, files, and query options.
 *
 * The prefix through `message` is mandatory. Every presence-gated field must
 * be covered by `size`. Historical prefixes that do not cover both `files`
 * and `file_count` treat files as absent; other uncovered optional suffix
 * fields are absent. Larger version-1 records are accepted. All nested
 * strings, arrays, builders, metadata, and file bytes are borrowed only for
 * the endpoint call and serialized before return.
 */
typedef struct dcc_rest_webhook_execute {
    size_t size;
    uint32_t version;
    uint64_t present;
    const dcc_message_builder_t *message;
    const dcc_rest_multipart_file_t *files;
    size_t file_count;
    const char *username;
    const char *avatar_url;
    const char *thread_name;
    const dcc_snowflake_t *applied_tag_ids;
    size_t applied_tag_count;
    uint8_t wait;
    dcc_snowflake_t thread_id;
    uint8_t with_components;
} dcc_rest_webhook_execute_t;

#define DCC_REST_WEBHOOK_EXECUTE_INIT \
    { sizeof(dcc_rest_webhook_execute_t), DCC_REST_WEBHOOK_EXECUTE_VERSION, \
      UINT64_C(0), NULL, NULL, 0U, NULL, NULL, NULL, NULL, 0U, 0U, \
      UINT64_C(0), 0U }

static inline void dcc_rest_webhook_execute_init(
    dcc_rest_webhook_execute_t *execute,
    const dcc_message_builder_t *message
) {
    if (execute != NULL) {
        dcc_rest_webhook_execute_t value = DCC_REST_WEBHOOK_EXECUTE_INIT;
        value.message = message;
        *execute = value;
    }
}

/**
 * @brief Presence-gated webhook-message query.
 *
 * The header through `present` is mandatory. A present thread ID must be
 * covered by `size`; an uncovered suffix is absent, and larger version-1
 * records are accepted.
 */
typedef struct dcc_rest_webhook_message_query {
    size_t size;
    uint32_t version;
    uint64_t present;
    dcc_snowflake_t thread_id;
} dcc_rest_webhook_message_query_t;

#define DCC_REST_WEBHOOK_MESSAGE_QUERY_INIT \
    { sizeof(dcc_rest_webhook_message_query_t), \
      DCC_REST_WEBHOOK_MESSAGE_QUERY_VERSION, UINT64_C(0), UINT64_C(0) }

static inline void dcc_rest_webhook_message_query_init(
    dcc_rest_webhook_message_query_t *query
) {
    if (query != NULL) {
        dcc_rest_webhook_message_query_t value =
            DCC_REST_WEBHOOK_MESSAGE_QUERY_INIT;
        *query = value;
    }
}

/**
 * @brief Webhook-message edit payload and presence-gated query options.
 *
 * The prefix through `payload` is mandatory. The payload is borrowed for the
 * endpoint call. Present query fields must be covered by `size`; uncovered
 * suffix fields are absent, and larger version-1 records are accepted.
 */
typedef struct dcc_rest_webhook_message_edit {
    size_t size;
    uint32_t version;
    uint64_t present;
    const dcc_rest_message_payload_t *payload;
    dcc_snowflake_t thread_id;
    uint8_t with_components;
} dcc_rest_webhook_message_edit_t;

#define DCC_REST_WEBHOOK_MESSAGE_EDIT_INIT \
    { sizeof(dcc_rest_webhook_message_edit_t), \
      DCC_REST_WEBHOOK_MESSAGE_EDIT_VERSION, UINT64_C(0), NULL, \
      UINT64_C(0), 0U }

static inline void dcc_rest_webhook_message_edit_init(
    dcc_rest_webhook_message_edit_t *edit,
    const dcc_rest_message_payload_t *payload
) {
    if (edit != NULL) {
        dcc_rest_webhook_message_edit_t value =
            DCC_REST_WEBHOOK_MESSAGE_EDIT_INIT;
        value.payload = payload;
        *edit = value;
    }
}

/**
 * @brief Exact opaque JSON byte span for Slack/GitHub compatible webhooks.
 *
 * The size/version header is mandatory. A historical prefix that does not
 * cover both `body` and `body_len` treats the body as absent; larger version-1
 * records are accepted. The byte span is borrowed only for the endpoint call
 * and copied before return.
 */
typedef struct dcc_rest_webhook_compat_payload {
    size_t size;
    uint32_t version;
    const void *body;
    size_t body_len;
} dcc_rest_webhook_compat_payload_t;

#define DCC_REST_WEBHOOK_COMPAT_PAYLOAD_INIT \
    { sizeof(dcc_rest_webhook_compat_payload_t), \
      DCC_REST_WEBHOOK_COMPAT_PAYLOAD_VERSION, NULL, 0U }

static inline void dcc_rest_webhook_compat_payload_init(
    dcc_rest_webhook_compat_payload_t *payload,
    const void *body,
    size_t body_len
) {
    if (payload != NULL) {
        dcc_rest_webhook_compat_payload_t value =
            DCC_REST_WEBHOOK_COMPAT_PAYLOAD_INIT;
        value.body = body;
        value.body_len = body_len;
        *payload = value;
    }
}

#ifdef __cplusplus
}
#endif

#endif
