#include <dcc/app.h>

#include <stddef.h>
#include <stdint.h>

#define DCC_CTX_FORM_FIELD_BINDING_MIN_SIZE \
    (offsetof(dcc_ctx_form_field_binding_t, fallback_values_count) + \
     sizeof(((dcc_ctx_form_field_binding_t *)0)->fallback_values_count))

static dcc_status_t dcc_ctx_bind_missing_form(const dcc_ctx_form_binding_t *binding) {
    if (binding->required) {
        return DCC_ERR_NOT_FOUND;
    }

    switch (binding->type) {
        case DCC_CTX_FORM_BIND_STRING:
            *(const char **)binding->out = binding->fallback_string;
            return DCC_OK;
        case DCC_CTX_FORM_BIND_BOOLEAN:
            *(uint8_t *)binding->out = binding->fallback_boolean ? 1U : 0U;
            return DCC_OK;
        case DCC_CTX_FORM_BIND_VALUES:
            *(const char ***)binding->out = binding->fallback_values;
            if (binding->out_count != NULL) {
                *binding->out_count = binding->fallback_values_count;
            }
            return DCC_OK;
    }

    return DCC_ERR_INVALID_ARG;
}

static dcc_status_t dcc_ctx_bind_one_form(
    const dcc_ctx_t *ctx,
    const dcc_ctx_form_binding_t *binding
) {
    if (ctx == NULL || binding == NULL || binding->custom_id == NULL ||
        binding->custom_id[0] == '\0' || binding->out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    const dcc_interaction_form_field_t *field =
        dcc_ctx_form_field(ctx, binding->custom_id);
    if (field == NULL) {
        return dcc_ctx_bind_missing_form(binding);
    }

    switch (binding->type) {
        case DCC_CTX_FORM_BIND_STRING:
            if (field->value_type != DCC_INTERACTION_FORM_VALUE_STRING ||
                field->value == NULL) {
                return DCC_ERR_STATE;
            }
            *(const char **)binding->out = field->value;
            return DCC_OK;

        case DCC_CTX_FORM_BIND_BOOLEAN:
            if (field->value_type != DCC_INTERACTION_FORM_VALUE_BOOLEAN) {
                return DCC_ERR_STATE;
            }
            *(uint8_t *)binding->out = field->boolean_value ? 1U : 0U;
            return DCC_OK;

        case DCC_CTX_FORM_BIND_VALUES:
            if (field->value_type != DCC_INTERACTION_FORM_VALUE_VALUES) {
                return DCC_ERR_STATE;
            }
            *(const char ***)binding->out = field->values;
            if (binding->out_count != NULL) {
                *binding->out_count = field->values_count;
            }
            return DCC_OK;
    }

    return DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_ctx_bind_form(
    const dcc_ctx_t *ctx,
    const dcc_ctx_form_binding_t *bindings,
    size_t binding_count
) {
    if (ctx == NULL || (bindings == NULL && binding_count != 0U)) {
        return DCC_ERR_INVALID_ARG;
    }

    for (size_t i = 0; i < binding_count; ++i) {
        dcc_status_t status = dcc_ctx_bind_one_form(ctx, &bindings[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}

static dcc_status_t dcc_ctx_bind_one_form_field(
    const dcc_ctx_t *ctx,
    void *base,
    const dcc_ctx_form_field_binding_t *field
) {
    if (ctx == NULL || base == NULL || field == NULL ||
        field->size < DCC_CTX_FORM_FIELD_BINDING_MIN_SIZE) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_ctx_form_binding_t binding = {
        .size = sizeof(dcc_ctx_form_binding_t),
        .custom_id = field->custom_id,
        .type = field->type,
        .out = (void *)((unsigned char *)base + field->field_offset),
        .out_count = field->has_count_offset
            ? (size_t *)((unsigned char *)base + field->count_offset)
            : NULL,
        .required = field->required,
        .fallback_string = field->fallback_string,
        .fallback_boolean = field->fallback_boolean,
        .fallback_values = field->fallback_values,
        .fallback_values_count = field->fallback_values_count,
    };
    return dcc_ctx_bind_one_form(ctx, &binding);
}

dcc_status_t dcc_ctx_bind_form_fields(
    const dcc_ctx_t *ctx,
    void *base,
    const dcc_ctx_form_field_binding_t *bindings,
    size_t binding_count
) {
    if (ctx == NULL || base == NULL || (bindings == NULL && binding_count != 0U)) {
        return DCC_ERR_INVALID_ARG;
    }

    for (size_t i = 0U; i < binding_count; ++i) {
        dcc_status_t status = dcc_ctx_bind_one_form_field(ctx, base, &bindings[i]);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}
