#include "internal/events/dcc_event_state_internal.h"

#include <string.h>

static const dcc_interaction_option_t *dcc_interaction_option_array_find(
    const dcc_interaction_option_t *options,
    size_t count,
    const char *name
) {
    if (options == NULL || name == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < count; i++) {
        if (options[i].name != NULL && strcmp(options[i].name, name) == 0) {
            return &options[i];
        }
        const dcc_interaction_option_t *child =
            dcc_interaction_option_array_find(options[i].options, options[i].options_count, name);
        if (child != NULL) {
            return child;
        }
    }
    return NULL;
}

static const dcc_interaction_option_t *dcc_interaction_option_array_focused(
    const dcc_interaction_option_t *options,
    size_t count
) {
    if (options == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < count; i++) {
        if (options[i].focused) {
            return &options[i];
        }
        const dcc_interaction_option_t *child =
            dcc_interaction_option_array_focused(options[i].options, options[i].options_count);
        if (child != NULL) {
            return child;
        }
    }
    return NULL;
}

const dcc_interaction_option_t *dcc_interaction_option_by_name(
    const dcc_interaction_t *interaction,
    const char *name
) {
    if (interaction == NULL || name == NULL) {
        return NULL;
    }

    return dcc_interaction_option_array_find(interaction->options, interaction->options_count, name);
}

const dcc_interaction_option_t *dcc_interaction_focused_option(
    const dcc_interaction_t *interaction
) {
    if (interaction == NULL) {
        return NULL;
    }
    if (interaction->focused_option != NULL) {
        return interaction->focused_option;
    }
    if (interaction->options == NULL) {
        return NULL;
    }

    return dcc_interaction_option_array_focused(interaction->options, interaction->options_count);
}

const dcc_interaction_option_t *dcc_interaction_option_child_by_name(
    const dcc_interaction_option_t *option,
    const char *name
) {
    if (option == NULL || name == NULL || option->options == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < option->options_count; i++) {
        if (option->options[i].name != NULL && strcmp(option->options[i].name, name) == 0) {
            return &option->options[i];
        }
    }
    return NULL;
}
