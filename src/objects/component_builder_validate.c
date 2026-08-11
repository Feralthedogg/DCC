#include "internal/objects/dcc_component_builder_internal.h"

#include <stdint.h>

#define DCC_COMPONENT_MAX_ACTION_ROWS 5U
#define DCC_COMPONENT_MAX_ROW_CHILDREN 5U
#define DCC_COMPONENT_MAX_SELECT_OPTIONS 25U
#define DCC_COMPONENT_MAX_TREE_ITEMS \
    (DCC_COMPONENT_MAX_ACTION_ROWS * (DCC_COMPONENT_MAX_ROW_CHILDREN + 1U))

static int dcc_component_array_span_valid(
    const void *values,
    size_t count,
    size_t item_size
) {
    if (count == 0U) {
        return 1;
    }
    if (values == NULL || item_size == 0U || count > SIZE_MAX / item_size) {
        return 0;
    }
    size_t span = count * item_size;
    uintptr_t address = (uintptr_t)values;
    return address <= UINTPTR_MAX - (span - 1U);
}

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

static dcc_status_t dcc_component_validate_one(
    const dcc_component_builder_t *builder,
    size_t depth,
    size_t *total_count
) {
    if (builder == NULL || !dcc_component_type_valid(builder->type)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (total_count == NULL || *total_count >= DCC_COMPONENT_MAX_TREE_ITEMS ||
        depth > 1U) {
        return DCC_ERR_INVALID_ARG;
    }
    ++*total_count;
    if (builder->has_style && !dcc_button_style_valid(builder->style)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (builder->has_text_input_style && !dcc_text_input_style_valid(builder->text_input_style)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (builder->has_emoji && dcc_component_validate_emoji(&builder->emoji) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    if (builder->children_count > DCC_COMPONENT_MAX_ROW_CHILDREN ||
        !dcc_component_array_span_valid(
            builder->children,
            builder->children_count,
            sizeof(*builder->children)
        )) {
        return DCC_ERR_INVALID_ARG;
    }
    if (builder->options_count > DCC_COMPONENT_MAX_SELECT_OPTIONS ||
        !dcc_component_array_span_valid(
            builder->options,
            builder->options_count,
            sizeof(*builder->options)
        )) {
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
        dcc_status_t status = dcc_component_validate_one(
            &builder->children[i],
            depth + 1U,
            total_count
        );
        if (status != DCC_OK) {
            return status;
        }
    }

    return DCC_OK;
}

dcc_status_t dcc_component_validate(const dcc_component_builder_t *builder) {
    size_t total_count = 0U;
    return dcc_component_validate_one(builder, 0U, &total_count);
}

static int dcc_component_message_child_allowed(dcc_component_type_t type) {
    switch (type) {
        case DCC_COMPONENT_BUTTON:
        case DCC_COMPONENT_STRING_SELECT:
        case DCC_COMPONENT_USER_SELECT:
        case DCC_COMPONENT_ROLE_SELECT:
        case DCC_COMPONENT_MENTIONABLE_SELECT:
        case DCC_COMPONENT_CHANNEL_SELECT:
            return 1;
        default:
            return 0;
    }
}

static int dcc_component_is_select(dcc_component_type_t type) {
    return type == DCC_COMPONENT_STRING_SELECT ||
        type == DCC_COMPONENT_USER_SELECT ||
        type == DCC_COMPONENT_ROLE_SELECT ||
        type == DCC_COMPONENT_MENTIONABLE_SELECT ||
        type == DCC_COMPONENT_CHANNEL_SELECT;
}

dcc_status_t dcc_component_validate_message_array(
    const dcc_component_builder_t *builders,
    size_t builder_count
) {
    if (builder_count > DCC_COMPONENT_MAX_ACTION_ROWS ||
        !dcc_component_array_span_valid(
            builders,
            builder_count,
            sizeof(*builders)
        )) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t total_count = 0U;
    for (size_t i = 0U; i < builder_count; ++i) {
        const dcc_component_builder_t *row = &builders[i];
        if (row->type != DCC_COMPONENT_ACTION_ROW ||
            row->children_count == 0U ||
            dcc_component_validate_one(row, 0U, &total_count) != DCC_OK) {
            return DCC_ERR_INVALID_ARG;
        }
        int has_select = 0;
        for (size_t j = 0U; j < row->children_count; ++j) {
            dcc_component_type_t type = row->children[j].type;
            if (!dcc_component_message_child_allowed(type)) {
                return DCC_ERR_INVALID_ARG;
            }
            if (dcc_component_is_select(type)) {
                const dcc_component_builder_t *select = &row->children[j];
                if ((type == DCC_COMPONENT_STRING_SELECT &&
                        select->options_count == 0U) ||
                    (type != DCC_COMPONENT_STRING_SELECT &&
                        select->options_count != 0U)) {
                    return DCC_ERR_INVALID_ARG;
                }
                has_select = 1;
            }
        }
        if (has_select && row->children_count != 1U) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    return DCC_OK;
}
