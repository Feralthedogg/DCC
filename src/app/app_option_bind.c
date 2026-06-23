#include <dcc/app.h>

#include <stddef.h>
#include <stdint.h>

#define DCC_CTX_OPTION_FIELD_BINDING_MIN_SIZE \
    (offsetof(dcc_ctx_option_field_binding_t, fallback_snowflake) + \
     sizeof(((dcc_ctx_option_field_binding_t *)0)->fallback_snowflake))

static dcc_status_t dcc_ctx_bind_missing_option(const dcc_ctx_option_binding_t *binding) {
    if (binding->required) {
        return DCC_ERR_NOT_FOUND;
    }

    switch (binding->type) {
        case DCC_CTX_OPTION_BIND_STRING:
            *(const char **)binding->out = binding->fallback_string;
            return DCC_OK;
        case DCC_CTX_OPTION_BIND_INTEGER:
            *(int64_t *)binding->out = binding->fallback_integer;
            return DCC_OK;
        case DCC_CTX_OPTION_BIND_NUMBER:
            *(double *)binding->out = binding->fallback_number;
            return DCC_OK;
        case DCC_CTX_OPTION_BIND_BOOLEAN:
            *(uint8_t *)binding->out = binding->fallback_boolean ? 1U : 0U;
            return DCC_OK;
        case DCC_CTX_OPTION_BIND_SNOWFLAKE:
            *(dcc_snowflake_t *)binding->out = binding->fallback_snowflake;
            return DCC_OK;
        case DCC_CTX_OPTION_BIND_USER:
            *(const dcc_user_t **)binding->out = NULL;
            return DCC_OK;
        case DCC_CTX_OPTION_BIND_MEMBER:
            *(const dcc_member_t **)binding->out = NULL;
            return DCC_OK;
        case DCC_CTX_OPTION_BIND_ROLE:
            *(const dcc_role_t **)binding->out = NULL;
            return DCC_OK;
        case DCC_CTX_OPTION_BIND_CHANNEL:
            *(const dcc_channel_t **)binding->out = NULL;
            return DCC_OK;
        case DCC_CTX_OPTION_BIND_MESSAGE:
            *(const dcc_message_t **)binding->out = NULL;
            return DCC_OK;
        case DCC_CTX_OPTION_BIND_ATTACHMENT:
            *(const dcc_attachment_t **)binding->out = NULL;
            return DCC_OK;
    }

    return DCC_ERR_INVALID_ARG;
}

static dcc_status_t dcc_ctx_bind_option_snowflake_object(
    const dcc_ctx_t *ctx,
    const dcc_interaction_option_t *option,
    const dcc_ctx_option_binding_t *binding
) {
    if (option->value_type != DCC_INTERACTION_OPTION_VALUE_SNOWFLAKE) {
        return DCC_ERR_STATE;
    }

    switch (binding->type) {
        case DCC_CTX_OPTION_BIND_USER: {
            const dcc_user_t *value = dcc_ctx_resolved_user(ctx, option->snowflake_value);
            if (value == NULL) {
                return DCC_ERR_NOT_FOUND;
            }
            *(const dcc_user_t **)binding->out = value;
            return DCC_OK;
        }

        case DCC_CTX_OPTION_BIND_MEMBER: {
            const dcc_member_t *value = dcc_ctx_resolved_member(ctx, option->snowflake_value);
            if (value == NULL) {
                return DCC_ERR_NOT_FOUND;
            }
            *(const dcc_member_t **)binding->out = value;
            return DCC_OK;
        }

        case DCC_CTX_OPTION_BIND_ROLE: {
            const dcc_role_t *value = dcc_ctx_resolved_role(ctx, option->snowflake_value);
            if (value == NULL) {
                return DCC_ERR_NOT_FOUND;
            }
            *(const dcc_role_t **)binding->out = value;
            return DCC_OK;
        }

        case DCC_CTX_OPTION_BIND_CHANNEL: {
            const dcc_channel_t *value = dcc_ctx_resolved_channel(ctx, option->snowflake_value);
            if (value == NULL) {
                return DCC_ERR_NOT_FOUND;
            }
            *(const dcc_channel_t **)binding->out = value;
            return DCC_OK;
        }

        case DCC_CTX_OPTION_BIND_MESSAGE: {
            const dcc_message_t *value = dcc_ctx_resolved_message(ctx, option->snowflake_value);
            if (value == NULL) {
                return DCC_ERR_NOT_FOUND;
            }
            *(const dcc_message_t **)binding->out = value;
            return DCC_OK;
        }

        case DCC_CTX_OPTION_BIND_ATTACHMENT: {
            const dcc_attachment_t *value =
                dcc_ctx_resolved_attachment(ctx, option->snowflake_value);
            if (value == NULL) {
                return DCC_ERR_NOT_FOUND;
            }
            *(const dcc_attachment_t **)binding->out = value;
            return DCC_OK;
        }

        default:
            return DCC_ERR_INVALID_ARG;
    }
}

