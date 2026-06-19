#include "internal/objects/dcc_interaction_clone_internal.h"

int dcc_clone_interaction_options(
    dcc_interaction_t *copy,
    const dcc_interaction_t *interaction
) {
    if (interaction->options_count == 0) {
        return 0;
    }

    dcc_interaction_option_t *options = NULL;
    const dcc_interaction_option_t *focused = NULL;
    if (dcc_clone_interaction_option_array(
            interaction->options,
            interaction->options_count,
            &options,
            interaction->focused_option,
            &focused
        ) != 0) {
        return -1;
    }
    copy->options = options;
    copy->focused_option = focused;
    return 0;
}
