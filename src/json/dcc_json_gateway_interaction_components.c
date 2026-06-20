#include "internal/json/dcc_json_internal.h"

#include <string.h>

#define DCC_JSON_INTERACTION_COMPONENT_MAX_DEPTH 16U

typedef struct dcc_json_interaction_component_state {
    uint32_t type;
    int has_type;
    char custom_id[DCC_JSON_GATEWAY_INTERACTION_FORM_FIELD_CUSTOM_ID_CAP];
    int has_custom_id;
    char value[DCC_JSON_GATEWAY_INTERACTION_FORM_FIELD_VALUE_CAP];
    int has_value;
    int value_is_boolean;
    uint8_t boolean_value;
    const char *values[DCC_JSON_GATEWAY_INTERACTION_FORM_FIELD_VALUE_LIST_CAP];
    char value_storage[DCC_JSON_GATEWAY_INTERACTION_FORM_FIELD_VALUE_LIST_CAP]
        [DCC_JSON_GATEWAY_INTERACTION_FORM_FIELD_VALUE_LIST_ITEM_CAP];
    size_t values_count;
    uint8_t values_truncated;
    int has_values;
} dcc_json_interaction_component_state_t;

static dcc_status_t dcc_json_gateway_parse_interaction_component_object(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    unsigned depth
);

static dcc_status_t dcc_json_gateway_parse_interaction_component_array(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    unsigned depth
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '[') {
        return dcc_json_skip_value(parser);
    }
    if (depth > DCC_JSON_INTERACTION_COMPONENT_MAX_DEPTH) {
        return DCC_ERR_JSON;
    }

    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == ']') {
        parser->cur++;
        return DCC_OK;
    }

    for (;;) {
        dcc_status_t status = DCC_OK;
        dcc_json_skip_ws(parser);
        if (parser->cur < parser->end && *parser->cur == '{') {
            status = dcc_json_gateway_parse_interaction_component_object(parser, out, depth + 1U);
        } else {
            status = dcc_json_skip_value(parser);
        }
        if (status != DCC_OK) {
            return status;
        }

        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == ']') {
            parser->cur++;
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
    }
}

static dcc_status_t dcc_json_gateway_parse_component_value(
    dcc_json_parser_t *parser,
    dcc_json_interaction_component_state_t *state
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur == 't' || *parser->cur == 'f') {
        int value = 0;
        dcc_status_t status = dcc_json_parse_bool_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        state->has_value = 1;
        state->value_is_boolean = 1;
        state->boolean_value = value ? 1U : 0U;
        state->value[0] = '\0';
        return DCC_OK;
    }
    if (*parser->cur == '"' || *parser->cur == 'n' ||
        *parser->cur == '-' || (*parser->cur >= '0' && *parser->cur <= '9')) {
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_string_or_number_buffer(
            parser,
            state->value,
            sizeof(state->value),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        state->has_value = has_value;
        state->value_is_boolean = 0;
        return DCC_OK;
    }
    return dcc_json_skip_value(parser);
}

static dcc_status_t dcc_json_gateway_parse_component_values(
    dcc_json_parser_t *parser,
    dcc_json_interaction_component_state_t *state
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    state->has_values = 1;
    if (*parser->cur != '[') {
        return dcc_json_skip_value(parser);
    }

    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == ']') {
        parser->cur++;
        return DCC_OK;
    }

    size_t index = 0;
    for (;;) {
        if (index < DCC_JSON_GATEWAY_INTERACTION_FORM_FIELD_VALUE_LIST_CAP) {
            int has_value = 0;
            dcc_status_t status = dcc_json_parse_nullable_string_buffer(
                parser,
                state->value_storage[index],
                sizeof(state->value_storage[index]),
                &has_value
            );
            if (status != DCC_OK) {
                return status;
            }
            state->values[index] = has_value ? state->value_storage[index] : NULL;
            state->values_count = index + 1U;
        } else {
            dcc_status_t status = dcc_json_skip_value(parser);
            if (status != DCC_OK) {
                return status;
            }
            state->values_truncated = 1;
        }
        index++;

        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == ']') {
            parser->cur++;
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}

