#ifndef DCC_SUGAR_CONTEXT_REQUIRE_H
#define DCC_SUGAR_CONTEXT_REQUIRE_H

#include <dcc/sugar/context/forms.h>

#ifndef DCC_CTX_REQUIRED_OPTION_MESSAGE
#define DCC_CTX_REQUIRED_OPTION_MESSAGE "Missing option."
#endif
#ifndef DCC_CTX_REQUIRED_GUILD_MESSAGE
#define DCC_CTX_REQUIRED_GUILD_MESSAGE "This command can only be used in a server."
#endif
#ifndef DCC_CTX_REQUIRED_DM_MESSAGE
#define DCC_CTX_REQUIRED_DM_MESSAGE "This command can only be used in DMs."
#endif
#ifndef DCC_CTX_REQUIRED_AUTHOR_MESSAGE
#define DCC_CTX_REQUIRED_AUTHOR_MESSAGE "Only the original user can use this."
#endif
#ifndef DCC_CTX_REQUIRED_PERMISSION_MESSAGE
#define DCC_CTX_REQUIRED_PERMISSION_MESSAGE "Missing required permissions."
#endif
#ifndef DCC_CTX_REQUIRED_ROLE_MESSAGE
#define DCC_CTX_REQUIRED_ROLE_MESSAGE "Missing required role."
#endif
#ifndef DCC_CTX_REQUIRED_TEXT_MESSAGE
#define DCC_CTX_REQUIRED_TEXT_MESSAGE "Missing text."
#endif
#ifndef DCC_CTX_REQUIRED_FIELD_MESSAGE
#define DCC_CTX_REQUIRED_FIELD_MESSAGE "Missing field."
#endif
#ifndef DCC_CTX_REQUIRED_SELECT_MESSAGE
#define DCC_CTX_REQUIRED_SELECT_MESSAGE "Choose a value."
#endif
#ifndef DCC_CTX_REQUIRED_COMPONENT_MESSAGE
#define DCC_CTX_REQUIRED_COMPONENT_MESSAGE "Unknown component."
#endif
#ifndef DCC_CTX_REQUIRED_ACTION_MESSAGE
#define DCC_CTX_REQUIRED_ACTION_MESSAGE "Unknown component action."
#endif

#define DCC_CTX_REQUIRE_SELECTED_FIRST(var_, ctx_, message_) \
    const char *var_ = DCC_CTX_SELECTED_FIRST((ctx_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), dcc_sugar_ctx_has_text(var_), (message_))
#define DCC_CTX_REQUIRE_SELECT_FIRST(var_, ctx_, message_) \
    DCC_CTX_REQUIRE_SELECTED_FIRST(var_, (ctx_), (message_))
#define DCC_CTX_REQUIRE_SELECTED_VALUE(var_, ctx_, index_, message_) \
    const char *var_ = DCC_CTX_SELECTED_VALUE((ctx_), (index_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), dcc_sugar_ctx_has_text(var_), (message_))
#define DCC_CTX_REQUIRE_SELECT_VALUE(var_, ctx_, index_, message_) \
    DCC_CTX_REQUIRE_SELECTED_VALUE(var_, (ctx_), (index_), (message_))
#define DCC_CTX_REQUIRE_SELECTED_VALUES(values_var_, count_var_, ctx_, message_) \
    size_t count_var_ = 0U; \
    const char **values_var_ = DCC_CTX_SELECTED_VALUES((ctx_), &count_var_); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), values_var_ != NULL && count_var_ != 0U, (message_))
#define DCC_CTX_REQUIRE_SELECT_VALUES(values_var_, count_var_, ctx_, message_) \
    DCC_CTX_REQUIRE_SELECTED_VALUES(values_var_, count_var_, (ctx_), (message_))
#define DCC_REQUIRE_SELECTED_FIRST(var_, ctx_, message_) \
    DCC_CTX_REQUIRE_SELECTED_FIRST(var_, (ctx_), (message_))
#define DCC_REQUIRE_SELECT_FIRST(var_, ctx_, message_) \
    DCC_CTX_REQUIRE_SELECT_FIRST(var_, (ctx_), (message_))
#define DCC_REQUIRE_SELECTED_VALUE(var_, ctx_, index_, message_) \
    DCC_CTX_REQUIRE_SELECTED_VALUE(var_, (ctx_), (index_), (message_))
#define DCC_REQUIRE_SELECT_VALUE(var_, ctx_, index_, message_) \
    DCC_CTX_REQUIRE_SELECT_VALUE(var_, (ctx_), (index_), (message_))
#define DCC_REQUIRE_SELECTED_VALUES(values_var_, count_var_, ctx_, message_) \
    DCC_CTX_REQUIRE_SELECTED_VALUES(values_var_, count_var_, (ctx_), (message_))
#define DCC_REQUIRE_SELECT_VALUES(values_var_, count_var_, ctx_, message_) \
    DCC_CTX_REQUIRE_SELECT_VALUES(values_var_, count_var_, (ctx_), (message_))
