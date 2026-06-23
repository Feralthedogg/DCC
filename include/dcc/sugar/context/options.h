#ifndef DCC_SUGAR_CONTEXT_OPTIONS_H
#define DCC_SUGAR_CONTEXT_OPTIONS_H

#include <dcc/sugar/context/display.h>

#define DCC_CTX_FOCUSED_OPTION(ctx_) dcc_ctx_focused_option((ctx_))
#define DCC_CTX_FOCUSED_NAME(ctx_) dcc_ctx_focused_option_name((ctx_))
#define DCC_CTX_FOCUSED_STRING(ctx_, fallback_) dcc_ctx_focused_option_string((ctx_), (fallback_))

#define DCC_CTX_ARG(ctx_, name_) dcc_ctx_option((ctx_), (name_))
#define DCC_CTX_ARG_PRESENT(ctx_, name_) dcc_ctx_option_present((ctx_), (name_))
#define DCC_CTX_ARG_STRING_OR(ctx_, name_, fallback_) \
    dcc_ctx_option_string((ctx_), (name_), (fallback_))
#define DCC_CTX_ARG_STRING(ctx_, name_) DCC_CTX_ARG_STRING_OR((ctx_), (name_), NULL)
#define DCC_CTX_ARG_TEXT_OR(ctx_, name_, fallback_) \
    DCC_CTX_ARG_STRING_OR((ctx_), (name_), (fallback_))
#define DCC_CTX_ARG_TEXT(ctx_, name_) DCC_CTX_ARG_STRING((ctx_), (name_))
#define DCC_CTX_ARG_INT_OR(ctx_, name_, fallback_) \
    dcc_ctx_option_int((ctx_), (name_), (fallback_))
#define DCC_CTX_ARG_INT(ctx_, name_) DCC_CTX_ARG_INT_OR((ctx_), (name_), 0)
#define DCC_CTX_ARG_INTEGER_OR(ctx_, name_, fallback_) \
    DCC_CTX_ARG_INT_OR((ctx_), (name_), (fallback_))
#define DCC_CTX_ARG_INTEGER(ctx_, name_) DCC_CTX_ARG_INT((ctx_), (name_))
#define DCC_CTX_ARG_NUMBER_OR(ctx_, name_, fallback_) \
    dcc_ctx_option_number((ctx_), (name_), (fallback_))
#define DCC_CTX_ARG_NUMBER(ctx_, name_) DCC_CTX_ARG_NUMBER_OR((ctx_), (name_), 0.0)
#define DCC_CTX_ARG_BOOL_OR(ctx_, name_, fallback_) \
    dcc_ctx_option_bool((ctx_), (name_), (fallback_))
#define DCC_CTX_ARG_BOOL(ctx_, name_) DCC_CTX_ARG_BOOL_OR((ctx_), (name_), 0U)
#define DCC_CTX_ARG_BOOLEAN_OR(ctx_, name_, fallback_) \
    DCC_CTX_ARG_BOOL_OR((ctx_), (name_), (fallback_))
#define DCC_CTX_ARG_BOOLEAN(ctx_, name_) DCC_CTX_ARG_BOOL((ctx_), (name_))
#define DCC_CTX_ARG_SNOWFLAKE_OR(ctx_, name_, fallback_) \
    dcc_ctx_option_snowflake((ctx_), (name_), (fallback_))
#define DCC_CTX_ARG_SNOWFLAKE(ctx_, name_) DCC_CTX_ARG_SNOWFLAKE_OR((ctx_), (name_), 0U)
#define DCC_CTX_ARG_USER(ctx_, name_) dcc_ctx_option_user((ctx_), (name_))
#define DCC_CTX_ARG_MEMBER(ctx_, name_) dcc_ctx_option_member((ctx_), (name_))
#define DCC_CTX_ARG_ROLE(ctx_, name_) dcc_ctx_option_role((ctx_), (name_))
#define DCC_CTX_ARG_CHANNEL(ctx_, name_) dcc_ctx_option_channel((ctx_), (name_))
#define DCC_CTX_ARG_MESSAGE(ctx_, name_) dcc_ctx_option_message((ctx_), (name_))
#define DCC_CTX_ARG_ATTACHMENT(ctx_, name_) dcc_ctx_option_attachment((ctx_), (name_))
#define DCC_CTX_ARG_USERNAME_OR(ctx_, name_, fallback_) \
    DCC_USER_USERNAME_OR(DCC_CTX_ARG_USER((ctx_), (name_)), (fallback_))
