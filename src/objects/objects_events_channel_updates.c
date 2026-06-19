#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_channel_pins_update_t *dcc_channel_pins_update_clone(const dcc_channel_pins_update_t *pins) {
    if (pins == NULL) {
        return NULL;
    }

    dcc_channel_pins_update_t *copy = (dcc_channel_pins_update_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *pins;
    copy->last_pin_timestamp = dcc_clone_string_or_null(pins->last_pin_timestamp);
    if (pins->last_pin_timestamp != NULL && copy->last_pin_timestamp == NULL) {
        dcc_channel_pins_update_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_channel_pins_update_free(dcc_channel_pins_update_t *pins) {
    if (pins == NULL) {
        return;
    }
    free((void *)pins->last_pin_timestamp);
    free(pins);
}

dcc_typing_start_t *dcc_typing_start_clone(const dcc_typing_start_t *typing_start) {
    if (typing_start == NULL) {
        return NULL;
    }

    dcc_typing_start_t *copy = (dcc_typing_start_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *typing_start;
    memset(&copy->user, 0, sizeof(copy->user));
    memset(&copy->member, 0, sizeof(copy->member));
    if (dcc_clone_user_fields(&copy->user, &typing_start->user) != 0) {
        dcc_typing_start_free(copy);
        return NULL;
    }
    if (dcc_clone_member_fields(&copy->member, &typing_start->member) != 0) {
        dcc_typing_start_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_typing_start_free(dcc_typing_start_t *typing_start) {
    if (typing_start == NULL) {
        return;
    }
    dcc_free_user_fields(&typing_start->user);
    dcc_free_member_fields(&typing_start->member);
    free(typing_start);
}
