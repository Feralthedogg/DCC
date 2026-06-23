#ifndef DCC_SUGAR_CONTEXT_REQUIRE_SELECTED_H
#define DCC_SUGAR_CONTEXT_REQUIRE_SELECTED_H

#include <dcc/sugar/context/require/core.h>

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

#endif /* DCC_SUGAR_CONTEXT_REQUIRE_SELECTED_H */