static dcc_status_t dcc_ctx_bind_one_option(
    const dcc_ctx_t *ctx,
    const dcc_ctx_option_binding_t *binding
) {
    if (ctx == NULL || binding == NULL || binding->name == NULL ||
        binding->name[0] == '\0' || binding->out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    const dcc_interaction_option_t *option = dcc_ctx_option(ctx, binding->name);
    if (option == NULL) {
        return dcc_ctx_bind_missing_option(binding);
    }

    switch (binding->type) {
        case DCC_CTX_OPTION_BIND_STRING:
            if (option->value_type != DCC_INTERACTION_OPTION_VALUE_STRING ||
                option->string_value == NULL) {
                return DCC_ERR_STATE;
            }
            *(const char **)binding->out = option->string_value;
            return DCC_OK;

        case DCC_CTX_OPTION_BIND_INTEGER:
            if (option->value_type != DCC_INTERACTION_OPTION_VALUE_INTEGER) {
                return DCC_ERR_STATE;
            }
            *(int64_t *)binding->out = option->integer_value;
            return DCC_OK;

        case DCC_CTX_OPTION_BIND_NUMBER:
            if (option->value_type != DCC_INTERACTION_OPTION_VALUE_NUMBER) {
                return DCC_ERR_STATE;
            }
            *(double *)binding->out = option->number_value;
            return DCC_OK;

        case DCC_CTX_OPTION_BIND_BOOLEAN:
            if (option->value_type != DCC_INTERACTION_OPTION_VALUE_BOOLEAN) {
                return DCC_ERR_STATE;
            }
            *(uint8_t *)binding->out = option->boolean_value ? 1U : 0U;
            return DCC_OK;

        case DCC_CTX_OPTION_BIND_SNOWFLAKE:
            if (option->value_type != DCC_INTERACTION_OPTION_VALUE_SNOWFLAKE) {
                return DCC_ERR_STATE;
            }
            *(dcc_snowflake_t *)binding->out = option->snowflake_value;
            return DCC_OK;

        case DCC_CTX_OPTION_BIND_USER:
        case DCC_CTX_OPTION_BIND_MEMBER:
        case DCC_CTX_OPTION_BIND_ROLE:
        case DCC_CTX_OPTION_BIND_CHANNEL:
        case DCC_CTX_OPTION_BIND_MESSAGE:
        case DCC_CTX_OPTION_BIND_ATTACHMENT:
            return dcc_ctx_bind_option_snowflake_object(ctx, option, binding);
    }

    return DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_ctx_bind_options(
    const dcc_ctx_t *ctx,
    const dcc_ctx_option_binding_t *bindings,
    size_t binding_count
) {
    if (ctx == NULL || (bindings == NULL && binding_count != 0U)) {
        return DCC_ERR_INVALID_ARG;
    }

    for (size_t i = 0; i < binding_count; ++i) {
        dcc_status_t status = dcc_ctx_bind_one_option(ctx, &bindings[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}

static dcc_status_t dcc_ctx_bind_one_option_field(
    const dcc_ctx_t *ctx,
    void *base,
    const dcc_ctx_option_field_binding_t *field
) {
    if (ctx == NULL || base == NULL || field == NULL ||
        field->size < DCC_CTX_OPTION_FIELD_BINDING_MIN_SIZE) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_ctx_option_binding_t binding = {
        .size = sizeof(dcc_ctx_option_binding_t),
        .name = field->name,
        .type = field->type,
        .out = (void *)((unsigned char *)base + field->field_offset),
        .required = field->required,
        .fallback_string = field->fallback_string,
        .fallback_integer = field->fallback_integer,
        .fallback_number = field->fallback_number,
        .fallback_boolean = field->fallback_boolean,
        .fallback_snowflake = field->fallback_snowflake,
    };
    return dcc_ctx_bind_one_option(ctx, &binding);
}

dcc_status_t dcc_ctx_bind_option_fields(
    const dcc_ctx_t *ctx,
    void *base,
    const dcc_ctx_option_field_binding_t *bindings,
    size_t binding_count
) {
    if (ctx == NULL || base == NULL || (bindings == NULL && binding_count != 0U)) {
        return DCC_ERR_INVALID_ARG;
    }

    for (size_t i = 0U; i < binding_count; ++i) {
        dcc_status_t status = dcc_ctx_bind_one_option_field(ctx, base, &bindings[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}