#define DCC_CTX_ARG_USERNAME(ctx_, name_) DCC_CTX_ARG_USERNAME_OR((ctx_), (name_), "")
#define DCC_CTX_ARG_USER_DISPLAY_NAME_OR(ctx_, name_, fallback_) \
    dcc_sugar_member_display_name_or( \
        DCC_CTX_ARG_MEMBER((ctx_), (name_)), \
        DCC_USER_DISPLAY_NAME_OR(DCC_CTX_ARG_USER((ctx_), (name_)), (fallback_)) \
    )
#define DCC_CTX_ARG_USER_DISPLAY_NAME(ctx_, name_) \
    DCC_CTX_ARG_USER_DISPLAY_NAME_OR((ctx_), (name_), "")
#define DCC_CTX_ARG_MEMBER_DISPLAY_NAME_OR(ctx_, name_, fallback_) \
    DCC_CTX_ARG_USER_DISPLAY_NAME_OR((ctx_), (name_), (fallback_))
#define DCC_CTX_ARG_MEMBER_DISPLAY_NAME(ctx_, name_) \
    DCC_CTX_ARG_USER_DISPLAY_NAME((ctx_), (name_))
#define DCC_CTX_ARG_ROLE_NAME_OR(ctx_, name_, fallback_) \
    DCC_ROLE_NAME_OR(DCC_CTX_ARG_ROLE((ctx_), (name_)), (fallback_))
#define DCC_CTX_ARG_ROLE_NAME(ctx_, name_) DCC_CTX_ARG_ROLE_NAME_OR((ctx_), (name_), "")
#define DCC_CTX_ARG_CHANNEL_NAME_OR(ctx_, name_, fallback_) \
    DCC_CHANNEL_NAME_OR(DCC_CTX_ARG_CHANNEL((ctx_), (name_)), (fallback_))
#define DCC_CTX_ARG_CHANNEL_NAME(ctx_, name_) \
    DCC_CTX_ARG_CHANNEL_NAME_OR((ctx_), (name_), "")
#define DCC_CTX_ARG_USER_AVATAR_URL_EXT(ctx_, name_, extension_, image_size_) \
    DCC_MEMBER_AVATAR_URL_EXT( \
        DCC_CTX_ARG_MEMBER((ctx_), (name_)), \
        DCC_CTX_ARG_USER((ctx_), (name_)), \
        (extension_), \
        (image_size_) \
    )
#define DCC_CTX_ARG_USER_AVATAR_URL(ctx_, name_) \
    DCC_CTX_ARG_USER_AVATAR_URL_EXT((ctx_), (name_), "png", DCC_CDN_DEFAULT_SIZE)
#define DCC_CTX_ARG_MEMBER_AVATAR_URL_EXT(ctx_, name_, extension_, image_size_) \
    DCC_CTX_ARG_USER_AVATAR_URL_EXT((ctx_), (name_), (extension_), (image_size_))
#define DCC_CTX_ARG_MEMBER_AVATAR_URL(ctx_, name_) \
    DCC_CTX_ARG_USER_AVATAR_URL((ctx_), (name_))
#define DCC_CTX_ARG_MENTION(ctx_, name_) \
    DCC_MENTION_USER(DCC_CTX_ARG_SNOWFLAKE((ctx_), (name_)))
#define DCC_CTX_ARG_USER_MENTION(ctx_, name_) \
    DCC_MENTION_USER(DCC_CTX_ARG_SNOWFLAKE((ctx_), (name_)))
#define DCC_CTX_ARG_MEMBER_MENTION(ctx_, name_) \
    DCC_MENTION_MEMBER(DCC_CTX_ARG_SNOWFLAKE((ctx_), (name_)))
#define DCC_CTX_ARG_ROLE_MENTION(ctx_, name_) \
    DCC_MENTION_ROLE(DCC_CTX_ARG_SNOWFLAKE((ctx_), (name_)))
#define DCC_CTX_ARG_CHANNEL_MENTION(ctx_, name_) \
    DCC_MENTION_CHANNEL(DCC_CTX_ARG_SNOWFLAKE((ctx_), (name_)))

#define DCC_CTX_OPT_PRESENT(ctx_, name_) dcc_ctx_option_present((ctx_), (name_))
#define DCC_CTX_OPT_STRING(ctx_, name_, fallback_) \
    dcc_ctx_option_string((ctx_), (name_), (fallback_))
