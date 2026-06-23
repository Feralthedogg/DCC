#ifndef DCC_SUGAR_POLL_H
#define DCC_SUGAR_POLL_H

#include <dcc/component.h>
#include <dcc/message.h>

#define DCC_POLL_MEDIA(text_) \
    ((dcc_poll_media_t){ \
        .text = (text_) \
    })

#define DCC_POLL_MEDIA_EMOJI(text_, emoji_) \
    ((dcc_poll_media_t){ \
        .text = (text_), \
        .emoji = (emoji_), \
        .has_emoji = 1U \
    })

#define DCC_POLL_ANSWER(text_) \
    ((dcc_poll_answer_t){ \
        .media = DCC_POLL_MEDIA(text_) \
    })

#define DCC_POLL_ANSWER_EMOJI(text_, emoji_) \
    ((dcc_poll_answer_t){ \
        .media = DCC_POLL_MEDIA_EMOJI((text_), (emoji_)) \
    })

#define DCC_POLL_ARRAY(question_, answers_, answer_count_, duration_hours_) \
    ((dcc_poll_builder_t){ \
        .size = sizeof(dcc_poll_builder_t), \
        .question = DCC_POLL_MEDIA(question_), \
        .answers = (answers_), \
        .answer_count = (answer_count_), \
        .duration_hours = (duration_hours_), \
        .layout_type = DCC_POLL_LAYOUT_DEFAULT, \
        .has_duration = 1U, \
        .has_layout_type = 1U \
    })

#define DCC_POLL(question_, duration_hours_, ...) \
    ((dcc_poll_builder_t){ \
        .size = sizeof(dcc_poll_builder_t), \
        .question = DCC_POLL_MEDIA(question_), \
        .answers = (dcc_poll_answer_t[]){ __VA_ARGS__ }, \
        .answer_count = sizeof((dcc_poll_answer_t[]){ __VA_ARGS__ }) / sizeof(dcc_poll_answer_t), \
        .duration_hours = (duration_hours_), \
        .layout_type = DCC_POLL_LAYOUT_DEFAULT, \
        .has_duration = 1U, \
        .has_layout_type = 1U \
    })

#define DCC_POLL_DEFAULT(question_, ...) \
    ((dcc_poll_builder_t){ \
        .size = sizeof(dcc_poll_builder_t), \
        .question = DCC_POLL_MEDIA(question_), \
        .answers = (dcc_poll_answer_t[]){ __VA_ARGS__ }, \
        .answer_count = sizeof((dcc_poll_answer_t[]){ __VA_ARGS__ }) / sizeof(dcc_poll_answer_t), \
        .layout_type = DCC_POLL_LAYOUT_DEFAULT, \
        .has_layout_type = 1U \
    })

#define DCC_POLL_MULTI(question_, duration_hours_, ...) \
    ((dcc_poll_builder_t){ \
        .size = sizeof(dcc_poll_builder_t), \
        .question = DCC_POLL_MEDIA(question_), \
        .answers = (dcc_poll_answer_t[]){ __VA_ARGS__ }, \
        .answer_count = sizeof((dcc_poll_answer_t[]){ __VA_ARGS__ }) / sizeof(dcc_poll_answer_t), \
        .duration_hours = (duration_hours_), \
        .layout_type = DCC_POLL_LAYOUT_DEFAULT, \
        .allow_multiselect = 1U, \
        .has_duration = 1U, \
        .has_layout_type = 1U, \
        .has_allow_multiselect = 1U \
    })

#define DCC_POLL_MULTI_DEFAULT(question_, ...) \
    ((dcc_poll_builder_t){ \
        .size = sizeof(dcc_poll_builder_t), \
        .question = DCC_POLL_MEDIA(question_), \
        .answers = (dcc_poll_answer_t[]){ __VA_ARGS__ }, \
        .answer_count = sizeof((dcc_poll_answer_t[]){ __VA_ARGS__ }) / sizeof(dcc_poll_answer_t), \
        .layout_type = DCC_POLL_LAYOUT_DEFAULT, \
        .allow_multiselect = 1U, \
        .has_layout_type = 1U, \
        .has_allow_multiselect = 1U \
    })

#define DCC_POLL_YES_NO(question_) \
    DCC_POLL_DEFAULT((question_), DCC_POLL_ANSWER("Yes"), DCC_POLL_ANSWER("No"))

#define DCC_POLL_YES_NO_FOR(question_, duration_hours_) \
    DCC_POLL((question_), (duration_hours_), DCC_POLL_ANSWER("Yes"), DCC_POLL_ANSWER("No"))

#define DCC_MESSAGE_POLL(poll_) \
    ((dcc_message_builder_t){ \
        .poll = &((dcc_poll_builder_t[]){ (poll_) })[0] \
    })

#define DCC_MESSAGE_TEXT_POLL(content_, poll_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .poll = &((dcc_poll_builder_t[]){ (poll_) })[0], \
        .has_content = 1U \
    })

#define DCC_MESSAGE_POLL_JSON(poll_json_) \
    ((dcc_message_builder_t){ \
        .poll_json = (poll_json_) \
    })

#define DCC_MESSAGE_TEXT_POLL_JSON(content_, poll_json_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .poll_json = (poll_json_), \
        .has_content = 1U \
    })

#endif
