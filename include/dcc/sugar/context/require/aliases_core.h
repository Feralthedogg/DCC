#ifndef DCC_SUGAR_CONTEXT_REQUIRE_ALIASES_CORE_H
#define DCC_SUGAR_CONTEXT_REQUIRE_ALIASES_CORE_H

#include <dcc/sugar/context/require/context_checks.h>

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

#endif /* DCC_SUGAR_CONTEXT_REQUIRE_ALIASES_CORE_H */