static void dcc_json_gateway_store_interaction_component_field(
    dcc_json_gateway_payload_t *out,
    const dcc_json_interaction_component_state_t *state
) {
    if (!state->has_custom_id) {
        return;
    }
    out->interaction.form_fields = out->interaction_form_fields;
    if (out->interaction.form_fields_count >= DCC_JSON_GATEWAY_INTERACTION_FORM_FIELD_CAP) {
        out->interaction.form_fields_truncated = 1;
        return;
    }

    size_t index = out->interaction.form_fields_count++;
    dcc_interaction_form_field_t *field = &out->interaction_form_fields[index];
    memset(field, 0, sizeof(*field));

    size_t custom_id_len = strlen(state->custom_id);
    memcpy(out->interaction_form_field_custom_ids[index], state->custom_id, custom_id_len + 1U);
    field->custom_id = out->interaction_form_field_custom_ids[index];
    field->component_type = state->has_type ? state->type : 0U;

    if (state->has_values) {
        field->value_type = DCC_INTERACTION_FORM_VALUE_VALUES;
        field->values = out->interaction_form_field_value_lists[index];
        field->values_count = state->values_count;
        field->values_truncated = state->values_truncated;
        for (size_t i = 0; i < state->values_count; ++i) {
            if (state->values[i] == NULL) {
                field->values[i] = NULL;
                continue;
            }
            size_t value_len = strlen(state->values[i]);
            memcpy(
                out->interaction_form_field_value_storage[index][i],
                state->values[i],
                value_len + 1U
            );
            field->values[i] = out->interaction_form_field_value_storage[index][i];
        }
    } else if (state->has_value) {
        if (state->value_is_boolean) {
            field->value_type = DCC_INTERACTION_FORM_VALUE_BOOLEAN;
            field->boolean_value = state->boolean_value;
        } else {
            size_t value_len = strlen(state->value);
            memcpy(out->interaction_form_field_values[index], state->value, value_len + 1U);
            field->value = out->interaction_form_field_values[index];
            field->value_type = DCC_INTERACTION_FORM_VALUE_STRING;
        }
    }
    out->has_interaction = 1;
}

static dcc_status_t dcc_json_gateway_parse_interaction_component_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_interaction_component_state_t *state,
    dcc_json_key_id_t key_id,
    unsigned depth
) {
    switch (key_id) {
        case DCC_JSON_KEY_TYPE: {
            dcc_status_t status = dcc_json_parse_u32_direct(parser, &state->type);
            if (status == DCC_OK) {
                state->has_type = 1;
            }
            return status;
        }
        case DCC_JSON_KEY_CUSTOM_ID: {
            int has_value = 0;
            dcc_status_t status = dcc_json_parse_nullable_string_buffer(
                parser,
                state->custom_id,
                sizeof(state->custom_id),
                &has_value
            );
            if (status == DCC_OK) {
                state->has_custom_id = has_value;
            }
            return status;
        }
        case DCC_JSON_KEY_VALUE:
            return dcc_json_gateway_parse_component_value(parser, state);
        case DCC_JSON_KEY_VALUES:
            return dcc_json_gateway_parse_component_values(parser, state);
        case DCC_JSON_KEY_COMPONENTS:
            return dcc_json_gateway_parse_interaction_component_array(parser, out, depth + 1U);
        case DCC_JSON_KEY_COMPONENT:
            return dcc_json_gateway_parse_interaction_component_object(parser, out, depth + 1U);
        default:
            return dcc_json_skip_value(parser);
    }
}

static dcc_status_t dcc_json_gateway_parse_interaction_component_object(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    unsigned depth
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '{') {
        return dcc_json_skip_value(parser);
    }
    if (depth > DCC_JSON_INTERACTION_COMPONENT_MAX_DEPTH) {
        return DCC_ERR_JSON;
    }

    dcc_json_interaction_component_state_t state;
    memset(&state, 0, sizeof(state));

    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == '}') {
        parser->cur++;
        return DCC_OK;
    }

    for (;;) {
        dcc_json_key_id_t key_id = DCC_JSON_KEY_UNKNOWN;
        if (dcc_json_parse_gateway_key(parser, &key_id) != 0) {
            return DCC_ERR_JSON;
        }
        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end || *parser->cur != ':') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);

        dcc_status_t status = dcc_json_gateway_parse_interaction_component_field(
            parser,
            out,
            &state,
            key_id,
            depth
        );
        if (status != DCC_OK) {
            return status;
        }

        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == '}') {
            parser->cur++;
            dcc_json_gateway_store_interaction_component_field(out, &state);
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}

dcc_status_t dcc_json_gateway_parse_interaction_components(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
) {
    if (parser == NULL || out == NULL) {
        return DCC_ERR_JSON;
    }
    out->interaction.form_fields = out->interaction_form_fields;
    out->interaction.form_fields_count = 0;
    out->interaction.form_fields_truncated = 0;
    return dcc_json_gateway_parse_interaction_component_array(parser, out, 0U);
}
