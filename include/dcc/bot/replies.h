#ifndef DCC_BOT_REPLIES_H
#define DCC_BOT_REPLIES_H

#include <dcc/app/context.h>
#include <dcc/autocomplete.h>
#include <dcc/component_v2.h>
#include <dcc/message.h>
#include <dcc/modal.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Queues a borrowed Components v2 array as a state-aware reply. */
static inline dcc_status_t DCC_CTX_REPLY_UI_ARRAY(
    dcc_ctx_t *, const dcc_component_v2_builder_t *, size_t);
/** Queues an ephemeral borrowed Components v2 array reply. */
static inline dcc_status_t DCC_CTX_REPLY_EPHEMERAL_UI_ARRAY(
    dcc_ctx_t *, const dcc_component_v2_builder_t *, size_t);
/** Queues a Components v2 array as a component-source update. */
static inline dcc_status_t DCC_CTX_UPDATE_UI_ARRAY(
    dcc_ctx_t *, const dcc_component_v2_builder_t *, size_t);
/** Forces a borrowed Components v2 array as a followup. */
static inline dcc_status_t DCC_CTX_FOLLOWUP_UI_ARRAY(
    dcc_ctx_t *, const dcc_component_v2_builder_t *, size_t);
/** Forces an ephemeral Components v2 array followup. */
static inline dcc_status_t DCC_CTX_FOLLOWUP_EPHEMERAL_UI_ARRAY(
    dcc_ctx_t *, const dcc_component_v2_builder_t *, size_t);
/** Sends a borrowed Components v2 array to the unrelated channel. */
static inline dcc_status_t DCC_CTX_SEND_UI_ARRAY(
    dcc_ctx_t *, const dcc_component_v2_builder_t *, size_t);

