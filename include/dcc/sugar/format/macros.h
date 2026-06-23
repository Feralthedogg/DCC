#ifndef DCC_SUGAR_FORMAT_MACROS_H
#define DCC_SUGAR_FORMAT_MACROS_H

#include <dcc/sugar/format/mentions.h>

#define DCC_MENTION_USER(user_id_) \
    dcc_sugar_user_mention((char[DCC_FORMAT_MENTION_MAX]){ 0 }, DCC_FORMAT_MENTION_MAX, (user_id_))

#define DCC_MENTION_MEMBER(user_id_) \
    dcc_sugar_member_mention((char[DCC_FORMAT_MENTION_MAX]){ 0 }, DCC_FORMAT_MENTION_MAX, (user_id_))

#define DCC_MENTION_ROLE(role_id_) \
    dcc_sugar_role_mention((char[DCC_FORMAT_MENTION_MAX]){ 0 }, DCC_FORMAT_MENTION_MAX, (role_id_))

#define DCC_MENTION_CHANNEL(channel_id_) \
    dcc_sugar_channel_mention((char[DCC_FORMAT_MENTION_MAX]){ 0 }, DCC_FORMAT_MENTION_MAX, (channel_id_))

#define DCC_MENTION_COMMAND(name_, command_id_) \
    dcc_sugar_command_mention( \
        (char[DCC_FORMAT_COMMAND_MENTION_MAX]){ 0 }, \
        DCC_FORMAT_COMMAND_MENTION_MAX, \
        (name_), \
        (command_id_) \
    )

#define DCC_MENTION_SUBCOMMAND(name_, subcommand_, command_id_) \
    dcc_sugar_subcommand_mention( \
        (char[DCC_FORMAT_COMMAND_MENTION_MAX]){ 0 }, \
        DCC_FORMAT_COMMAND_MENTION_MAX, \
        (name_), \
        (subcommand_), \
        (command_id_) \
    )

#define DCC_MENTION_SUBCOMMAND_GROUP(name_, group_, subcommand_, command_id_) \
    dcc_sugar_subcommand_group_mention( \
        (char[DCC_FORMAT_COMMAND_MENTION_MAX]){ 0 }, \
        DCC_FORMAT_COMMAND_MENTION_MAX, \
        (name_), \
        (group_), \
        (subcommand_), \
        (command_id_) \
    )

#define DCC_CUSTOM_EMOJI(name_, emoji_id_, animated_) \
    dcc_sugar_custom_emoji( \
        (char[DCC_FORMAT_EMOJI_MAX]){ 0 }, \
        DCC_FORMAT_EMOJI_MAX, \
        (name_), \
        (emoji_id_), \
        (animated_) \
    )

#define DCC_EMOJI(name_, emoji_id_) DCC_CUSTOM_EMOJI((name_), (emoji_id_), 0U)
#define DCC_ANIMATED_EMOJI(name_, emoji_id_) DCC_CUSTOM_EMOJI((name_), (emoji_id_), 1U)

#define DCC_TIMESTAMP(unix_seconds_, style_) \
    dcc_sugar_timestamp( \
        (char[DCC_FORMAT_TIMESTAMP_MAX]){ 0 }, \
        DCC_FORMAT_TIMESTAMP_MAX, \
        (unix_seconds_), \
        (style_) \
    )

#define DCC_TIMESTAMP_SHORT_TIME(unix_seconds_) DCC_TIMESTAMP((unix_seconds_), 't')
#define DCC_TIMESTAMP_LONG_TIME(unix_seconds_) DCC_TIMESTAMP((unix_seconds_), 'T')
#define DCC_TIMESTAMP_SHORT_DATE(unix_seconds_) DCC_TIMESTAMP((unix_seconds_), 'd')
#define DCC_TIMESTAMP_LONG_DATE(unix_seconds_) DCC_TIMESTAMP((unix_seconds_), 'D')
#define DCC_TIMESTAMP_SHORT(unix_seconds_) DCC_TIMESTAMP((unix_seconds_), 'f')
#define DCC_TIMESTAMP_LONG(unix_seconds_) DCC_TIMESTAMP((unix_seconds_), 'F')
#define DCC_TIMESTAMP_RELATIVE(unix_seconds_) DCC_TIMESTAMP((unix_seconds_), 'R')

#endif /* DCC_SUGAR_FORMAT_MACROS_H */
