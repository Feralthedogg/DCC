#include <dcc/app.h>
#include <dcc/objects/clone/interactions.h>

#include <llam/runtime.h>

#include <string.h>

static uint64_t dcc_app_wait_now_ms(void) {
    return llam_now_ns() / UINT64_C(1000000);
}

static dcc_status_t dcc_app_wait_result_prepare(dcc_app_wait_result_t *out) {
    if (out == NULL || (out->size != 0U && out->size < sizeof(*out))) {
        return DCC_ERR_INVALID_ARG;
    }
    memset(out, 0, sizeof(*out));
    out->size = sizeof(*out);
    out->snapshot.size = sizeof(out->snapshot);
    return DCC_OK;
}

static dcc_status_t dcc_app_wait_filter_prepare(
    const dcc_app_wait_filter_t *filter,
    dcc_app_wait_filter_t *out
) {
    memset(out, 0, sizeof(*out));
    out->size = sizeof(*out);
    if (filter == NULL) {
        return DCC_OK;
    }
    if (filter->size < sizeof(*filter) ||
        (filter->custom_id != NULL && filter->custom_id_prefix != NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = *filter;
    out->size = sizeof(*out);
    return DCC_OK;
}

static dcc_status_t dcc_app_wait_remaining_timeout(
    uint64_t started_ms,
    uint32_t timeout_ms,
    uint32_t *out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (timeout_ms == 0U) {
        *out = 0U;
        return DCC_OK;
    }

    uint64_t now_ms = dcc_app_wait_now_ms();
    uint64_t elapsed_ms = now_ms >= started_ms ? now_ms - started_ms : 0U;
    if (elapsed_ms >= timeout_ms) {
        *out = 0U;
        return DCC_ERR_TIMEOUT;
    }

    uint64_t remaining = (uint64_t)timeout_ms - elapsed_ms;
    *out = remaining > UINT32_MAX ? UINT32_MAX : (uint32_t)remaining;
    if (*out == 0U) {
        *out = 1U;
    }
    return DCC_OK;
}

static uint8_t dcc_app_wait_custom_id_prefix_matches(const char *value, const char *prefix) {
    if (value == NULL || prefix == NULL) {
        return 0U;
    }
    return strncmp(value, prefix, strlen(prefix)) == 0 ? 1U : 0U;
}

static uint8_t dcc_app_wait_interaction_matches(
    const dcc_interaction_t *interaction,
    const dcc_app_wait_filter_t *filter
) {
    if (interaction == NULL || filter == NULL) {
        return 0U;
    }
    if (filter->user_id != 0U && interaction->user_id != filter->user_id) {
        return 0U;
    }
    if (filter->channel_id != 0U && interaction->channel_id != filter->channel_id) {
        return 0U;
    }
    if (filter->guild_id != 0U && interaction->guild_id != filter->guild_id) {
        return 0U;
    }
    if (filter->custom_id != NULL &&
        (interaction->custom_id == NULL || strcmp(interaction->custom_id, filter->custom_id) != 0)) {
        return 0U;
    }
    if (filter->custom_id_prefix != NULL &&
        !dcc_app_wait_custom_id_prefix_matches(interaction->custom_id, filter->custom_id_prefix)) {
        return 0U;
    }
    return 1U;
}

static uint8_t dcc_app_wait_message_matches(
    const dcc_message_t *message,
    const dcc_app_wait_filter_t *filter
) {
    if (message == NULL || filter == NULL) {
        return 0U;
    }
    if (filter->user_id != 0U && message->author.id != filter->user_id) {
        return 0U;
    }
    if (filter->channel_id != 0U && message->channel_id != filter->channel_id) {
        return 0U;
    }
    if (filter->guild_id != 0U && message->guild_id != filter->guild_id) {
        return 0U;
    }
    return 1U;
}

dcc_status_t dcc_app_wait_for_interaction(
    dcc_app_t *app,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
) {
    dcc_status_t status = dcc_app_wait_result_prepare(out);
    if (status != DCC_OK) {
        return status;
    }
    dcc_client_t *client = dcc_app_client(app);
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_client_wait_for_interaction(
        client,
        type,
        timeout_ms,
        &out->snapshot,
        &out->interaction
    );
}

void dcc_app_wait_filter_init(dcc_app_wait_filter_t *filter) {
    if (filter == NULL) {
        return;
    }
    memset(filter, 0, sizeof(*filter));
    filter->size = sizeof(*filter);
}

dcc_status_t dcc_app_wait_filter_from_ctx(
    const dcc_ctx_t *ctx,
    dcc_app_wait_filter_t *out
) {
    if (ctx == NULL || out == NULL || dcc_ctx_interaction(ctx) == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_wait_filter_init(out);
    out->user_id = dcc_ctx_user_id(ctx);
    out->channel_id = dcc_ctx_channel_id(ctx);
    out->guild_id = dcc_ctx_guild_id(ctx);
    return DCC_OK;
}

dcc_status_t dcc_app_wait_for_interaction_filtered(
    dcc_app_t *app,
    dcc_event_type_t type,
    const dcc_app_wait_filter_t *filter,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
) {
    dcc_app_wait_filter_t effective_filter;
    dcc_status_t status = dcc_app_wait_filter_prepare(filter, &effective_filter);
    if (status != DCC_OK) {
        return status;
    }

    uint64_t started_ms = dcc_app_wait_now_ms();
    for (;;) {
        uint32_t remaining_ms = 0U;
        status = dcc_app_wait_remaining_timeout(started_ms, timeout_ms, &remaining_ms);
        if (status != DCC_OK) {
            return status;
        }
        status = dcc_app_wait_for_interaction(app, type, remaining_ms, out);
        if (status != DCC_OK) {
            return status;
        }
        if (dcc_app_wait_interaction_matches(out->interaction, &effective_filter)) {
            return DCC_OK;
        }
        dcc_app_wait_result_deinit(out);
    }
}

dcc_status_t dcc_app_wait_for_component(
    dcc_app_t *app,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
) {
    static const dcc_event_type_t types[] = {
        DCC_EVENT_BUTTON_CLICK,
        DCC_EVENT_SELECT_CLICK,
    };
    dcc_status_t status = dcc_app_wait_result_prepare(out);
    if (status != DCC_OK) {
        return status;
    }
    dcc_client_t *client = dcc_app_client(app);
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_client_wait_for_interaction_any(
        client,
        types,
        sizeof(types) / sizeof(types[0]),
        timeout_ms,
        &out->snapshot,
        &out->interaction
    );
}

dcc_status_t dcc_app_wait_for_component_filtered(
    dcc_app_t *app,
    const dcc_app_wait_filter_t *filter,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
) {
    static const dcc_event_type_t types[] = {
        DCC_EVENT_BUTTON_CLICK,
        DCC_EVENT_SELECT_CLICK,
    };
    dcc_app_wait_filter_t effective_filter;
    dcc_status_t status = dcc_app_wait_filter_prepare(filter, &effective_filter);
    if (status != DCC_OK) {
        return status;
    }

    uint64_t started_ms = dcc_app_wait_now_ms();
    for (;;) {
        uint32_t remaining_ms = 0U;
        status = dcc_app_wait_remaining_timeout(started_ms, timeout_ms, &remaining_ms);
        if (status != DCC_OK) {
            return status;
        }

        status = dcc_app_wait_result_prepare(out);
        if (status != DCC_OK) {
            return status;
        }
        dcc_client_t *client = dcc_app_client(app);
        if (client == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
        status = dcc_client_wait_for_interaction_any(
            client,
            types,
            sizeof(types) / sizeof(types[0]),
            remaining_ms,
            &out->snapshot,
            &out->interaction
        );
        if (status != DCC_OK) {
            return status;
        }
        if (dcc_app_wait_interaction_matches(out->interaction, &effective_filter)) {
            return DCC_OK;
        }
        dcc_app_wait_result_deinit(out);
    }
}

dcc_status_t dcc_app_wait_for_button(
    dcc_app_t *app,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
) {
    return dcc_app_wait_for_interaction(app, DCC_EVENT_BUTTON_CLICK, timeout_ms, out);
}

dcc_status_t dcc_app_wait_for_select(
    dcc_app_t *app,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
) {
    return dcc_app_wait_for_interaction(app, DCC_EVENT_SELECT_CLICK, timeout_ms, out);
}

dcc_status_t dcc_app_wait_for_modal(
    dcc_app_t *app,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
) {
    return dcc_app_wait_for_interaction(app, DCC_EVENT_FORM_SUBMIT, timeout_ms, out);
}

dcc_status_t dcc_app_wait_for_message(
    dcc_app_t *app,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
) {
    dcc_status_t status = dcc_app_wait_result_prepare(out);
    if (status != DCC_OK) {
        return status;
    }
    dcc_client_t *client = dcc_app_client(app);
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_client_wait_for_message(
        client,
        type,
        timeout_ms,
        &out->snapshot,
        &out->message
    );
}

dcc_status_t dcc_app_wait_for_message_filtered(
    dcc_app_t *app,
    dcc_event_type_t type,
    const dcc_app_wait_filter_t *filter,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
) {
    dcc_app_wait_filter_t effective_filter;
    dcc_status_t status = dcc_app_wait_filter_prepare(filter, &effective_filter);
    if (status != DCC_OK || effective_filter.custom_id != NULL ||
        effective_filter.custom_id_prefix != NULL) {
        return status == DCC_OK ? DCC_ERR_INVALID_ARG : status;
    }

    uint64_t started_ms = dcc_app_wait_now_ms();
    for (;;) {
        uint32_t remaining_ms = 0U;
        status = dcc_app_wait_remaining_timeout(started_ms, timeout_ms, &remaining_ms);
        if (status != DCC_OK) {
            return status;
        }
        status = dcc_app_wait_for_message(app, type, remaining_ms, out);
        if (status != DCC_OK) {
            return status;
        }
        if (dcc_app_wait_message_matches(out->message, &effective_filter)) {
            return DCC_OK;
        }
        dcc_app_wait_result_deinit(out);
    }
}

static dcc_status_t dcc_ctx_wait_filter(
    dcc_ctx_t *ctx,
    dcc_app_wait_filter_t *out
) {
    dcc_status_t status = dcc_app_wait_filter_from_ctx(ctx, out);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_ctx_app(ctx) != NULL ? DCC_OK : DCC_ERR_INVALID_ARG;
}

static dcc_status_t dcc_ctx_wait_for_interaction_filtered(
    dcc_ctx_t *ctx,
    dcc_event_type_t type,
    const char *custom_id,
    const char *custom_id_prefix,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
) {
    if (custom_id != NULL && custom_id_prefix != NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (custom_id_prefix != NULL && custom_id_prefix[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_app_wait_filter_t filter;
    dcc_status_t status = dcc_ctx_wait_filter(ctx, &filter);
    if (status != DCC_OK) {
        return status;
    }
    filter.custom_id = custom_id;
    filter.custom_id_prefix = custom_id_prefix;
    return dcc_app_wait_for_interaction_filtered(
        dcc_ctx_app(ctx),
        type,
        &filter,
        timeout_ms,
        out
    );
}

dcc_status_t dcc_ctx_wait_for_component(
    dcc_ctx_t *ctx,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
) {
    dcc_app_wait_filter_t filter;
    dcc_status_t status = dcc_ctx_wait_filter(ctx, &filter);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_app_wait_for_component_filtered(dcc_ctx_app(ctx), &filter, timeout_ms, out);
}

dcc_status_t dcc_ctx_wait_for_button(
    dcc_ctx_t *ctx,
    const char *custom_id,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
) {
    return dcc_ctx_wait_for_interaction_filtered(
        ctx,
        DCC_EVENT_BUTTON_CLICK,
        custom_id,
        NULL,
        timeout_ms,
        out
    );
}

dcc_status_t dcc_ctx_wait_for_button_prefix(
    dcc_ctx_t *ctx,
    const char *custom_id_prefix,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
) {
    return dcc_ctx_wait_for_interaction_filtered(
        ctx,
        DCC_EVENT_BUTTON_CLICK,
        NULL,
        custom_id_prefix,
        timeout_ms,
        out
    );
}

dcc_status_t dcc_ctx_wait_for_select(
    dcc_ctx_t *ctx,
    const char *custom_id,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
) {
    return dcc_ctx_wait_for_interaction_filtered(
        ctx,
        DCC_EVENT_SELECT_CLICK,
        custom_id,
        NULL,
        timeout_ms,
        out
    );
}

dcc_status_t dcc_ctx_wait_for_select_prefix(
    dcc_ctx_t *ctx,
    const char *custom_id_prefix,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
) {
    return dcc_ctx_wait_for_interaction_filtered(
        ctx,
        DCC_EVENT_SELECT_CLICK,
        NULL,
        custom_id_prefix,
        timeout_ms,
        out
    );
}

dcc_status_t dcc_ctx_wait_for_modal(
    dcc_ctx_t *ctx,
    const char *custom_id,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
) {
    return dcc_ctx_wait_for_interaction_filtered(
        ctx,
        DCC_EVENT_FORM_SUBMIT,
        custom_id,
        NULL,
        timeout_ms,
        out
    );
}

dcc_status_t dcc_ctx_wait_for_modal_prefix(
    dcc_ctx_t *ctx,
    const char *custom_id_prefix,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
) {
    return dcc_ctx_wait_for_interaction_filtered(
        ctx,
        DCC_EVENT_FORM_SUBMIT,
        NULL,
        custom_id_prefix,
        timeout_ms,
        out
    );
}

dcc_status_t dcc_ctx_wait_for_message(
    dcc_ctx_t *ctx,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
) {
    dcc_app_wait_filter_t filter;
    dcc_status_t status = dcc_ctx_wait_filter(ctx, &filter);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_app_wait_for_message_filtered(
        dcc_ctx_app(ctx),
        DCC_EVENT_MESSAGE_CREATE,
        &filter,
        timeout_ms,
        out
    );
}

dcc_event_type_t dcc_app_wait_result_type(const dcc_app_wait_result_t *result) {
    return result != NULL ? result->snapshot.type : (dcc_event_type_t)0;
}

const dcc_interaction_t *dcc_app_wait_result_interaction(
    const dcc_app_wait_result_t *result
) {
    return result != NULL ? result->interaction : NULL;
}

const dcc_message_t *dcc_app_wait_result_message(const dcc_app_wait_result_t *result) {
    return result != NULL ? result->message : NULL;
}

dcc_snowflake_t dcc_app_wait_result_id(const dcc_app_wait_result_t *result) {
    const dcc_interaction_t *interaction = dcc_app_wait_result_interaction(result);
    const dcc_message_t *message = dcc_app_wait_result_message(result);
    if (interaction != NULL) {
        return interaction->id;
    }
    return message != NULL ? message->id : 0U;
}

dcc_snowflake_t dcc_app_wait_result_user_id(const dcc_app_wait_result_t *result) {
    const dcc_interaction_t *interaction = dcc_app_wait_result_interaction(result);
    const dcc_message_t *message = dcc_app_wait_result_message(result);
    if (interaction != NULL) {
        return interaction->user_id;
    }
    return message != NULL ? message->author.id : 0U;
}

dcc_snowflake_t dcc_app_wait_result_channel_id(const dcc_app_wait_result_t *result) {
    const dcc_interaction_t *interaction = dcc_app_wait_result_interaction(result);
    const dcc_message_t *message = dcc_app_wait_result_message(result);
    if (interaction != NULL) {
        return interaction->channel_id;
    }
    return message != NULL ? message->channel_id : 0U;
}

dcc_snowflake_t dcc_app_wait_result_guild_id(const dcc_app_wait_result_t *result) {
    const dcc_interaction_t *interaction = dcc_app_wait_result_interaction(result);
    const dcc_message_t *message = dcc_app_wait_result_message(result);
    if (interaction != NULL) {
        return interaction->guild_id;
    }
    return message != NULL ? message->guild_id : 0U;
}

const char *dcc_app_wait_result_custom_id(const dcc_app_wait_result_t *result) {
    const dcc_interaction_t *interaction = dcc_app_wait_result_interaction(result);
    return interaction != NULL ? interaction->custom_id : NULL;
}

const char *dcc_app_wait_result_message_content(
    const dcc_app_wait_result_t *result,
    const char *fallback
) {
    const dcc_message_t *message = dcc_app_wait_result_message(result);
    return message != NULL && message->content != NULL ? message->content : fallback;
}

size_t dcc_app_wait_result_value_count(const dcc_app_wait_result_t *result) {
    const dcc_interaction_t *interaction = dcc_app_wait_result_interaction(result);
    return interaction != NULL && interaction->values != NULL ? interaction->values_count : 0U;
}

const char *dcc_app_wait_result_value(
    const dcc_app_wait_result_t *result,
    size_t index
) {
    const dcc_interaction_t *interaction = dcc_app_wait_result_interaction(result);
    if (interaction == NULL || interaction->values == NULL || index >= interaction->values_count) {
        return NULL;
    }
    return interaction->values[index];
}

const dcc_interaction_form_field_t *dcc_app_wait_result_form_field(
    const dcc_app_wait_result_t *result,
    const char *custom_id
) {
    const dcc_interaction_t *interaction = dcc_app_wait_result_interaction(result);
    return dcc_interaction_form_field_by_custom_id(interaction, custom_id);
}

const char *dcc_app_wait_result_form_string(
    const dcc_app_wait_result_t *result,
    const char *custom_id,
    const char *fallback
) {
    const dcc_interaction_t *interaction = dcc_app_wait_result_interaction(result);
    const char *value = dcc_interaction_form_value(interaction, custom_id);
    return value != NULL ? value : fallback;
}

uint8_t dcc_app_wait_result_form_bool(
    const dcc_app_wait_result_t *result,
    const char *custom_id,
    uint8_t fallback
) {
    const dcc_interaction_t *interaction = dcc_app_wait_result_interaction(result);
    uint8_t value = 0U;
    if (!dcc_interaction_form_boolean(interaction, custom_id, &value)) {
        return fallback ? 1U : 0U;
    }
    return value ? 1U : 0U;
}

size_t dcc_app_wait_result_form_value_count(
    const dcc_app_wait_result_t *result,
    const char *custom_id
) {
    const dcc_interaction_form_field_t *field =
        dcc_app_wait_result_form_field(result, custom_id);
    return field != NULL && field->values != NULL ? field->values_count : 0U;
}

const char *dcc_app_wait_result_form_value(
    const dcc_app_wait_result_t *result,
    const char *custom_id,
    size_t index
) {
    const dcc_interaction_form_field_t *field =
        dcc_app_wait_result_form_field(result, custom_id);
    if (field == NULL || field->values == NULL || index >= field->values_count) {
        return NULL;
    }
    return field->values[index];
}

void dcc_app_wait_result_deinit(dcc_app_wait_result_t *result) {
    if (result == NULL) {
        return;
    }
    dcc_interaction_free(result->interaction);
    dcc_message_free(result->message);
    result->interaction = NULL;
    result->message = NULL;
}