#define DCC_CTX_OPT_INT(ctx_, name_, fallback_) dcc_ctx_option_int((ctx_), (name_), (fallback_))
#define DCC_CTX_OPT_NUMBER(ctx_, name_, fallback_) \
    dcc_ctx_option_number((ctx_), (name_), (fallback_))
#define DCC_CTX_OPT_BOOL(ctx_, name_, fallback_) dcc_ctx_option_bool((ctx_), (name_), (fallback_))
#define DCC_CTX_OPT_SNOWFLAKE(ctx_, name_, fallback_) \
    dcc_ctx_option_snowflake((ctx_), (name_), (fallback_))
#define DCC_CTX_OPT_USER(ctx_, name_) dcc_ctx_option_user((ctx_), (name_))
#define DCC_CTX_OPT_MEMBER(ctx_, name_) dcc_ctx_option_member((ctx_), (name_))
#define DCC_CTX_OPT_ROLE(ctx_, name_) dcc_ctx_option_role((ctx_), (name_))
#define DCC_CTX_OPT_CHANNEL(ctx_, name_) dcc_ctx_option_channel((ctx_), (name_))
#define DCC_CTX_OPT_MESSAGE(ctx_, name_) dcc_ctx_option_message((ctx_), (name_))
#define DCC_CTX_OPT_ATTACHMENT(ctx_, name_) dcc_ctx_option_attachment((ctx_), (name_))
#define DCC_CTX_OPTION_USER(ctx_, name_) dcc_ctx_option_user((ctx_), (name_))
#define DCC_CTX_OPTION_MEMBER(ctx_, name_) dcc_ctx_option_member((ctx_), (name_))
#define DCC_CTX_OPTION_ROLE(ctx_, name_) dcc_ctx_option_role((ctx_), (name_))
#define DCC_CTX_OPTION_CHANNEL(ctx_, name_) dcc_ctx_option_channel((ctx_), (name_))
#define DCC_CTX_OPTION_MESSAGE(ctx_, name_) dcc_ctx_option_message((ctx_), (name_))
#define DCC_CTX_OPTION_ATTACHMENT(ctx_, name_) dcc_ctx_option_attachment((ctx_), (name_))

#define DCC_PARAM_PRESENT(ctx_, name_) DCC_CTX_OPT_PRESENT((ctx_), (name_))
#define DCC_PARAM_STRING_OR(ctx_, name_, fallback_) \
    DCC_CTX_OPT_STRING((ctx_), (name_), (fallback_))
#define DCC_PARAM_STRING(ctx_, name_) DCC_PARAM_STRING_OR((ctx_), (name_), NULL)
#define DCC_PARAM_INT_OR(ctx_, name_, fallback_) DCC_CTX_OPT_INT((ctx_), (name_), (fallback_))
#define DCC_PARAM_INT(ctx_, name_) DCC_PARAM_INT_OR((ctx_), (name_), 0)
#define DCC_PARAM_INTEGER_OR(ctx_, name_, fallback_) \
    DCC_PARAM_INT_OR((ctx_), (name_), (fallback_))
#define DCC_PARAM_INTEGER(ctx_, name_) DCC_PARAM_INT((ctx_), (name_))
#define DCC_PARAM_NUMBER_OR(ctx_, name_, fallback_) \
    DCC_CTX_OPT_NUMBER((ctx_), (name_), (fallback_))
#define DCC_PARAM_NUMBER(ctx_, name_) DCC_PARAM_NUMBER_OR((ctx_), (name_), 0.0)
#define DCC_PARAM_BOOL_OR(ctx_, name_, fallback_) DCC_CTX_OPT_BOOL((ctx_), (name_), (fallback_))
#define DCC_PARAM_BOOL(ctx_, name_) DCC_PARAM_BOOL_OR((ctx_), (name_), 0U)
#define DCC_PARAM_BOOLEAN_OR(ctx_, name_, fallback_) \
    DCC_PARAM_BOOL_OR((ctx_), (name_), (fallback_))
#define DCC_PARAM_BOOLEAN(ctx_, name_) DCC_PARAM_BOOL((ctx_), (name_))
#define DCC_PARAM_SNOWFLAKE_OR(ctx_, name_, fallback_) \
    DCC_CTX_OPT_SNOWFLAKE((ctx_), (name_), (fallback_))
