#include "internal/objects/dcc_component_builder_internal.h"

static int dcc_component_type_valid(dcc_component_type_t type) {
    switch (type) {
        case DCC_COMPONENT_ACTION_ROW:
        case DCC_COMPONENT_BUTTON:
        case DCC_COMPONENT_STRING_SELECT:
        case DCC_COMPONENT_TEXT_INPUT:
        case DCC_COMPONENT_USER_SELECT:
        case DCC_COMPONENT_ROLE_SELECT:
        case DCC_COMPONENT_MENTIONABLE_SELECT:
        case DCC_COMPONENT_CHANNEL_SELECT:
            return 1;
        default:
            return 0;
    }
}

int dcc_button_style_valid(dcc_button_style_t style) {
    switch (style) {
        case DCC_BUTTON_PRIMARY:
        case DCC_BUTTON_SECONDARY:
        case DCC_BUTTON_SUCCESS:
        case DCC_BUTTON_DANGER:
        case DCC_BUTTON_LINK:
        case DCC_BUTTON_PREMIUM:
            return 1;
        default:
            return 0;
    }
}

int dcc_text_input_style_valid(dcc_text_input_style_t style) {
    switch (style) {
        case DCC_TEXT_INPUT_SHORT:
        case DCC_TEXT_INPUT_PARAGRAPH:
            return 1;
        default:
            return 0;
    }
}

dcc_status_t dcc_component_validate_emoji(const dcc_component_emoji_t *emoji) {
    if (emoji == NULL || (!emoji->has_id && emoji->name == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

dcc_status_t dcc_component_validate(const dcc_component_builder_t *builder) {
    if (builder == NULL || !dcc_component_type_valid(builder->type)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (builder->has_style && !dcc_button_style_valid(builder->style)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (builder->has_text_input_style && !dcc_text_input_style_valid(builder->text_input_style)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (builder->has_emoji && dcc_component_validate_emoji(&builder->emoji) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    if (builder->children_count != 0 && builder->children == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (builder->options_count != 0 && builder->options == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    for (size_t i = 0; i < builder->options_count; ++i) {
        const dcc_select_option_t *option = &builder->options[i];
        if (option->label == NULL || option->value == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
        if (option->has_emoji && dcc_component_validate_emoji(&option->emoji) != DCC_OK) {
            return DCC_ERR_INVALID_ARG;
        }
    }

    for (size_t i = 0; i < builder->children_count; ++i) {
        if (builder->type == DCC_COMPONENT_ACTION_ROW &&
            builder->children[i].type == DCC_COMPONENT_ACTION_ROW) {
            return DCC_ERR_INVALID_ARG;
        }
        dcc_status_t status = dcc_component_validate(&builder->children[i]);
        if (status != DCC_OK) {
            return status;
        }
    }

    return DCC_OK;
}

