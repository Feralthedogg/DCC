#ifndef DCC_SUGAR_CONTEXT_DISPLAY_H
#define DCC_SUGAR_CONTEXT_DISPLAY_H

#include <dcc/sugar/context/accessors.h>

#define DCC_USER_USERNAME_OR(user_, fallback_) \
    dcc_sugar_user_username_or((user_), (fallback_))
#define DCC_USER_USERNAME(user_) DCC_USER_USERNAME_OR((user_), "")
#define DCC_USER_GLOBAL_NAME_OR(user_, fallback_) \
    dcc_sugar_user_global_name_or((user_), (fallback_))
#define DCC_USER_GLOBAL_NAME(user_) DCC_USER_GLOBAL_NAME_OR((user_), "")
#define DCC_USER_DISPLAY_NAME_OR(user_, fallback_) \
    dcc_sugar_user_display_name_or((user_), (fallback_))
#define DCC_USER_DISPLAY_NAME(user_) DCC_USER_DISPLAY_NAME_OR((user_), "")
#define DCC_MEMBER_NICK_OR(member_, fallback_) \
    dcc_sugar_member_nick_or((member_), (fallback_))
#define DCC_MEMBER_NICK(member_) DCC_MEMBER_NICK_OR((member_), "")
#define DCC_MEMBER_DISPLAY_NAME_OR(member_, fallback_) \
    dcc_sugar_member_display_name_or((member_), (fallback_))
#define DCC_MEMBER_DISPLAY_NAME(member_) DCC_MEMBER_DISPLAY_NAME_OR((member_), "")
#define DCC_GUILD_NAME_OR(guild_, fallback_) dcc_sugar_guild_name_or((guild_), (fallback_))
#define DCC_GUILD_NAME(guild_) DCC_GUILD_NAME_OR((guild_), "")
#define DCC_CHANNEL_NAME_OR(channel_, fallback_) \
    dcc_sugar_channel_name_or((channel_), (fallback_))
#define DCC_CHANNEL_NAME(channel_) DCC_CHANNEL_NAME_OR((channel_), "")
#define DCC_ROLE_NAME_OR(role_, fallback_) dcc_sugar_role_name_or((role_), (fallback_))
#define DCC_ROLE_NAME(role_) DCC_ROLE_NAME_OR((role_), "")

#define DCC_USER_AVATAR_URL_EXT(user_, extension_, image_size_) \
    dcc_sugar_user_avatar_url( \
        (char[DCC_CDN_URL_MAX]){ 0 }, \
        DCC_CDN_URL_MAX, \
        (user_), \
        (extension_), \
        (image_size_) \
    )
#define DCC_USER_AVATAR_URL(user_) \
    DCC_USER_AVATAR_URL_EXT((user_), "png", DCC_CDN_DEFAULT_SIZE)
#define DCC_MEMBER_AVATAR_URL_EXT(member_, fallback_user_, extension_, image_size_) \
    dcc_sugar_member_avatar_url( \
        (char[DCC_CDN_URL_MAX]){ 0 }, \
        DCC_CDN_URL_MAX, \
        (member_), \
        (fallback_user_), \
        (extension_), \
        (image_size_) \
    )
#define DCC_MEMBER_AVATAR_URL(member_, fallback_user_) \
    DCC_MEMBER_AVATAR_URL_EXT((member_), (fallback_user_), "png", DCC_CDN_DEFAULT_SIZE)
#define DCC_GUILD_ICON_URL_EXT(guild_, extension_, image_size_) \
    dcc_sugar_guild_icon_url( \
        (char[DCC_CDN_URL_MAX]){ 0 }, \
        DCC_CDN_URL_MAX, \
        (guild_), \
        (extension_), \
        (image_size_) \
    )
#define DCC_GUILD_ICON_URL(guild_) \
    DCC_GUILD_ICON_URL_EXT((guild_), "png", DCC_CDN_DEFAULT_SIZE)

#define DCC_CTX_GUILD_NAME_OR(ctx_, fallback_) \
    DCC_GUILD_NAME_OR(DCC_CTX_GUILD((ctx_)), (fallback_))
#define DCC_CTX_GUILD_NAME(ctx_) DCC_CTX_GUILD_NAME_OR((ctx_), "")
#define DCC_CTX_CHANNEL_NAME_OR(ctx_, fallback_) \
    DCC_CHANNEL_NAME_OR(DCC_CTX_CHANNEL((ctx_)), (fallback_))
#define DCC_CTX_CHANNEL_NAME(ctx_) DCC_CTX_CHANNEL_NAME_OR((ctx_), "")
#define DCC_CTX_AUTHOR_USERNAME_OR(ctx_, fallback_) \
    DCC_USER_USERNAME_OR(DCC_CTX_AUTHOR((ctx_)), (fallback_))
#define DCC_CTX_AUTHOR_USERNAME(ctx_) DCC_CTX_AUTHOR_USERNAME_OR((ctx_), "")
#define DCC_CTX_AUTHOR_GLOBAL_NAME_OR(ctx_, fallback_) \
    DCC_USER_GLOBAL_NAME_OR(DCC_CTX_AUTHOR((ctx_)), (fallback_))
#define DCC_CTX_AUTHOR_GLOBAL_NAME(ctx_) DCC_CTX_AUTHOR_GLOBAL_NAME_OR((ctx_), "")
#define DCC_CTX_MEMBER_NICK_OR(ctx_, fallback_) \
    DCC_MEMBER_NICK_OR(DCC_CTX_MEMBER((ctx_)), (fallback_))