#define DCC_PARAM_SNOWFLAKE(ctx_, name_) DCC_PARAM_SNOWFLAKE_OR((ctx_), (name_), 0U)
#define DCC_PARAM_USER(ctx_, name_) DCC_CTX_OPT_USER((ctx_), (name_))
#define DCC_PARAM_MEMBER(ctx_, name_) DCC_CTX_OPT_MEMBER((ctx_), (name_))
#define DCC_PARAM_ROLE(ctx_, name_) DCC_CTX_OPT_ROLE((ctx_), (name_))
#define DCC_PARAM_CHANNEL(ctx_, name_) DCC_CTX_OPT_CHANNEL((ctx_), (name_))
#define DCC_PARAM_MESSAGE(ctx_, name_) DCC_CTX_OPT_MESSAGE((ctx_), (name_))
#define DCC_PARAM_ATTACHMENT(ctx_, name_) DCC_CTX_OPT_ATTACHMENT((ctx_), (name_))
#define DCC_PARAM_FOCUSED(ctx_) DCC_CTX_FOCUSED_OPTION((ctx_))
#define DCC_PARAM_FOCUSED_NAME(ctx_) DCC_CTX_FOCUSED_NAME((ctx_))
#define DCC_PARAM_FOCUSED_STRING_OR(ctx_, fallback_) \
    DCC_CTX_FOCUSED_STRING((ctx_), (fallback_))
#define DCC_PARAM_FOCUSED_STRING(ctx_) DCC_PARAM_FOCUSED_STRING_OR((ctx_), NULL)

#define DCC_OPTION_PRESENT(ctx_, name_) DCC_PARAM_PRESENT((ctx_), (name_))
#define DCC_OPTION_STRING_OR(ctx_, name_, fallback_) \
    DCC_PARAM_STRING_OR((ctx_), (name_), (fallback_))
#define DCC_OPTION_STRING(ctx_, name_) DCC_PARAM_STRING((ctx_), (name_))
#define DCC_OPTION_INT_OR(ctx_, name_, fallback_) \
    DCC_PARAM_INT_OR((ctx_), (name_), (fallback_))
#define DCC_OPTION_INT(ctx_, name_) DCC_PARAM_INT((ctx_), (name_))
#define DCC_OPTION_INTEGER_OR(ctx_, name_, fallback_) \
    DCC_PARAM_INTEGER_OR((ctx_), (name_), (fallback_))
#define DCC_OPTION_INTEGER(ctx_, name_) DCC_PARAM_INTEGER((ctx_), (name_))
#define DCC_OPTION_NUMBER_OR(ctx_, name_, fallback_) \
    DCC_PARAM_NUMBER_OR((ctx_), (name_), (fallback_))
#define DCC_OPTION_NUMBER(ctx_, name_) DCC_PARAM_NUMBER((ctx_), (name_))
#define DCC_OPTION_BOOL_OR(ctx_, name_, fallback_) \
    DCC_PARAM_BOOL_OR((ctx_), (name_), (fallback_))
#define DCC_OPTION_BOOL(ctx_, name_) DCC_PARAM_BOOL((ctx_), (name_))
#define DCC_OPTION_BOOLEAN_OR(ctx_, name_, fallback_) \
    DCC_PARAM_BOOLEAN_OR((ctx_), (name_), (fallback_))
#define DCC_OPTION_BOOLEAN(ctx_, name_) DCC_PARAM_BOOLEAN((ctx_), (name_))
#define DCC_OPTION_SNOWFLAKE_OR(ctx_, name_, fallback_) \
    DCC_PARAM_SNOWFLAKE_OR((ctx_), (name_), (fallback_))
#define DCC_OPTION_SNOWFLAKE(ctx_, name_) DCC_PARAM_SNOWFLAKE((ctx_), (name_))
#define DCC_OPTION_USER(ctx_, name_) DCC_PARAM_USER((ctx_), (name_))
#define DCC_OPTION_MEMBER(ctx_, name_) DCC_PARAM_MEMBER((ctx_), (name_))
#define DCC_OPTION_ROLE(ctx_, name_) DCC_PARAM_ROLE((ctx_), (name_))
#define DCC_OPTION_CHANNEL(ctx_, name_) DCC_PARAM_CHANNEL((ctx_), (name_))
#define DCC_OPTION_MESSAGE(ctx_, name_) DCC_PARAM_MESSAGE((ctx_), (name_))
#define DCC_OPTION_ATTACHMENT(ctx_, name_) DCC_PARAM_ATTACHMENT((ctx_), (name_))
#define DCC_OPTION_FOCUSED(ctx_) DCC_PARAM_FOCUSED((ctx_))
#define DCC_OPTION_FOCUSED_NAME(ctx_) DCC_PARAM_FOCUSED_NAME((ctx_))
#define DCC_OPTION_FOCUSED_STRING_OR(ctx_, fallback_) \
    DCC_PARAM_FOCUSED_STRING_OR((ctx_), (fallback_))