#define DCC_CTX_REQUIRED_SELECTED_FIRST(var_, ctx_) \
    DCC_CTX_REQUIRE_SELECTED_FIRST(var_, (ctx_), DCC_CTX_REQUIRED_SELECT_MESSAGE)
#define DCC_CTX_REQUIRED_SELECT_FIRST(var_, ctx_) \
    DCC_CTX_REQUIRE_SELECT_FIRST(var_, (ctx_), DCC_CTX_REQUIRED_SELECT_MESSAGE)
#define DCC_CTX_REQUIRED_SELECTED_VALUE(var_, ctx_, index_) \
    DCC_CTX_REQUIRE_SELECTED_VALUE(var_, (ctx_), (index_), DCC_CTX_REQUIRED_SELECT_MESSAGE)
#define DCC_CTX_REQUIRED_SELECT_VALUE(var_, ctx_, index_) \
    DCC_CTX_REQUIRE_SELECT_VALUE(var_, (ctx_), (index_), DCC_CTX_REQUIRED_SELECT_MESSAGE)
#define DCC_CTX_REQUIRED_SELECTED_VALUES(values_var_, count_var_, ctx_) \
    DCC_CTX_REQUIRE_SELECTED_VALUES(values_var_, count_var_, (ctx_), DCC_CTX_REQUIRED_SELECT_MESSAGE)
#define DCC_CTX_REQUIRED_SELECT_VALUES(values_var_, count_var_, ctx_) \
    DCC_CTX_REQUIRE_SELECT_VALUES(values_var_, count_var_, (ctx_), DCC_CTX_REQUIRED_SELECT_MESSAGE)
#define DCC_REQUIRED_SELECTED_FIRST(var_, ctx_) \
    DCC_CTX_REQUIRED_SELECTED_FIRST(var_, (ctx_))
#define DCC_REQUIRED_SELECT_FIRST(var_, ctx_) \
    DCC_CTX_REQUIRED_SELECT_FIRST(var_, (ctx_))
#define DCC_REQUIRED_SELECTED_VALUE(var_, ctx_, index_) \
    DCC_CTX_REQUIRED_SELECTED_VALUE(var_, (ctx_), (index_))
#define DCC_REQUIRED_SELECT_VALUE(var_, ctx_, index_) \
    DCC_CTX_REQUIRED_SELECT_VALUE(var_, (ctx_), (index_))
#define DCC_REQUIRED_SELECTED_VALUES(values_var_, count_var_, ctx_) \
    DCC_CTX_REQUIRED_SELECTED_VALUES(values_var_, count_var_, (ctx_))
#define DCC_REQUIRED_SELECT_VALUES(values_var_, count_var_, ctx_) \
    DCC_CTX_REQUIRED_SELECT_VALUES(values_var_, count_var_, (ctx_))