#define DCC_CTX_MEMBER_NICK(ctx_) DCC_CTX_MEMBER_NICK_OR((ctx_), "")
#define DCC_CTX_AUTHOR_DISPLAY_NAME_OR(ctx_, fallback_) \
    dcc_sugar_member_display_name_or( \
        DCC_CTX_MEMBER((ctx_)), \
        DCC_USER_DISPLAY_NAME_OR(DCC_CTX_AUTHOR((ctx_)), (fallback_)) \
    )
#define DCC_CTX_AUTHOR_DISPLAY_NAME(ctx_) DCC_CTX_AUTHOR_DISPLAY_NAME_OR((ctx_), "")
#define DCC_CTX_TARGET_USERNAME_OR(ctx_, fallback_) \
    DCC_USER_USERNAME_OR(DCC_CTX_TARGET_USER((ctx_)), (fallback_))
#define DCC_CTX_TARGET_USERNAME(ctx_) DCC_CTX_TARGET_USERNAME_OR((ctx_), "")
#define DCC_CTX_TARGET_DISPLAY_NAME_OR(ctx_, fallback_) \
    dcc_sugar_member_display_name_or( \
        DCC_CTX_TARGET_MEMBER((ctx_)), \
        DCC_USER_DISPLAY_NAME_OR(DCC_CTX_TARGET_USER((ctx_)), (fallback_)) \
    )
#define DCC_CTX_TARGET_DISPLAY_NAME(ctx_) DCC_CTX_TARGET_DISPLAY_NAME_OR((ctx_), "")
#define DCC_CTX_TARGET_ROLE_NAME_OR(ctx_, fallback_) \
    DCC_ROLE_NAME_OR(DCC_CTX_TARGET_ROLE((ctx_)), (fallback_))
#define DCC_CTX_TARGET_ROLE_NAME(ctx_) DCC_CTX_TARGET_ROLE_NAME_OR((ctx_), "")
#define DCC_CTX_TARGET_CHANNEL_NAME_OR(ctx_, fallback_) \
    DCC_CHANNEL_NAME_OR(DCC_CTX_TARGET_CHANNEL((ctx_)), (fallback_))
#define DCC_CTX_TARGET_CHANNEL_NAME(ctx_) DCC_CTX_TARGET_CHANNEL_NAME_OR((ctx_), "")

#define DCC_CTX_GUILD_ICON_URL_EXT(ctx_, extension_, image_size_) \
    DCC_GUILD_ICON_URL_EXT(DCC_CTX_GUILD((ctx_)), (extension_), (image_size_))
#define DCC_CTX_GUILD_ICON_URL(ctx_) DCC_CTX_GUILD_ICON_URL_EXT((ctx_), "png", DCC_CDN_DEFAULT_SIZE)
#define DCC_CTX_AUTHOR_AVATAR_URL_EXT(ctx_, extension_, image_size_) \
    DCC_MEMBER_AVATAR_URL_EXT( \
        DCC_CTX_MEMBER((ctx_)), \
        DCC_CTX_AUTHOR((ctx_)), \
        (extension_), \
        (image_size_) \
    )
#define DCC_CTX_AUTHOR_AVATAR_URL(ctx_) \
    DCC_CTX_AUTHOR_AVATAR_URL_EXT((ctx_), "png", DCC_CDN_DEFAULT_SIZE)
#define DCC_CTX_TARGET_AVATAR_URL_EXT(ctx_, extension_, image_size_) \
    DCC_MEMBER_AVATAR_URL_EXT( \
        DCC_CTX_TARGET_MEMBER((ctx_)), \
        DCC_CTX_TARGET_USER((ctx_)), \
        (extension_), \
        (image_size_) \
    )
#define DCC_CTX_TARGET_AVATAR_URL(ctx_) \
    DCC_CTX_TARGET_AVATAR_URL_EXT((ctx_), "png", DCC_CDN_DEFAULT_SIZE)

#define DCC_CTX_USER_MENTION(ctx_) DCC_MENTION_USER(DCC_CTX_USER_ID((ctx_)))
#define DCC_CTX_AUTHOR_MENTION(ctx_) DCC_CTX_USER_MENTION((ctx_))
#define DCC_CTX_MEMBER_MENTION(ctx_) DCC_MENTION_MEMBER(DCC_CTX_USER_ID((ctx_)))
#define DCC_CTX_CHANNEL_MENTION(ctx_) DCC_MENTION_CHANNEL(DCC_CTX_CHANNEL_ID((ctx_)))
#define DCC_CTX_TARGET_MENTION(ctx_) DCC_MENTION_USER(DCC_CTX_TARGET_ID((ctx_)))
#define DCC_CTX_TARGET_USER_MENTION(ctx_) DCC_MENTION_USER(DCC_CTX_TARGET_ID((ctx_)))
#define DCC_CTX_TARGET_MEMBER_MENTION(ctx_) DCC_MENTION_MEMBER(DCC_CTX_TARGET_ID((ctx_)))
#define DCC_CTX_TARGET_ROLE_MENTION(ctx_) DCC_MENTION_ROLE(DCC_CTX_TARGET_ID((ctx_)))
#define DCC_CTX_TARGET_CHANNEL_MENTION(ctx_) DCC_MENTION_CHANNEL(DCC_CTX_TARGET_ID((ctx_)))
#define DCC_CTX_COMMAND_MENTION(ctx_) \
    DCC_MENTION_COMMAND(DCC_CTX_COMMAND_NAME((ctx_)), DCC_CTX_COMMAND_ID((ctx_)))
#define DCC_CTX_MESSAGE_LINK(ctx_) \
    dcc_sugar_ctx_message_link((char[DCC_MESSAGE_LINK_MAX]){ 0 }, DCC_MESSAGE_LINK_MAX, (ctx_))


#endif /* DCC_SUGAR_CONTEXT_DISPLAY_H */
