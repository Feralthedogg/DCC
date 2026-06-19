#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

void dcc_free_interaction_option_array(dcc_interaction_option_t *options, size_t count) {
    if (options == NULL) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        dcc_free_interaction_option_array(
            (dcc_interaction_option_t *)options[i].options,
            options[i].options_count
        );
        free((void *)options[i].name);
        free((void *)options[i].string_value);
    }
    free(options);
}

int dcc_clone_interaction_option_array(
    const dcc_interaction_option_t *src,
    size_t count,
    dcc_interaction_option_t **out,
    const dcc_interaction_option_t *src_focused,
    const dcc_interaction_option_t **copy_focused
) {
    if (out == NULL || copy_focused == NULL) {
        return -1;
    }
    *out = NULL;
    if (count == 0) {
        return 0;
    }
    if (src == NULL || count > (size_t)-1 / sizeof(dcc_interaction_option_t)) {
        return -1;
    }

    dcc_interaction_option_t *copy = (dcc_interaction_option_t *)calloc(count, sizeof(*copy));
    if (copy == NULL) {
        return -1;
    }

    for (size_t i = 0; i < count; i++) {
        copy[i] = src[i];
        copy[i].name = dcc_clone_string_or_null(src[i].name);
        copy[i].string_value = dcc_clone_string_or_null(src[i].string_value);
        copy[i].options = NULL;
        if ((src[i].name != NULL && copy[i].name == NULL) ||
            (src[i].string_value != NULL && copy[i].string_value == NULL)) {
            dcc_free_interaction_option_array(copy, count);
            return -1;
        }

        if (src_focused == &src[i] || (*copy_focused == NULL && src[i].focused)) {
            *copy_focused = &copy[i];
        }

        if (src[i].options_count > 0) {
            dcc_interaction_option_t *children = NULL;
            if (dcc_clone_interaction_option_array(
                    src[i].options,
                    src[i].options_count,
                    &children,
                    src_focused,
                    copy_focused
                ) != 0) {
                dcc_free_interaction_option_array(copy, count);
                return -1;
            }
            copy[i].options = children;
        }
    }

    *out = copy;
    return 0;
}
