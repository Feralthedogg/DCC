#ifndef DCC_SUGAR_MENTIONS_H
#define DCC_SUGAR_MENTIONS_H

#include <dcc/message.h>

#define DCC_ALLOWED_MENTIONS_NONE() \
    ((dcc_allowed_mentions_builder_t)DCC_ALLOWED_MENTIONS_BUILDER_INIT)

#define DCC_ALLOWED_MENTIONS_ALL() \
    ((dcc_allowed_mentions_builder_t){ \
        .size = sizeof(dcc_allowed_mentions_builder_t), \
        .version = DCC_ALLOWED_MENTIONS_BUILDER_VERSION, \
        .present = DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_PARSE_USERS | DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_PARSE_ROLES | DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_PARSE_EVERYONE, \
        .parse_users = 1U, \
        .parse_roles = 1U, \
        .parse_everyone = 1U \
    })

#define DCC_ALLOWED_MENTIONS_PARSE(users_, roles_, everyone_) \
    ((dcc_allowed_mentions_builder_t){ \
        .size = sizeof(dcc_allowed_mentions_builder_t), \
        .version = DCC_ALLOWED_MENTIONS_BUILDER_VERSION, \
        .present = DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_PARSE_USERS | DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_PARSE_ROLES | DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_PARSE_EVERYONE, \
        .parse_users = (uint8_t)((users_) ? 1U : 0U), \
        .parse_roles = (uint8_t)((roles_) ? 1U : 0U), \
        .parse_everyone = (uint8_t)((everyone_) ? 1U : 0U) \
    })

#define DCC_ALLOWED_MENTIONS_REPLY(replied_user_) \
    ((dcc_allowed_mentions_builder_t){ \
        .size = sizeof(dcc_allowed_mentions_builder_t), \
        .version = DCC_ALLOWED_MENTIONS_BUILDER_VERSION, \
        .present = DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_REPLIED_USER, \
        .replied_user = (uint8_t)((replied_user_) ? 1U : 0U), \
    })

#define DCC_ALLOWED_MENTIONS_PARSE_REPLY(users_, roles_, everyone_, replied_user_) \
    ((dcc_allowed_mentions_builder_t){ \
        .size = sizeof(dcc_allowed_mentions_builder_t), \
        .version = DCC_ALLOWED_MENTIONS_BUILDER_VERSION, \
        .present = DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_PARSE_USERS | DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_PARSE_ROLES | DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_PARSE_EVERYONE | DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_REPLIED_USER, \
        .parse_users = (uint8_t)((users_) ? 1U : 0U), \
        .parse_roles = (uint8_t)((roles_) ? 1U : 0U), \
        .parse_everyone = (uint8_t)((everyone_) ? 1U : 0U), \
        .replied_user = (uint8_t)((replied_user_) ? 1U : 0U) \
    })

#define DCC_ALLOWED_MENTIONS_USERS_ARRAY(users_, user_count_) \
    ((dcc_allowed_mentions_builder_t){ \
        .size = sizeof(dcc_allowed_mentions_builder_t), \
        .version = DCC_ALLOWED_MENTIONS_BUILDER_VERSION, \
        .present = DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_USERS, \
        .users = (users_), \
        .user_count = (user_count_) \
    })

#define DCC_ALLOWED_MENTIONS_ROLES_ARRAY(roles_, role_count_) \
    ((dcc_allowed_mentions_builder_t){ \
        .size = sizeof(dcc_allowed_mentions_builder_t), \
        .version = DCC_ALLOWED_MENTIONS_BUILDER_VERSION, \
        .present = DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_ROLES, \
        .roles = (roles_), \
        .role_count = (role_count_) \
    })

#define DCC_ALLOWED_MENTIONS_USERS_ROLES_ARRAY(users_, user_count_, roles_, role_count_) \
    ((dcc_allowed_mentions_builder_t){ \
        .size = sizeof(dcc_allowed_mentions_builder_t), \
        .version = DCC_ALLOWED_MENTIONS_BUILDER_VERSION, \
        .present = DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_USERS | DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_ROLES, \
        .users = (users_), \
        .user_count = (user_count_), \
        .roles = (roles_), \
        .role_count = (role_count_) \
    })

#define DCC_ALLOWED_MENTIONS_USERS(...) \
    ((dcc_allowed_mentions_builder_t){ \
        .size = sizeof(dcc_allowed_mentions_builder_t), \
        .version = DCC_ALLOWED_MENTIONS_BUILDER_VERSION, \
        .present = DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_USERS, \
        .users = (dcc_snowflake_t[]){ __VA_ARGS__ }, \
        .user_count = sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t) \
    })

#define DCC_ALLOWED_MENTIONS_ROLES(...) \
    ((dcc_allowed_mentions_builder_t){ \
        .size = sizeof(dcc_allowed_mentions_builder_t), \
        .version = DCC_ALLOWED_MENTIONS_BUILDER_VERSION, \
        .present = DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_ROLES, \
        .roles = (dcc_snowflake_t[]){ __VA_ARGS__ }, \
        .role_count = sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t) \
    })

#define DCC_ALLOWED_MENTIONS_USERS_REPLY(replied_user_, ...) \
    ((dcc_allowed_mentions_builder_t){ \
        .size = sizeof(dcc_allowed_mentions_builder_t), \
        .version = DCC_ALLOWED_MENTIONS_BUILDER_VERSION, \
        .present = DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_USERS | DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_REPLIED_USER, \
        .users = (dcc_snowflake_t[]){ __VA_ARGS__ }, \
        .user_count = sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t), \
        .replied_user = (uint8_t)((replied_user_) ? 1U : 0U) \
    })

#define DCC_ALLOWED_MENTIONS_ROLES_REPLY(replied_user_, ...) \
    ((dcc_allowed_mentions_builder_t){ \
        .size = sizeof(dcc_allowed_mentions_builder_t), \
        .version = DCC_ALLOWED_MENTIONS_BUILDER_VERSION, \
        .present = DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_ROLES | DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_REPLIED_USER, \
        .roles = (dcc_snowflake_t[]){ __VA_ARGS__ }, \
        .role_count = sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t), \
        .replied_user = (uint8_t)((replied_user_) ? 1U : 0U) \
    })

#define DCC_MESSAGE_ALLOWED_MENTIONS(content_, allowed_mentions_) \
    ((dcc_message_builder_t){ \
        .size = sizeof(dcc_message_builder_t), \
        .version = DCC_MESSAGE_BUILDER_VERSION, \
        .present = DCC_MESSAGE_BUILDER_PRESENT_CONTENT | DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS, \
        .content = (content_), \
        .allowed_mentions = &((dcc_allowed_mentions_builder_t[]){ (allowed_mentions_) })[0] \
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
        .size = sizeof(dcc_message_builder_t), \
        .version = DCC_MESSAGE_BUILDER_VERSION, \
        .present = DCC_MESSAGE_BUILDER_PRESENT_CONTENT | DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS | DCC_MESSAGE_BUILDER_PRESENT_FLAGS, \
        .content = (content_), \
        .allowed_mentions = &((dcc_allowed_mentions_builder_t[]){ (allowed_mentions_) })[0], \
        .flags = DCC_MESSAGE_FLAG_EPHEMERAL \
    })

#define DCC_MESSAGE_EPHEMERAL_NO_MENTIONS(content_) \
    DCC_MESSAGE_EPHEMERAL_MENTIONS((content_), DCC_ALLOWED_MENTIONS_NONE())

#endif
