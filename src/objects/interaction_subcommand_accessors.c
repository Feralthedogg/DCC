#include "internal/events/dcc_event_state_internal.h"

const dcc_interaction_option_t *dcc_interaction_subcommand(
    const dcc_interaction_t *interaction
) {
    if (interaction == NULL || interaction->options == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < interaction->options_count; i++) {
        if (interaction->options[i].type == 1U) {
            return &interaction->options[i];
        }
        if (interaction->options[i].type == 2U) {
            for (size_t j = 0; j < interaction->options[i].options_count; j++) {
                if (interaction->options[i].options[j].type == 1U) {
                    return &interaction->options[i].options[j];
                }
            }
        }
    }
    return NULL;
}

const dcc_interaction_option_t *dcc_interaction_subcommand_group(
    const dcc_interaction_t *interaction
) {
    if (interaction == NULL || interaction->options == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < interaction->options_count; i++) {
        if (interaction->options[i].type == 2U) {
            return &interaction->options[i];
        }
    }
    return NULL;
}
