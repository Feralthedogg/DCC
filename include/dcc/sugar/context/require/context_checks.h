#ifndef DCC_SUGAR_CONTEXT_REQUIRE_CONTEXT_CHECKS_H
#define DCC_SUGAR_CONTEXT_REQUIRE_CONTEXT_CHECKS_H

#include <dcc/sugar/context/require/core.h>

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

#endif /* DCC_SUGAR_CONTEXT_REQUIRE_CONTEXT_CHECKS_H */
