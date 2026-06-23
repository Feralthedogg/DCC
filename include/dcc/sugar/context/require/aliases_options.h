#ifndef DCC_SUGAR_CONTEXT_REQUIRE_ALIASES_OPTIONS_H
#define DCC_SUGAR_CONTEXT_REQUIRE_ALIASES_OPTIONS_H

#include <dcc/sugar/context/require/options.h>

#define DCC_REQUIRE_ARG_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG_BAD_INPUT((ctx_), (name_), (message_))
#define DCC_REQUIRE_ARG_TEXT_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG_TEXT_BAD_INPUT((ctx_), (name_), (message_))
#define DCC_REQUIRE_ARG_STRING_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG_STRING_BAD_INPUT((ctx_), (name_), (message_))
#define DCC_REQUIRE_OPTION_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_OPTION_BAD_INPUT((ctx_), (name_), (message_))
#define DCC_REQUIRE_OPTION_TEXT_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_OPTION_TEXT_BAD_INPUT((ctx_), (name_), (message_))
#define DCC_REQUIRE_OPTION_STRING_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_OPTION_STRING_BAD_INPUT((ctx_), (name_), (message_))
#define DCC_REQUIRE_STRING_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_STRING_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_TEXT_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_TEXT_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_INT_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_INT_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_INTEGER_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_INTEGER_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_NUMBER_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_NUMBER_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_BOOL_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_BOOL_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_BOOLEAN_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_BOOLEAN_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_SNOWFLAKE_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_SNOWFLAKE_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_USER_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_USER_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_MEMBER_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_MEMBER_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_ROLE_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ROLE_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_CHANNEL_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_CHANNEL_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_MESSAGE_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_MESSAGE_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRE_ATTACHMENT_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ATTACHMENT_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_REQUIRED_STRING_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_STRING_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_TEXT_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_TEXT_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_INT_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_INT_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_INTEGER_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_INTEGER_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_NUMBER_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_NUMBER_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_BOOL_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_BOOL_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_BOOLEAN_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_BOOLEAN_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_SNOWFLAKE_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_SNOWFLAKE_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_USER_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_USER_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_MEMBER_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_MEMBER_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_ROLE_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_ROLE_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_CHANNEL_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_CHANNEL_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_MESSAGE_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_MESSAGE_OPTION(var_, (ctx_), (name_))
#define DCC_REQUIRED_ATTACHMENT_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRED_ATTACHMENT_OPTION(var_, (ctx_), (name_))

#endif /* DCC_SUGAR_CONTEXT_REQUIRE_ALIASES_OPTIONS_H */
