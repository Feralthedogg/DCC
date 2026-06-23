#ifndef DCC_SUGAR_CONTEXT_REQUIRE_FORMS_H
#define DCC_SUGAR_CONTEXT_REQUIRE_FORMS_H

#include <dcc/sugar/context/require/core.h>

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

#endif /* DCC_SUGAR_CONTEXT_REQUIRE_FORMS_H */
