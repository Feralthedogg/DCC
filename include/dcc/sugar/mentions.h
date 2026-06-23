#ifndef DCC_SUGAR_MENTIONS_H
#define DCC_SUGAR_MENTIONS_H

#include <dcc/message.h>

#define DCC_ALLOWED_MENTIONS_NONE() \
    ((dcc_allowed_mentions_builder_t){ 0 })

#define DCC_ALLOWED_MENTIONS_ALL() \
    ((dcc_allowed_mentions_builder_t){ \
        .parse_users = 1U, \
        .parse_roles = 1U, \
        .parse_everyone = 1U \
    })

#define DCC_ALLOWED_MENTIONS_PARSE(users_, roles_, everyone_) \
    ((dcc_allowed_mentions_builder_t){ \
        .parse_users = (uint8_t)((users_) ? 1U : 0U), \
        .parse_roles = (uint8_t)((roles_) ? 1U : 0U), \
        .parse_everyone = (uint8_t)((everyone_) ? 1U : 0U) \
    })

#define DCC_ALLOWED_MENTIONS_REPLY(replied_user_) \
    ((dcc_allowed_mentions_builder_t){ \
        .replied_user = (uint8_t)((replied_user_) ? 1U : 0U), \
        .has_replied_user = 1U \
    })

#define DCC_ALLOWED_MENTIONS_PARSE_REPLY(users_, roles_, everyone_, replied_user_) \
    ((dcc_allowed_mentions_builder_t){ \
        .parse_users = (uint8_t)((users_) ? 1U : 0U), \
        .parse_roles = (uint8_t)((roles_) ? 1U : 0U), \
        .parse_everyone = (uint8_t)((everyone_) ? 1U : 0U), \
        .replied_user = (uint8_t)((replied_user_) ? 1U : 0U), \
        .has_replied_user = 1U \
    })

#define DCC_ALLOWED_MENTIONS_USERS_ARRAY(users_, user_count_) \
    ((dcc_allowed_mentions_builder_t){ \
        .users = (users_), \
        .user_count = (user_count_) \
    })

#define DCC_ALLOWED_MENTIONS_ROLES_ARRAY(roles_, role_count_) \
    ((dcc_allowed_mentions_builder_t){ \
        .roles = (roles_), \
        .role_count = (role_count_) \
    })

#define DCC_ALLOWED_MENTIONS_USERS_ROLES_ARRAY(users_, user_count_, roles_, role_count_) \
    ((dcc_allowed_mentions_builder_t){ \
        .users = (users_), \
        .user_count = (user_count_), \
        .roles = (roles_), \
        .role_count = (role_count_) \
    })

#define DCC_ALLOWED_MENTIONS_USERS(...) \
    ((dcc_allowed_mentions_builder_t){ \
        .users = (dcc_snowflake_t[]){ __VA_ARGS__ }, \
        .user_count = sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t) \
    })

#define DCC_ALLOWED_MENTIONS_ROLES(...) \
    ((dcc_allowed_mentions_builder_t){ \
        .roles = (dcc_snowflake_t[]){ __VA_ARGS__ }, \
        .role_count = sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t) \
    })

#define DCC_ALLOWED_MENTIONS_USERS_REPLY(replied_user_, ...) \
    ((dcc_allowed_mentions_builder_t){ \
        .users = (dcc_snowflake_t[]){ __VA_ARGS__ }, \
        .user_count = sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t), \
        .replied_user = (uint8_t)((replied_user_) ? 1U : 0U), \
        .has_replied_user = 1U \
    })

#define DCC_ALLOWED_MENTIONS_ROLES_REPLY(replied_user_, ...) \
    ((dcc_allowed_mentions_builder_t){ \
        .roles = (dcc_snowflake_t[]){ __VA_ARGS__ }, \
        .role_count = sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t), \
        .replied_user = (uint8_t)((replied_user_) ? 1U : 0U), \
        .has_replied_user = 1U \
    })

#define DCC_MESSAGE_ALLOWED_MENTIONS(content_, allowed_mentions_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .allowed_mentions = &((dcc_allowed_mentions_builder_t[]){ (allowed_mentions_) })[0], \
        .has_content = 1U \
    })

#define DCC_MESSAGE_MENTIONS(content_, allowed_mentions_) \
    DCC_MESSAGE_ALLOWED_MENTIONS((content_), (allowed_mentions_))

#define DCC_MESSAGE_NO_MENTIONS(content_) \
    DCC_MESSAGE_ALLOWED_MENTIONS((content_), DCC_ALLOWED_MENTIONS_NONE())

#define DCC_MESSAGE_MENTION_ALL(content_) \
    DCC_MESSAGE_ALLOWED_MENTIONS((content_), DCC_ALLOWED_MENTIONS_ALL())

#define DCC_MESSAGE_MENTION_USERS(content_, ...) \
    DCC_MESSAGE_ALLOWED_MENTIONS((content_), DCC_ALLOWED_MENTIONS_USERS(__VA_ARGS__))

#define DCC_MESSAGE_MENTION_ROLES(content_, ...) \
    DCC_MESSAGE_ALLOWED_MENTIONS((content_), DCC_ALLOWED_MENTIONS_ROLES(__VA_ARGS__))

#define DCC_MESSAGE_EPHEMERAL_MENTIONS(content_, allowed_mentions_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .allowed_mentions = &((dcc_allowed_mentions_builder_t[]){ (allowed_mentions_) })[0], \
        .flags = DCC_MESSAGE_FLAG_EPHEMERAL, \
        .has_content = 1U, \
        .has_flags = 1U \
    })

#define DCC_MESSAGE_EPHEMERAL_NO_MENTIONS(content_) \
    DCC_MESSAGE_EPHEMERAL_MENTIONS((content_), DCC_ALLOWED_MENTIONS_NONE())

#endif