#define DCC_CTX_REQUIRE(ctx_, condition_, message_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_REPLY_EPHEMERAL((ctx_), (message_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_BAD_INPUT(ctx_, condition_, message_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_BAD_INPUT((ctx_), (message_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_BAD_INPUT_F(ctx_, condition_, ...) \
    do { \
        if (!(condition_)) { \
            (void)DCC_CTX_BAD_INPUT_F((ctx_), __VA_ARGS__); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_DENY(ctx_, condition_, message_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_DENY((ctx_), (message_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_DENY_F(ctx_, condition_, ...) \
    do { \
        if (!(condition_)) { \
            (void)DCC_CTX_DENY_F((ctx_), __VA_ARGS__); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_NOT_FOUND(ctx_, condition_, message_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_NOT_FOUND((ctx_), (message_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_NOT_FOUND_F(ctx_, condition_, ...) \
    do { \
        if (!(condition_)) { \
            (void)DCC_CTX_NOT_FOUND_F((ctx_), __VA_ARGS__); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_BUSY(ctx_, condition_, message_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_BUSY((ctx_), (message_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_BUSY_F(ctx_, condition_, ...) \
    do { \
        if (!(condition_)) { \
            (void)DCC_CTX_BUSY_F((ctx_), __VA_ARGS__); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_FAIL(ctx_, condition_, message_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_FAIL((ctx_), (message_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_FAIL_F(ctx_, condition_, ...) \
    do { \
        if (!(condition_)) { \
            (void)DCC_CTX_FAIL_F((ctx_), __VA_ARGS__); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_INTERNAL(ctx_, condition_) \
    do { \
        if (!(condition_)) { \
            (void)DCC_INTERNAL_ERROR((ctx_)); \
            return; \
        } \
    } while (0)

#define DCC_CTX_REQUIRE_GUILD(ctx_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_IS_GUILD((ctx_)), (message_))
#define DCC_CTX_REQUIRE_DM(ctx_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_IS_DM((ctx_)), (message_))
#define DCC_CTX_REQUIRE_AUTHOR(ctx_, user_id_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_IS_AUTHOR((ctx_), (user_id_)), (message_))
#define DCC_CTX_REQUIRE_PERMISSIONS(ctx_, permissions_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_MEMBER_HAS_PERMISSIONS((ctx_), (permissions_)), (message_))
#define DCC_CTX_REQUIRE_ROLE(ctx_, role_id_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_MEMBER_HAS_ROLE((ctx_), (role_id_)), (message_))
#define DCC_CTX_REQUIRE_ANY_ROLE(ctx_, role_ids_, role_id_count_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_MEMBER_HAS_ANY_ROLE((ctx_), (role_ids_), (role_id_count_)), (message_))
#define DCC_CTX_REQUIRE_ALL_ROLES(ctx_, role_ids_, role_id_count_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_MEMBER_HAS_ALL_ROLES((ctx_), (role_ids_), (role_id_count_)), (message_))
#define DCC_CTX_REQUIRE_ANY_ROLE_IDS(ctx_, message_, ...) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_MEMBER_HAS_ANY_ROLE_IDS((ctx_), __VA_ARGS__), (message_))
#define DCC_CTX_REQUIRE_ALL_ROLE_IDS(ctx_, message_, ...) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_MEMBER_HAS_ALL_ROLE_IDS((ctx_), __VA_ARGS__), (message_))
#define DCC_CTX_REQUIRE_TEXT(ctx_, value_, message_) \
    DCC_CTX_REQUIRE((ctx_), dcc_sugar_ctx_has_text((value_)), (message_))
#define DCC_CTX_REQUIRE_OPTION(ctx_, name_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_OPT_PRESENT((ctx_), (name_)), (message_))
#define DCC_CTX_REQUIRE_ARG(ctx_, name_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_ARG_PRESENT((ctx_), (name_)), (message_))
#define DCC_CTX_REQUIRE_ARG_TEXT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE((ctx_), dcc_sugar_ctx_has_text(DCC_CTX_ARG_TEXT((ctx_), (name_))), (message_))
#define DCC_CTX_REQUIRE_ARG_STRING(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG_TEXT((ctx_), (name_), (message_))
#define DCC_CTX_REQUIRE_FIELD(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE((ctx_), dcc_sugar_ctx_has_text(DCC_FIELD_TEXT((ctx_), (custom_id_))), (message_))
#define DCC_CTX_REQUIRE_FORM(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_FORM_PRESENT((ctx_), (custom_id_)), (message_))
#define DCC_CTX_REQUIRE_FORM_TEXT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE((ctx_), dcc_sugar_ctx_has_text(DCC_CTX_FORM_TEXT_VALUE((ctx_), (custom_id_))), (message_))
#define DCC_CTX_REQUIRE_FORM_STRING(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM_TEXT((ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_SELECTED(ctx_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_SELECTED_COUNT((ctx_)) != 0U, (message_))
#define DCC_CTX_REQUIRE_SELECT(ctx_, message_) DCC_CTX_REQUIRE_SELECTED((ctx_), (message_))
#define DCC_CTX_REQUIRE_CUSTOM_ID(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_HAS_CUSTOM_ID((ctx_), (custom_id_)), (message_))
#define DCC_CTX_REQUIRE_CUSTOM_ID_PREFIX(ctx_, prefix_, message_) \
    DCC_CTX_REQUIRE((ctx_), DCC_CTX_CUSTOM_ID_STARTS_WITH((ctx_), (prefix_)), (message_))
#define DCC_CTX_REQUIRED_GUILD(ctx_) DCC_CTX_REQUIRE_GUILD((ctx_), DCC_CTX_REQUIRED_GUILD_MESSAGE)
#define DCC_CTX_REQUIRED_DM(ctx_) DCC_CTX_REQUIRE_DM((ctx_), DCC_CTX_REQUIRED_DM_MESSAGE)
#define DCC_CTX_REQUIRED_AUTHOR(ctx_, user_id_) \
    DCC_CTX_REQUIRE_AUTHOR((ctx_), (user_id_), DCC_CTX_REQUIRED_AUTHOR_MESSAGE)
#define DCC_CTX_REQUIRED_PERMISSIONS(ctx_, permissions_) \
    DCC_CTX_REQUIRE_PERMISSIONS((ctx_), (permissions_), DCC_CTX_REQUIRED_PERMISSION_MESSAGE)
#define DCC_CTX_REQUIRED_ROLE(ctx_, role_id_) \
    DCC_CTX_REQUIRE_ROLE((ctx_), (role_id_), DCC_CTX_REQUIRED_ROLE_MESSAGE)
#define DCC_CTX_REQUIRED_ANY_ROLE(ctx_, role_ids_, role_id_count_) \
    DCC_CTX_REQUIRE_ANY_ROLE((ctx_), (role_ids_), (role_id_count_), DCC_CTX_REQUIRED_ROLE_MESSAGE)
#define DCC_CTX_REQUIRED_ALL_ROLES(ctx_, role_ids_, role_id_count_) \
    DCC_CTX_REQUIRE_ALL_ROLES((ctx_), (role_ids_), (role_id_count_), DCC_CTX_REQUIRED_ROLE_MESSAGE)
#define DCC_CTX_REQUIRED_ANY_ROLE_IDS(ctx_, ...) \
    DCC_CTX_REQUIRE_ANY_ROLE_IDS((ctx_), DCC_CTX_REQUIRED_ROLE_MESSAGE, __VA_ARGS__)
#define DCC_CTX_REQUIRED_ALL_ROLE_IDS(ctx_, ...) \
    DCC_CTX_REQUIRE_ALL_ROLE_IDS((ctx_), DCC_CTX_REQUIRED_ROLE_MESSAGE, __VA_ARGS__)
#define DCC_CTX_REQUIRED_TEXT(ctx_, value_) \
    DCC_CTX_REQUIRE_TEXT((ctx_), (value_), DCC_CTX_REQUIRED_TEXT_MESSAGE)
#define DCC_CTX_REQUIRED_OPTION(ctx_, name_) \
    DCC_CTX_REQUIRE_OPTION((ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_ARG(ctx_, name_) \
    DCC_CTX_REQUIRE_ARG((ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_ARG_TEXT(ctx_, name_) \
    DCC_CTX_REQUIRE_ARG_TEXT((ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_ARG_STRING(ctx_, name_) \
    DCC_CTX_REQUIRE_ARG_STRING((ctx_), (name_), DCC_CTX_REQUIRED_OPTION_MESSAGE)
#define DCC_CTX_REQUIRED_FIELD(ctx_, custom_id_) \
    DCC_CTX_REQUIRE_FIELD((ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_FORM(ctx_, custom_id_) \
    DCC_CTX_REQUIRE_FORM((ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_FORM_TEXT(ctx_, custom_id_) \
    DCC_CTX_REQUIRE_FORM_TEXT((ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_FORM_STRING(ctx_, custom_id_) \
    DCC_CTX_REQUIRE_FORM_STRING((ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_SELECTED(ctx_) \
    DCC_CTX_REQUIRE_SELECTED((ctx_), DCC_CTX_REQUIRED_SELECT_MESSAGE)
#define DCC_CTX_REQUIRED_SELECT(ctx_) DCC_CTX_REQUIRED_SELECTED((ctx_))
#define DCC_CTX_REQUIRED_CUSTOM_ID(ctx_, custom_id_) \
    DCC_CTX_REQUIRE_CUSTOM_ID((ctx_), (custom_id_), DCC_CTX_REQUIRED_COMPONENT_MESSAGE)
#define DCC_CTX_REQUIRED_CUSTOM_ID_PREFIX(ctx_, prefix_) \
    DCC_CTX_REQUIRE_CUSTOM_ID_PREFIX((ctx_), (prefix_), DCC_CTX_REQUIRED_COMPONENT_MESSAGE)
#define DCC_CTX_REQUIRE_CUSTOM_ID_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), DCC_CTX_HAS_CUSTOM_ID((ctx_), (custom_id_)), (message_))
#define DCC_CTX_REQUIRE_CUSTOM_ID_PREFIX_BAD_INPUT(ctx_, prefix_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), DCC_CTX_CUSTOM_ID_STARTS_WITH((ctx_), (prefix_)), (message_))
#define DCC_CTX_REQUIRE_CUSTOM_ID_VALUE(var_, ctx_, message_) \
    const char *var_ = DCC_CTX_CUSTOM_ID((ctx_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), dcc_sugar_ctx_has_text(var_), (message_))
#define DCC_CTX_REQUIRE_CUSTOM_ID_SUFFIX(var_, ctx_, prefix_, message_) \
    const char *var_ = DCC_CTX_CUSTOM_ID_SUFFIX((ctx_), (prefix_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), dcc_sugar_ctx_has_text(var_), (message_))
#define DCC_CTX_REQUIRE_ACTION(var_, ctx_, prefix_, message_) \
    DCC_CTX_REQUIRE_CUSTOM_ID_SUFFIX(var_, (ctx_), (prefix_), (message_))
#define DCC_CTX_REQUIRE_COMPONENT_ACTION(var_, ctx_, namespace_, message_) \
    const char *var_ = DCC_CTX_COMPONENT_ACTION((ctx_), (namespace_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), dcc_sugar_ctx_has_text(var_), (message_))
#define DCC_CTX_REQUIRE_ACTION_IN_NS(var_, ctx_, namespace_, message_) \
    DCC_CTX_REQUIRE_COMPONENT_ACTION(var_, (ctx_), (namespace_), (message_))
#define DCC_CTX_REQUIRE_ACTION_IN_NAMESPACE(var_, ctx_, namespace_, message_) \
    DCC_CTX_REQUIRE_COMPONENT_ACTION(var_, (ctx_), (namespace_), (message_))
#define DCC_CTX_REQUIRE_COMPONENT_IS(ctx_, namespace_, action_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), DCC_CTX_COMPONENT_IS((ctx_), (namespace_), (action_)), (message_))
#define DCC_CTX_REQUIRED_CUSTOM_ID_VALUE(var_, ctx_) \
    DCC_CTX_REQUIRE_CUSTOM_ID_VALUE(var_, (ctx_), DCC_CTX_REQUIRED_COMPONENT_MESSAGE)
#define DCC_CTX_REQUIRED_CUSTOM_ID_SUFFIX(var_, ctx_, prefix_) \
    DCC_CTX_REQUIRE_CUSTOM_ID_SUFFIX(var_, (ctx_), (prefix_), DCC_CTX_REQUIRED_ACTION_MESSAGE)
#define DCC_CTX_REQUIRED_ACTION(var_, ctx_, prefix_) \
    DCC_CTX_REQUIRE_ACTION(var_, (ctx_), (prefix_), DCC_CTX_REQUIRED_ACTION_MESSAGE)
#define DCC_CTX_REQUIRED_COMPONENT_ACTION(var_, ctx_, namespace_) \
    DCC_CTX_REQUIRE_COMPONENT_ACTION(var_, (ctx_), (namespace_), DCC_CTX_REQUIRED_ACTION_MESSAGE)
#define DCC_CTX_REQUIRED_ACTION_IN_NS(var_, ctx_, namespace_) \
    DCC_CTX_REQUIRED_COMPONENT_ACTION(var_, (ctx_), (namespace_))
#define DCC_CTX_REQUIRED_ACTION_IN_NAMESPACE(var_, ctx_, namespace_) \
    DCC_CTX_REQUIRED_COMPONENT_ACTION(var_, (ctx_), (namespace_))
#define DCC_CTX_REQUIRED_COMPONENT_IS(ctx_, namespace_, action_) \
    DCC_CTX_REQUIRE_COMPONENT_IS((ctx_), (namespace_), (action_), DCC_CTX_REQUIRED_ACTION_MESSAGE)
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
#define DCC_CTX_REQUIRE_FORM_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), DCC_CTX_FORM_PRESENT((ctx_), (custom_id_)), (message_))
#define DCC_CTX_REQUIRE_FORM_TEXT_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT( \
        (ctx_), \
        dcc_sugar_ctx_has_text(DCC_CTX_FORM_TEXT_VALUE((ctx_), (custom_id_))), \
        (message_) \
    )
#define DCC_CTX_REQUIRE_FORM_STRING_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM_TEXT_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_FIELD_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_FIELD_TEXT_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM_TEXT_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_FIELD_STRING_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM_STRING_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_STRING_FIELD(var_, ctx_, custom_id_, message_) \
    const char *var_ = DCC_CTX_FIELD_TEXT((ctx_), (custom_id_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), dcc_sugar_ctx_has_text(var_), (message_))
#define DCC_CTX_REQUIRE_TEXT_FIELD(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_STRING_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_STRING_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_STRING_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_TEXT_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_TEXT_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_BOOL_FIELD(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT( \
        (ctx_), \
        dcc_sugar_ctx_form_has_type((ctx_), (custom_id_), DCC_INTERACTION_FORM_VALUE_BOOLEAN), \
        (message_) \
    ); \
    uint8_t var_ = DCC_CTX_FIELD_BOOL((ctx_), (custom_id_))
#define DCC_CTX_REQUIRE_BOOLEAN_FIELD(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BOOL_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_CHECKBOX_FIELD(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BOOL_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_BOOL_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BOOL_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_BOOLEAN_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BOOLEAN_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_CHECKBOX_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_CHECKBOX_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_VALUES_FIELD(values_var_, count_var_, ctx_, custom_id_, message_) \
    size_t count_var_ = 0U; \
    const char **values_var_ = DCC_CTX_FIELD_VALUES((ctx_), (custom_id_), &count_var_); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), values_var_ != NULL && count_var_ != 0U, (message_))
#define DCC_CTX_REQUIRE_VALUES_FORM(values_var_, count_var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_VALUES_FIELD(values_var_, count_var_, (ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_FIRST_VALUE_FIELD(var_, ctx_, custom_id_, message_) \
    const char *var_ = DCC_CTX_FIELD_FIRST_VALUE((ctx_), (custom_id_)); \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), dcc_sugar_ctx_has_text(var_), (message_))
#define DCC_CTX_REQUIRE_FIRST_VALUE_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FIRST_VALUE_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_CTX_REQUIRE_SELECT_BAD_INPUT(ctx_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), DCC_CTX_SELECTED_COUNT((ctx_)) != 0U, (message_))
#define DCC_CTX_REQUIRED_STRING_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_STRING_FIELD(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_TEXT_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_TEXT_FIELD(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_STRING_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_STRING_FORM(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_TEXT_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_TEXT_FORM(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_BOOL_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_BOOL_FIELD(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_BOOLEAN_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_BOOLEAN_FIELD(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_CHECKBOX_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_CHECKBOX_FIELD(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_BOOL_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_BOOL_FORM(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_BOOLEAN_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_BOOLEAN_FORM(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_CHECKBOX_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_CHECKBOX_FORM(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_FIELD_MESSAGE)
#define DCC_CTX_REQUIRED_VALUES_FIELD(values_var_, count_var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_VALUES_FIELD(values_var_, count_var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_SELECT_MESSAGE)
#define DCC_CTX_REQUIRED_VALUES_FORM(values_var_, count_var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_VALUES_FORM(values_var_, count_var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_SELECT_MESSAGE)
#define DCC_CTX_REQUIRED_FIRST_VALUE_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_FIRST_VALUE_FIELD(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_SELECT_MESSAGE)
#define DCC_CTX_REQUIRED_FIRST_VALUE_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRE_FIRST_VALUE_FORM(var_, (ctx_), (custom_id_), DCC_CTX_REQUIRED_SELECT_MESSAGE)

#define DCC_REQUIRE(ctx_, condition_, message_) \
    DCC_CTX_REQUIRE((ctx_), (condition_), (message_))
#define DCC_REQUIRE_BAD_INPUT(ctx_, condition_, message_) \
    DCC_CTX_REQUIRE_BAD_INPUT((ctx_), (condition_), (message_))
#define DCC_REQUIRE_BAD_INPUT_F(ctx_, condition_, ...) \
    DCC_CTX_REQUIRE_BAD_INPUT_F((ctx_), (condition_), __VA_ARGS__)
#define DCC_REQUIRE_DENY(ctx_, condition_, message_) \
    DCC_CTX_REQUIRE_DENY((ctx_), (condition_), (message_))
#define DCC_REQUIRE_DENY_F(ctx_, condition_, ...) \
    DCC_CTX_REQUIRE_DENY_F((ctx_), (condition_), __VA_ARGS__)
#define DCC_REQUIRE_NOT_FOUND(ctx_, condition_, message_) \
    DCC_CTX_REQUIRE_NOT_FOUND((ctx_), (condition_), (message_))
#define DCC_REQUIRE_NOT_FOUND_F(ctx_, condition_, ...) \
    DCC_CTX_REQUIRE_NOT_FOUND_F((ctx_), (condition_), __VA_ARGS__)
#define DCC_REQUIRE_BUSY(ctx_, condition_, message_) \
    DCC_CTX_REQUIRE_BUSY((ctx_), (condition_), (message_))
#define DCC_REQUIRE_BUSY_F(ctx_, condition_, ...) \
    DCC_CTX_REQUIRE_BUSY_F((ctx_), (condition_), __VA_ARGS__)
#define DCC_REQUIRE_FAIL(ctx_, condition_, message_) \
    DCC_CTX_REQUIRE_FAIL((ctx_), (condition_), (message_))
#define DCC_REQUIRE_FAIL_F(ctx_, condition_, ...) \
    DCC_CTX_REQUIRE_FAIL_F((ctx_), (condition_), __VA_ARGS__)
#define DCC_REQUIRE_INTERNAL(ctx_, condition_) \
    DCC_CTX_REQUIRE_INTERNAL((ctx_), (condition_))
#define DCC_REQUIRE_GUILD(ctx_, message_) DCC_CTX_REQUIRE_GUILD((ctx_), (message_))
#define DCC_REQUIRE_DM(ctx_, message_) DCC_CTX_REQUIRE_DM((ctx_), (message_))
#define DCC_REQUIRE_AUTHOR(ctx_, user_id_, message_) \
    DCC_CTX_REQUIRE_AUTHOR((ctx_), (user_id_), (message_))
#define DCC_REQUIRE_PERMISSIONS(ctx_, permissions_, message_) \
    DCC_CTX_REQUIRE_PERMISSIONS((ctx_), (permissions_), (message_))
#define DCC_REQUIRE_ROLE(ctx_, role_id_, message_) \
    DCC_CTX_REQUIRE_ROLE((ctx_), (role_id_), (message_))
#define DCC_REQUIRE_ANY_ROLE(ctx_, role_ids_, role_id_count_, message_) \
    DCC_CTX_REQUIRE_ANY_ROLE((ctx_), (role_ids_), (role_id_count_), (message_))
#define DCC_REQUIRE_ALL_ROLES(ctx_, role_ids_, role_id_count_, message_) \
    DCC_CTX_REQUIRE_ALL_ROLES((ctx_), (role_ids_), (role_id_count_), (message_))
#define DCC_REQUIRE_ANY_ROLE_IDS(ctx_, message_, ...) \
    DCC_CTX_REQUIRE_ANY_ROLE_IDS((ctx_), (message_), __VA_ARGS__)
#define DCC_REQUIRE_ALL_ROLE_IDS(ctx_, message_, ...) \
    DCC_CTX_REQUIRE_ALL_ROLE_IDS((ctx_), (message_), __VA_ARGS__)
#define DCC_REQUIRE_TEXT(ctx_, value_, message_) \
    DCC_CTX_REQUIRE_TEXT((ctx_), (value_), (message_))
#define DCC_REQUIRE_OPTION(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_OPTION((ctx_), (name_), (message_))
#define DCC_REQUIRE_ARG(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG((ctx_), (name_), (message_))
#define DCC_REQUIRE_ARG_TEXT(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG_TEXT((ctx_), (name_), (message_))
#define DCC_REQUIRE_ARG_STRING(ctx_, name_, message_) \
    DCC_CTX_REQUIRE_ARG_STRING((ctx_), (name_), (message_))
#define DCC_REQUIRE_FIELD(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FIELD((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_FORM(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_FORM_TEXT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM_TEXT((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_FORM_STRING(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM_STRING((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_SELECTED(ctx_, message_) DCC_CTX_REQUIRE_SELECTED((ctx_), (message_))
#define DCC_REQUIRE_SELECT(ctx_, message_) DCC_CTX_REQUIRE_SELECT((ctx_), (message_))
#define DCC_REQUIRE_CUSTOM_ID(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_CUSTOM_ID((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_CUSTOM_ID_PREFIX(ctx_, prefix_, message_) \
    DCC_CTX_REQUIRE_CUSTOM_ID_PREFIX((ctx_), (prefix_), (message_))
#define DCC_REQUIRED_GUILD(ctx_) DCC_CTX_REQUIRED_GUILD((ctx_))
#define DCC_REQUIRED_DM(ctx_) DCC_CTX_REQUIRED_DM((ctx_))
#define DCC_REQUIRED_AUTHOR(ctx_, user_id_) DCC_CTX_REQUIRED_AUTHOR((ctx_), (user_id_))
#define DCC_REQUIRED_PERMISSIONS(ctx_, permissions_) \
    DCC_CTX_REQUIRED_PERMISSIONS((ctx_), (permissions_))
#define DCC_REQUIRED_ANY_ROLE(ctx_, role_ids_, role_id_count_) \
    DCC_CTX_REQUIRED_ANY_ROLE((ctx_), (role_ids_), (role_id_count_))
#define DCC_REQUIRED_ALL_ROLES(ctx_, role_ids_, role_id_count_) \
    DCC_CTX_REQUIRED_ALL_ROLES((ctx_), (role_ids_), (role_id_count_))
#define DCC_REQUIRED_ANY_ROLE_IDS(ctx_, ...) \
    DCC_CTX_REQUIRED_ANY_ROLE_IDS((ctx_), __VA_ARGS__)
#define DCC_REQUIRED_ALL_ROLE_IDS(ctx_, ...) \
    DCC_CTX_REQUIRED_ALL_ROLE_IDS((ctx_), __VA_ARGS__)
#define DCC_REQUIRED_TEXT(ctx_, value_) DCC_CTX_REQUIRED_TEXT((ctx_), (value_))
#define DCC_REQUIRED_OPTION(ctx_, name_) DCC_CTX_REQUIRED_OPTION((ctx_), (name_))
#define DCC_REQUIRED_ARG(ctx_, name_) DCC_CTX_REQUIRED_ARG((ctx_), (name_))
#define DCC_REQUIRED_ARG_TEXT(ctx_, name_) DCC_CTX_REQUIRED_ARG_TEXT((ctx_), (name_))
#define DCC_REQUIRED_ARG_STRING(ctx_, name_) DCC_CTX_REQUIRED_ARG_STRING((ctx_), (name_))
#define DCC_REQUIRED_FIELD(ctx_, custom_id_) DCC_CTX_REQUIRED_FIELD((ctx_), (custom_id_))
#define DCC_REQUIRED_FORM(ctx_, custom_id_) DCC_CTX_REQUIRED_FORM((ctx_), (custom_id_))
#define DCC_REQUIRED_FORM_TEXT(ctx_, custom_id_) \
    DCC_CTX_REQUIRED_FORM_TEXT((ctx_), (custom_id_))
#define DCC_REQUIRED_FORM_STRING(ctx_, custom_id_) \
    DCC_CTX_REQUIRED_FORM_STRING((ctx_), (custom_id_))
#define DCC_REQUIRED_SELECTED(ctx_) DCC_CTX_REQUIRED_SELECTED((ctx_))
#define DCC_REQUIRED_SELECT(ctx_) DCC_CTX_REQUIRED_SELECT((ctx_))
#define DCC_REQUIRED_CUSTOM_ID(ctx_, custom_id_) \
    DCC_CTX_REQUIRED_CUSTOM_ID((ctx_), (custom_id_))
#define DCC_REQUIRED_CUSTOM_ID_PREFIX(ctx_, prefix_) \
    DCC_CTX_REQUIRED_CUSTOM_ID_PREFIX((ctx_), (prefix_))
#define DCC_REQUIRE_CUSTOM_ID_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_CUSTOM_ID_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_CUSTOM_ID_PREFIX_BAD_INPUT(ctx_, prefix_, message_) \
    DCC_CTX_REQUIRE_CUSTOM_ID_PREFIX_BAD_INPUT((ctx_), (prefix_), (message_))
#define DCC_REQUIRE_CUSTOM_ID_VALUE(var_, ctx_, message_) \
    DCC_CTX_REQUIRE_CUSTOM_ID_VALUE(var_, (ctx_), (message_))
#define DCC_REQUIRE_CUSTOM_ID_SUFFIX(var_, ctx_, prefix_, message_) \
    DCC_CTX_REQUIRE_CUSTOM_ID_SUFFIX(var_, (ctx_), (prefix_), (message_))
#define DCC_REQUIRE_ACTION(var_, ctx_, prefix_, message_) \
    DCC_CTX_REQUIRE_ACTION(var_, (ctx_), (prefix_), (message_))
#define DCC_REQUIRE_COMPONENT_ACTION(var_, ctx_, namespace_, message_) \
    DCC_CTX_REQUIRE_COMPONENT_ACTION(var_, (ctx_), (namespace_), (message_))
#define DCC_REQUIRE_ACTION_IN_NS(var_, ctx_, namespace_, message_) \
    DCC_CTX_REQUIRE_ACTION_IN_NS(var_, (ctx_), (namespace_), (message_))
#define DCC_REQUIRE_ACTION_IN_NAMESPACE(var_, ctx_, namespace_, message_) \
    DCC_CTX_REQUIRE_ACTION_IN_NAMESPACE(var_, (ctx_), (namespace_), (message_))
#define DCC_REQUIRE_COMPONENT_IS(ctx_, namespace_, action_, message_) \
    DCC_CTX_REQUIRE_COMPONENT_IS((ctx_), (namespace_), (action_), (message_))
#define DCC_REQUIRED_CUSTOM_ID_VALUE(var_, ctx_) \
    DCC_CTX_REQUIRED_CUSTOM_ID_VALUE(var_, (ctx_))
#define DCC_REQUIRED_CUSTOM_ID_SUFFIX(var_, ctx_, prefix_) \
    DCC_CTX_REQUIRED_CUSTOM_ID_SUFFIX(var_, (ctx_), (prefix_))
#define DCC_REQUIRED_ACTION(var_, ctx_, prefix_) \
    DCC_CTX_REQUIRED_ACTION(var_, (ctx_), (prefix_))
#define DCC_REQUIRED_COMPONENT_ACTION(var_, ctx_, namespace_) \
    DCC_CTX_REQUIRED_COMPONENT_ACTION(var_, (ctx_), (namespace_))
#define DCC_REQUIRED_ACTION_IN_NS(var_, ctx_, namespace_) \
    DCC_CTX_REQUIRED_ACTION_IN_NS(var_, (ctx_), (namespace_))
#define DCC_REQUIRED_ACTION_IN_NAMESPACE(var_, ctx_, namespace_) \
    DCC_CTX_REQUIRED_ACTION_IN_NAMESPACE(var_, (ctx_), (namespace_))
#define DCC_REQUIRED_COMPONENT_IS(ctx_, namespace_, action_) \
    DCC_CTX_REQUIRED_COMPONENT_IS((ctx_), (namespace_), (action_))
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
#define DCC_REQUIRE_FORM_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_FORM_TEXT_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM_TEXT_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_FORM_STRING_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FORM_STRING_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_FIELD_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FIELD_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_FIELD_TEXT_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FIELD_TEXT_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_FIELD_STRING_BAD_INPUT(ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FIELD_STRING_BAD_INPUT((ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_STRING_FIELD(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_STRING_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_TEXT_FIELD(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_TEXT_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_STRING_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_STRING_FORM(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_TEXT_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_TEXT_FORM(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_BOOL_FIELD(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BOOL_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_BOOLEAN_FIELD(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BOOLEAN_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_CHECKBOX_FIELD(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_CHECKBOX_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_BOOL_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BOOL_FORM(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_BOOLEAN_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_BOOLEAN_FORM(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_CHECKBOX_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_CHECKBOX_FORM(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_VALUES_FIELD(values_var_, count_var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_VALUES_FIELD(values_var_, count_var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_VALUES_FORM(values_var_, count_var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_VALUES_FORM(values_var_, count_var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_FIRST_VALUE_FIELD(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FIRST_VALUE_FIELD(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_FIRST_VALUE_FORM(var_, ctx_, custom_id_, message_) \
    DCC_CTX_REQUIRE_FIRST_VALUE_FORM(var_, (ctx_), (custom_id_), (message_))
#define DCC_REQUIRE_SELECT_BAD_INPUT(ctx_, message_) \
    DCC_CTX_REQUIRE_SELECT_BAD_INPUT((ctx_), (message_))
#define DCC_REQUIRED_STRING_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_STRING_FIELD(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_TEXT_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_TEXT_FIELD(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_STRING_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_STRING_FORM(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_TEXT_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_TEXT_FORM(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_BOOL_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_BOOL_FIELD(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_BOOLEAN_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_BOOLEAN_FIELD(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_CHECKBOX_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_CHECKBOX_FIELD(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_BOOL_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_BOOL_FORM(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_BOOLEAN_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_BOOLEAN_FORM(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_CHECKBOX_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_CHECKBOX_FORM(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_VALUES_FIELD(values_var_, count_var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_VALUES_FIELD(values_var_, count_var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_VALUES_FORM(values_var_, count_var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_VALUES_FORM(values_var_, count_var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_FIRST_VALUE_FIELD(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_FIRST_VALUE_FIELD(var_, (ctx_), (custom_id_))
#define DCC_REQUIRED_FIRST_VALUE_FORM(var_, ctx_, custom_id_) \
    DCC_CTX_REQUIRED_FIRST_VALUE_FORM(var_, (ctx_), (custom_id_))

#endif /* DCC_SUGAR_CONTEXT_REQUIRE_H */