#define DCC_OPTION_FOCUSED_STRING(ctx_) DCC_PARAM_FOCUSED_STRING((ctx_))

#define DCC_CTX_OPTION(ctx_, name_) DCC_CTX_ARG((ctx_), (name_))
#define DCC_CTX_OPTION_PRESENT(ctx_, name_) DCC_CTX_ARG_PRESENT((ctx_), (name_))
#define DCC_CTX_OPTION_TEXT_OR(ctx_, name_, fallback_) \
    DCC_CTX_ARG_TEXT_OR((ctx_), (name_), (fallback_))
#define DCC_CTX_OPTION_TEXT(ctx_, name_) DCC_CTX_ARG_TEXT((ctx_), (name_))
#define DCC_CTX_OPTION_INTEGER_OR(ctx_, name_, fallback_) \
    DCC_CTX_ARG_INTEGER_OR((ctx_), (name_), (fallback_))
#define DCC_CTX_OPTION_INTEGER(ctx_, name_) DCC_CTX_ARG_INTEGER((ctx_), (name_))
#define DCC_CTX_OPTION_BOOLEAN_OR(ctx_, name_, fallback_) \
    DCC_CTX_ARG_BOOLEAN_OR((ctx_), (name_), (fallback_))
#define DCC_CTX_OPTION_BOOLEAN(ctx_, name_) DCC_CTX_ARG_BOOLEAN((ctx_), (name_))
#define DCC_CTX_OPTION_MENTION(ctx_, name_) DCC_CTX_ARG_MENTION((ctx_), (name_))
#define DCC_CTX_OPTION_USER_MENTION(ctx_, name_) DCC_CTX_ARG_USER_MENTION((ctx_), (name_))
#define DCC_CTX_OPTION_MEMBER_MENTION(ctx_, name_) \
    DCC_CTX_ARG_MEMBER_MENTION((ctx_), (name_))
#define DCC_CTX_OPTION_ROLE_MENTION(ctx_, name_) DCC_CTX_ARG_ROLE_MENTION((ctx_), (name_))
#define DCC_CTX_OPTION_CHANNEL_MENTION(ctx_, name_) \
    DCC_CTX_ARG_CHANNEL_MENTION((ctx_), (name_))

#define DCC_OPTION_MENTION(ctx_, name_) \
    DCC_MENTION_USER(DCC_OPTION_SNOWFLAKE((ctx_), (name_)))
#define DCC_OPTION_USER_MENTION(ctx_, name_) \
    DCC_MENTION_USER(DCC_OPTION_SNOWFLAKE((ctx_), (name_)))
#define DCC_OPTION_MEMBER_MENTION(ctx_, name_) \
    DCC_MENTION_MEMBER(DCC_OPTION_SNOWFLAKE((ctx_), (name_)))
#define DCC_OPTION_ROLE_MENTION(ctx_, name_) \
    DCC_MENTION_ROLE(DCC_OPTION_SNOWFLAKE((ctx_), (name_)))
#define DCC_OPTION_CHANNEL_MENTION(ctx_, name_) \
    DCC_MENTION_CHANNEL(DCC_OPTION_SNOWFLAKE((ctx_), (name_)))
#define DCC_PARAM_MENTION(ctx_, name_) DCC_OPTION_MENTION((ctx_), (name_))
#define DCC_PARAM_USER_MENTION(ctx_, name_) DCC_OPTION_USER_MENTION((ctx_), (name_))
#define DCC_PARAM_MEMBER_MENTION(ctx_, name_) DCC_OPTION_MEMBER_MENTION((ctx_), (name_))
#define DCC_PARAM_ROLE_MENTION(ctx_, name_) DCC_OPTION_ROLE_MENTION((ctx_), (name_))
#define DCC_PARAM_CHANNEL_MENTION(ctx_, name_) DCC_OPTION_CHANNEL_MENTION((ctx_), (name_))


#endif /* DCC_SUGAR_CONTEXT_OPTIONS_H */
