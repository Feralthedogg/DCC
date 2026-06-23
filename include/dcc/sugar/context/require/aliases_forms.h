#ifndef DCC_SUGAR_CONTEXT_REQUIRE_ALIASES_FORMS_H
#define DCC_SUGAR_CONTEXT_REQUIRE_ALIASES_FORMS_H

#include <dcc/sugar/context/require/forms.h>

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

#endif /* DCC_SUGAR_CONTEXT_REQUIRE_ALIASES_FORMS_H */
