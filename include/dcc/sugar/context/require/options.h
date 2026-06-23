#ifndef DCC_SUGAR_CONTEXT_REQUIRE_OPTIONS_H
#define DCC_SUGAR_CONTEXT_REQUIRE_OPTIONS_H

#include <dcc/sugar/context/require/core.h>

#define DCC_CTX_REQUIRE_ARG_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), DCC_CTX_ARG_PRESENT((ctx_), (name_)), (message_))
#define DCC_CTX_REQUIRE_ARG_TEXT_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), dcc_sugar_ctx_has_text(DCC_CTX_ARG_TEXT((ctx_), (name_))), (message_))
#define DCC_CTX_REQUIRE_ARG_STRING_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG_TEXT_BAD_INPUT((ctx_), (name_), (message_))
#define DCC_CTX_REQUIRE_OPTION_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG_BAD_INPUT((ctx_), (name_), (message_))
#define DCC_CTX_REQUIRE_OPTION_TEXT_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG_TEXT_BAD_INPUT((ctx_), (name_), (message_))
#define DCC_CTX_REQUIRE_OPTION_STRING_BAD_INPUT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG_STRING_BAD_INPUT((ctx_), (name_), (message_))
#define DCC_CTX_REQUIRE_STRING_OPTION(var_, ctx_, name_, message_) \
    const char *var_ = DCC_CTX_OPTION_TEXT((ctx_), (name_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), dcc_sugar_ctx_has_text(var_), (message_))
#define DCC_CTX_REQUIRE_TEXT_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_STRING_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_CTX_REQUIRE_INT_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT( \
        (ctx_), \
        dcc_sugar_ctx_option_has_type((ctx_), (name_), DCC_INTERACTION_OPTION_VALUE_INTEGER), \
        (message_) \
    ); \
    int64_t var_ = DCC_CTX_OPTION_INTEGER((ctx_), (name_))
#define DCC_CTX_REQUIRE_INTEGER_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_INT_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_CTX_REQUIRE_NUMBER_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT( \
        (ctx_), \
        dcc_sugar_ctx_option_has_type((ctx_), (name_), DCC_INTERACTION_OPTION_VALUE_NUMBER), \
        (message_) \
    ); \
    double var_ = DCC_CTX_ARG_NUMBER((ctx_), (name_))
#define DCC_CTX_REQUIRE_BOOL_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT( \
        (ctx_), \
        dcc_sugar_ctx_option_has_type((ctx_), (name_), DCC_INTERACTION_OPTION_VALUE_BOOLEAN), \
        (message_) \
    ); \
    uint8_t var_ = DCC_CTX_OPTION_BOOLEAN((ctx_), (name_))
#define DCC_CTX_REQUIRE_BOOLEAN_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_BOOL_OPTION(var_, (ctx_), (name_), (message_))
#define DCC_CTX_REQUIRE_SNOWFLAKE_OPTION(var_, ctx_, name_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT( \
        (ctx_), \
        dcc_sugar_ctx_option_has_type((ctx_), (name_), DCC_INTERACTION_OPTION_VALUE_SNOWFLAKE), \
        (message_) \
    ); \
    dcc_snowflake_t var_ = DCC_CTX_ARG_SNOWFLAKE((ctx_), (name_))
#define DCC_CTX_REQUIRE_USER_OPTION(var_, ctx_, name_, message_) \
    const dcc_user_t *var_ = DCC_CTX_OPTION_USER((ctx_), (name_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), var_ != NULL, (message_))
#define DCC_CTX_REQUIRE_MEMBER_OPTION(var_, ctx_, name_, message_) \
    const dcc_member_t *var_ = DCC_CTX_OPTION_MEMBER((ctx_), (name_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), var_ != NULL, (message_))
#define DCC_CTX_REQUIRE_ROLE_OPTION(var_, ctx_, name_, message_) \
    const dcc_role_t *var_ = DCC_CTX_OPTION_ROLE((ctx_), (name_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), var_ != NULL, (message_))
#define DCC_CTX_REQUIRE_CHANNEL_OPTION(var_, ctx_, name_, message_) \
    const dcc_channel_t *var_ = DCC_CTX_OPTION_CHANNEL((ctx_), (name_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), var_ != NULL, (message_))
#define DCC_CTX_REQUIRE_MESSAGE_OPTION(var_, ctx_, name_, message_) \
    const dcc_message_t *var_ = DCC_CTX_OPTION_MESSAGE((ctx_), (name_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), var_ != NULL, (message_))
#define DCC_CTX_REQUIRE_ATTACHMENT_OPTION(var_, ctx_, name_, message_) \
    const dcc_attachment_t *var_ = DCC_CTX_OPTION_ATTACHMENT((ctx_), (name_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), var_ != NULL, (message_))
#define DCC_CTX_REQUIRED_STRING_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_STRING_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_TEXT_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_TEXT_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_INT_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_INT_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_INTEGER_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_INTEGER_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_NUMBER_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_NUMBER_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_BOOL_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_BOOL_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_BOOLEAN_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_BOOLEAN_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_SNOWFLAKE_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_SNOWFLAKE_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_USER_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_USER_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_MEMBER_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_MEMBER_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_ROLE_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_ROLE_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_CHANNEL_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_CHANNEL_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_MESSAGE_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_MESSAGE_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_ATTACHMENT_OPTION(var_, ctx_, name_) \
    DCC_CTX_REQUIRE_ATTACHMENT_OPTION(var_, (ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)

#endif /* DCC_SUGAR_CONTEXT_REQUIRE_OPTIONS_H */
