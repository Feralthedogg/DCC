#ifndef DCC_SUGAR_CONTEXT_REQUIRE_ALIASES_CUSTOM_ID_H
#define DCC_SUGAR_CONTEXT_REQUIRE_ALIASES_CUSTOM_ID_H

#include <dcc/sugar/context/require/custom_id.h>

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

#endif /* DCC_SUGAR_CONTEXT_REQUIRE_ALIASES_CUSTOM_ID_H */
