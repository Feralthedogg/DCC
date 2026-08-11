#include "internal/objects/dcc_message_poll_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

dcc_status_t dcc_message_poll_validate_emoji(const dcc_component_emoji_t *emoji) {
    if (emoji == NULL || (!emoji->has_id && emoji->name == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

dcc_status_t dcc_message_poll_validate(const dcc_poll_builder_t *poll) {
    dcc_builder_abi_view_t view;
    if (dcc_poll_builder_abi_validate(poll, &view) != DCC_OK ||
        !dcc_builder_abi_view_has(&view, DCC_POLL_BUILDER_PRESENT_QUESTION) ||
        !dcc_builder_abi_view_has(&view, DCC_POLL_BUILDER_PRESENT_ANSWERS) ||
        poll->question.text == NULL ||
        poll->answers == NULL ||
        poll->answer_count == 0U || poll->answer_count > 10U ||
        (dcc_builder_abi_view_has(&view, DCC_POLL_BUILDER_PRESENT_DURATION_HOURS) &&
            (poll->duration_hours == 0U || poll->duration_hours > 168U)) ||
        (dcc_builder_abi_view_has(&view, DCC_POLL_BUILDER_PRESENT_LAYOUT_TYPE) &&
            poll->layout_type != DCC_POLL_LAYOUT_DEFAULT) ||
        (poll->question.has_emoji &&
            dcc_message_poll_validate_emoji(&poll->question.emoji) != DCC_OK)) {
        return DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0; i < poll->answer_count; ++i) {
        const dcc_poll_answer_t *answer = &poll->answers[i];
        if (answer->media.text == NULL ||
            (answer->media.has_emoji && dcc_message_poll_validate_emoji(&answer->media.emoji) != DCC_OK)) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    return DCC_OK;
}
