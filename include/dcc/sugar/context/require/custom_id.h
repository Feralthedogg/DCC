#ifndef DCC_SUGAR_CONTEXT_REQUIRE_CUSTOM_ID_H
#define DCC_SUGAR_CONTEXT_REQUIRE_CUSTOM_ID_H

#include <dcc/sugar/context/require/core.h>

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

#endif /* DCC_SUGAR_CONTEXT_REQUIRE_CUSTOM_ID_H */