static inline dcc_status_t dcc_bot_message_ephemeral(
    dcc_message_builder_t *message
) {
    if (message == NULL ||
        message->size < offsetof(dcc_message_builder_t, flags) +
                            sizeof(message->flags)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_message_builder_set_flags(
        message, message->flags | DCC_MESSAGE_FLAG_EPHEMERAL);
}

static inline dcc_status_t dcc_bot_message_ui(
    dcc_message_builder_t *message,
    const dcc_component_v2_builder_t *components,
    size_t component_count,
    uint8_t ephemeral
) {
    dcc_message_builder_init(message);
    dcc_status_t status = dcc_message_builder_set_components_v2(
        message, components, component_count);
    if (status == DCC_OK && ephemeral != 0U) {
        status = dcc_bot_message_ephemeral(message);
    }
    return status;
}

/** Queues a state-aware reply after synchronously serializing `message`. */
static inline dcc_status_t DCC_CTX_REPLY(
    dcc_ctx_t *ctx, dcc_message_builder_t message
) {
    return dcc_ctx_reply(ctx, &message, NULL, NULL);
}

/** Queues a state-aware text reply. */
static inline dcc_status_t DCC_CTX_REPLY_TEXT(
    dcc_ctx_t *ctx, const char *content
) {
    return dcc_ctx_reply_text(ctx, content, NULL, NULL);
}

/** Queues an ephemeral state-aware reply while preserving existing flags. */
static inline dcc_status_t DCC_CTX_REPLY_EPHEMERAL(
    dcc_ctx_t *ctx, dcc_message_builder_t message
) {
    dcc_status_t status = dcc_bot_message_ephemeral(&message);
    return status == DCC_OK ? dcc_ctx_reply(ctx, &message, NULL, NULL) : status;
}

/** Queues an ephemeral state-aware text reply. */
static inline dcc_status_t DCC_CTX_REPLY_EPHEMERAL_TEXT(
    dcc_ctx_t *ctx, const char *content
) {
    return dcc_ctx_reply_ephemeral_text(ctx, content, NULL, NULL);
}

/** Queues one Components v2 value as a state-aware reply. */
static inline dcc_status_t DCC_CTX_REPLY_UI(
    dcc_ctx_t *ctx, dcc_component_v2_builder_t component
) {
    return DCC_CTX_REPLY_UI_ARRAY(ctx, &component, 1U);
}

/** Queues a borrowed Components v2 array as a state-aware reply. */
static inline dcc_status_t DCC_CTX_REPLY_UI_ARRAY(
    dcc_ctx_t *ctx, const dcc_component_v2_builder_t *components,
    size_t component_count
) {
    dcc_message_builder_t message;
    dcc_status_t status = dcc_bot_message_ui(
        &message, components, component_count, 0U);
    return status == DCC_OK ? dcc_ctx_reply(ctx, &message, NULL, NULL) : status;
}

/** Queues one Components v2 value as an ephemeral state-aware reply. */
static inline dcc_status_t DCC_CTX_REPLY_EPHEMERAL_UI(
    dcc_ctx_t *ctx, dcc_component_v2_builder_t component
) {
    return DCC_CTX_REPLY_EPHEMERAL_UI_ARRAY(ctx, &component, 1U);
}

/** Queues an ephemeral borrowed Components v2 array reply. */
static inline dcc_status_t DCC_CTX_REPLY_EPHEMERAL_UI_ARRAY(
    dcc_ctx_t *ctx, const dcc_component_v2_builder_t *components,
    size_t component_count
) {
    dcc_message_builder_t message;
    dcc_status_t status = dcc_bot_message_ui(
        &message, components, component_count, 1U);
    return status == DCC_OK ? dcc_ctx_reply(ctx, &message, NULL, NULL) : status;
}

/** Defers the initial response. */
static inline dcc_status_t DCC_CTX_DEFER(dcc_ctx_t *ctx) {
    return dcc_ctx_defer(ctx, NULL, NULL);
}

/** Defers the initial response ephemerally. */
static inline dcc_status_t DCC_CTX_DEFER_EPHEMERAL(dcc_ctx_t *ctx) {
    return dcc_ctx_defer_ephemeral(ctx, NULL, NULL);
}

/** Queues a component-source message update. */
static inline dcc_status_t DCC_CTX_UPDATE(
    dcc_ctx_t *ctx, dcc_message_builder_t message
) {
    return dcc_ctx_update_message(ctx, &message, NULL, NULL);
}

/** Queues one Components v2 value as a component-source update. */
static inline dcc_status_t DCC_CTX_UPDATE_UI(
    dcc_ctx_t *ctx, dcc_component_v2_builder_t component
) {
    return DCC_CTX_UPDATE_UI_ARRAY(ctx, &component, 1U);
}

/** Queues a Components v2 array as a component-source update. */
static inline dcc_status_t DCC_CTX_UPDATE_UI_ARRAY(
    dcc_ctx_t *ctx, const dcc_component_v2_builder_t *components,
    size_t component_count
) {
    dcc_message_builder_t message;
    dcc_status_t status = dcc_bot_message_ui(
        &message, components, component_count, 0U);
    return status == DCC_OK
               ? dcc_ctx_update_message(ctx, &message, NULL, NULL)
               : status;
}

/** Forces a followup after synchronously serializing `message`. */
static inline dcc_status_t DCC_CTX_FOLLOWUP(
    dcc_ctx_t *ctx, dcc_message_builder_t message
) {
    return dcc_ctx_followup(ctx, &message, NULL, NULL);
}

/** Forces a text followup. */
static inline dcc_status_t DCC_CTX_FOLLOWUP_TEXT(
    dcc_ctx_t *ctx, const char *content
) {
    return dcc_ctx_followup_text(ctx, content, NULL, NULL);
}

/** Forces an ephemeral followup while preserving existing flags. */
static inline dcc_status_t DCC_CTX_FOLLOWUP_EPHEMERAL(
    dcc_ctx_t *ctx, dcc_message_builder_t message
) {
    dcc_status_t status = dcc_bot_message_ephemeral(&message);
    return status == DCC_OK ? dcc_ctx_followup(ctx, &message, NULL, NULL)
                            : status;
}

/** Forces an ephemeral text followup. */
static inline dcc_status_t DCC_CTX_FOLLOWUP_EPHEMERAL_TEXT(
    dcc_ctx_t *ctx, const char *content
) {
    return dcc_ctx_followup_ephemeral_text(ctx, content, NULL, NULL);
}

/** Forces one Components v2 value as a followup. */
static inline dcc_status_t DCC_CTX_FOLLOWUP_UI(
    dcc_ctx_t *ctx, dcc_component_v2_builder_t component
) {
    return DCC_CTX_FOLLOWUP_UI_ARRAY(ctx, &component, 1U);
}

/** Forces a borrowed Components v2 array as a followup. */
static inline dcc_status_t DCC_CTX_FOLLOWUP_UI_ARRAY(
    dcc_ctx_t *ctx, const dcc_component_v2_builder_t *components,
    size_t component_count
) {
    dcc_message_builder_t message;
    dcc_status_t status = dcc_bot_message_ui(
        &message, components, component_count, 0U);
    return status == DCC_OK ? dcc_ctx_followup(ctx, &message, NULL, NULL)
                            : status;
}

/** Forces one Components v2 value as an ephemeral followup. */
static inline dcc_status_t DCC_CTX_FOLLOWUP_EPHEMERAL_UI(
    dcc_ctx_t *ctx, dcc_component_v2_builder_t component
) {
    return DCC_CTX_FOLLOWUP_EPHEMERAL_UI_ARRAY(ctx, &component, 1U);
}

/** Forces an ephemeral Components v2 array followup. */
static inline dcc_status_t DCC_CTX_FOLLOWUP_EPHEMERAL_UI_ARRAY(
    dcc_ctx_t *ctx, const dcc_component_v2_builder_t *components,
    size_t component_count
) {
    dcc_message_builder_t message;
    dcc_status_t status = dcc_bot_message_ui(
        &message, components, component_count, 1U);
    return status == DCC_OK ? dcc_ctx_followup(ctx, &message, NULL, NULL)
                            : status;
}

/** Sends an unrelated channel message after synchronous input consumption. */
static inline dcc_status_t DCC_CTX_SEND(
    dcc_ctx_t *ctx, dcc_message_builder_t message
) {
    return dcc_ctx_send(ctx, &message, NULL, NULL);
}

/** Sends unrelated channel text. */
static inline dcc_status_t DCC_CTX_SEND_TEXT(
    dcc_ctx_t *ctx, const char *content
) {
    return dcc_ctx_send_text(ctx, content, NULL, NULL);
}

/** Sends one Components v2 value to the unrelated channel. */
static inline dcc_status_t DCC_CTX_SEND_UI(
    dcc_ctx_t *ctx, dcc_component_v2_builder_t component
) {
    return DCC_CTX_SEND_UI_ARRAY(ctx, &component, 1U);
}

/** Sends a borrowed Components v2 array to the unrelated channel. */
static inline dcc_status_t DCC_CTX_SEND_UI_ARRAY(
    dcc_ctx_t *ctx, const dcc_component_v2_builder_t *components,
    size_t component_count
) {
    dcc_message_builder_t message;
    dcc_status_t status = dcc_bot_message_ui(
        &message, components, component_count, 0U);
    return status == DCC_OK ? dcc_ctx_send(ctx, &message, NULL, NULL) : status;
}

/** Shows a modal after synchronously serializing its borrowed children. */
static inline dcc_status_t DCC_CTX_SHOW_MODAL(
    dcc_ctx_t *ctx, dcc_modal_builder_t modal
) {
    return dcc_ctx_show_modal(ctx, &modal, NULL, NULL);
}

/** Sends an autocomplete result after synchronously serializing choices. */
static inline dcc_status_t DCC_CTX_AUTOCOMPLETE(
    dcc_ctx_t *ctx, dcc_autocomplete_builder_t autocomplete
) {
    return dcc_ctx_reply_autocomplete(ctx, &autocomplete, NULL, NULL);
}

#ifdef __cplusplus
}
#endif

#endif
