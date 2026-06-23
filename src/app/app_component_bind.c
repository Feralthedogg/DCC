#include <dcc/app.h>

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define DCC_CTX_COMPONENT_FIELD_BINDING_MIN_SIZE \
    (offsetof(dcc_ctx_component_field_binding_t, fallback_component_type) + \
     sizeof(((dcc_ctx_component_field_binding_t *)0)->fallback_component_type))

static uint8_t dcc_ctx_component_prefix_matches(const char *value, const char *prefix) {
    if (value == NULL || prefix == NULL) {
        return 0U;
    }
    return strncmp(value, prefix, strlen(prefix)) == 0 ? 1U : 0U;
}

static dcc_status_t dcc_ctx_bind_missing_component(
    const dcc_ctx_component_binding_t *binding
) {
    if (binding->required) {
        return DCC_ERR_NOT_FOUND;
    }

    switch (binding->type) {
        case DCC_CTX_COMPONENT_BIND_CUSTOM_ID:
        case DCC_CTX_COMPONENT_BIND_CUSTOM_ID_SUFFIX:
        case DCC_CTX_COMPONENT_BIND_FIRST_VALUE:
            *(const char **)binding->out = binding->fallback_string;
            return DCC_OK;

        case DCC_CTX_COMPONENT_BIND_VALUES:
            *(const char ***)binding->out = binding->fallback_values;
            if (binding->out_count != NULL) {
                *binding->out_count = binding->fallback_values_count;
            }
            return DCC_OK;

        case DCC_CTX_COMPONENT_BIND_COMPONENT_TYPE:
            *(uint32_t *)binding->out = binding->fallback_component_type;
            return DCC_OK;
    }

    return DCC_ERR_INVALID_ARG;
}

static dcc_status_t dcc_ctx_bind_one_component(
    const dcc_ctx_t *ctx,
    const dcc_ctx_component_binding_t *binding
) {
    if (ctx == NULL || binding == NULL || binding->out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    const dcc_interaction_t *interaction = dcc_ctx_interaction(ctx);
    if (interaction == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    switch (binding->type) {
        case DCC_CTX_COMPONENT_BIND_CUSTOM_ID:
            if (interaction->custom_id == NULL) {
                return dcc_ctx_bind_missing_component(binding);
            }
            *(const char **)binding->out = interaction->custom_id;
            return DCC_OK;

        case DCC_CTX_COMPONENT_BIND_CUSTOM_ID_SUFFIX:
            if (interaction->custom_id == NULL) {
                return dcc_ctx_bind_missing_component(binding);
            }
            if (binding->custom_id_prefix == NULL || binding->custom_id_prefix[0] == '\0') {
                *(const char **)binding->out = interaction->custom_id;
                return DCC_OK;
            }
            if (!dcc_ctx_component_prefix_matches(interaction->custom_id, binding->custom_id_prefix)) {
                return dcc_ctx_bind_missing_component(binding);
            }
            *(const char **)binding->out = interaction->custom_id + strlen(binding->custom_id_prefix);
            return DCC_OK;

        case DCC_CTX_COMPONENT_BIND_VALUES:
            if (interaction->values_count == 0U || interaction->values == NULL) {
                return dcc_ctx_bind_missing_component(binding);
            }
            *(const char ***)binding->out = interaction->values;
            if (binding->out_count != NULL) {
                *binding->out_count = interaction->values_count;
            }
            return DCC_OK;

        case DCC_CTX_COMPONENT_BIND_FIRST_VALUE:
            if (interaction->values_count == 0U ||
                interaction->values == NULL ||
                interaction->values[0] == NULL) {
                return dcc_ctx_bind_missing_component(binding);
            }
            *(const char **)binding->out = interaction->values[0];
            return DCC_OK;

        case DCC_CTX_COMPONENT_BIND_COMPONENT_TYPE:
            *(uint32_t *)binding->out = interaction->component_type;
            return DCC_OK;
    }

    return DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_ctx_bind_component(
    const dcc_ctx_t *ctx,
    const dcc_ctx_component_binding_t *bindings,
    size_t binding_count
) {
    if (ctx == NULL || (bindings == NULL && binding_count != 0U)) {
        return DCC_ERR_INVALID_ARG;
    }

    for (size_t i = 0U; i < binding_count; ++i) {
        dcc_status_t status = dcc_ctx_bind_one_component(ctx, &bindings[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}

static dcc_status_t dcc_ctx_bind_one_component_field(
    const dcc_ctx_t *ctx,
    void *base,
    const dcc_ctx_component_field_binding_t *field
) {
    if (ctx == NULL || base == NULL || field == NULL ||
        field->size < DCC_CTX_COMPONENT_FIELD_BINDING_MIN_SIZE) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_ctx_component_binding_t binding = {
        .size = sizeof(dcc_ctx_component_binding_t),
        .type = field->type,
        .custom_id_prefix = field->custom_id_prefix,
        .out = (void *)((unsigned char *)base + field->field_offset),
        .out_count = field->has_count_offset
            ? (size_t *)((unsigned char *)base + field->count_offset)
            : NULL,
        .required = field->required,
        .fallback_string = field->fallback_string,
        .fallback_values = field->fallback_values,
        .fallback_values_count = field->fallback_values_count,
        .fallback_component_type = field->fallback_component_type,
    };
    return dcc_ctx_bind_one_component(ctx, &binding);
}

dcc_status_t dcc_ctx_bind_component_fields(
    const dcc_ctx_t *ctx,
    void *base,
    const dcc_ctx_component_field_binding_t *bindings,
    size_t binding_count
) {
    if (ctx == NULL || base == NULL || (bindings == NULL && binding_count != 0U)) {
        return DCC_ERR_INVALID_ARG;
    }

    for (size_t i = 0U; i < binding_count; ++i) {
        dcc_status_t status = dcc_ctx_bind_one_component_field(ctx, base, &bindings[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}
