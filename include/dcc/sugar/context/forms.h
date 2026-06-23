#ifndef DCC_SUGAR_CONTEXT_FORMS_H
#define DCC_SUGAR_CONTEXT_FORMS_H

#include <dcc/sugar/context/options.h>

#define DCC_CTX_FORM_PRESENT(ctx_, custom_id_) \
    (dcc_ctx_form_field((ctx_), (custom_id_)) != NULL ? 1U : 0U)
#define DCC_CTX_FORM_STRING_OR(ctx_, custom_id_, fallback_) \
    dcc_ctx_form_string((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FORM_TEXT(ctx_, custom_id_, fallback_) \
    dcc_ctx_form_string((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FORM_TEXT_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_TEXT((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FORM_TEXT_VALUE(ctx_, custom_id_) \
    DCC_CTX_FORM_TEXT((ctx_), (custom_id_), NULL)
#define DCC_CTX_FORM_BOOL_OR(ctx_, custom_id_, fallback_) \
    dcc_ctx_form_bool((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FORM_CHECKED(ctx_, custom_id_, fallback_) \
    dcc_ctx_form_bool((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FORM_CHECKED_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_CHECKED((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FORM_IS_CHECKED(ctx_, custom_id_) \
    DCC_CTX_FORM_CHECKED((ctx_), (custom_id_), 0U)
#define DCC_CTX_FORM_VALUE_COUNT(ctx_, custom_id_) \
    dcc_ctx_form_values_count((ctx_), (custom_id_))
#define DCC_CTX_FORM_VALUE(ctx_, custom_id_, index_) \
    dcc_ctx_form_value((ctx_), (custom_id_), (index_))
#define DCC_CTX_FORM_FIRST_VALUE(ctx_, custom_id_, fallback_) \
    dcc_ctx_form_first_value((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FORM_FIRST_VALUE_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_FIRST_VALUE((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FIELD_PRESENT(ctx_, custom_id_) DCC_CTX_FORM_PRESENT((ctx_), (custom_id_))
#define DCC_CTX_FIELD_TEXT_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_TEXT((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FIELD_TEXT(ctx_, custom_id_) DCC_CTX_FIELD_TEXT_OR((ctx_), (custom_id_), NULL)
#define DCC_CTX_FIELD_STRING_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_STRING_OR((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FIELD_STRING(ctx_, custom_id_) \
    DCC_CTX_FIELD_STRING_OR((ctx_), (custom_id_), NULL)
#define DCC_CTX_FIELD_CHECKED_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_CHECKED((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FIELD_CHECKED(ctx_, custom_id_) \
    DCC_CTX_FIELD_CHECKED_OR((ctx_), (custom_id_), 0U)
#define DCC_CTX_FIELD_BOOL_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_BOOL_OR((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FIELD_BOOL(ctx_, custom_id_) DCC_CTX_FIELD_BOOL_OR((ctx_), (custom_id_), 0U)
#define DCC_CTX_FIELD_VALUE_COUNT(ctx_, custom_id_) \
    DCC_CTX_FORM_VALUE_COUNT((ctx_), (custom_id_))
#define DCC_CTX_FIELD_VALUE_AT(ctx_, custom_id_, index_) \
    DCC_CTX_FORM_VALUE((ctx_), (custom_id_), (index_))
#define DCC_CTX_FIELD_FIRST_VALUE_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_FIRST_VALUE((ctx_), (custom_id_), (fallback_))
#define DCC_CTX_FIELD_FIRST_VALUE(ctx_, custom_id_) \
    DCC_CTX_FIELD_FIRST_VALUE_OR((ctx_), (custom_id_), NULL)
#define DCC_CTX_FIELD_VALUES(ctx_, custom_id_, out_count_) \
    dcc_ctx_form_values((ctx_), (custom_id_), (out_count_))
#define DCC_FIELD_TEXT_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_TEXT((ctx_), (custom_id_), (fallback_))
#define DCC_FIELD_TEXT(ctx_, custom_id_) DCC_FIELD_TEXT_OR((ctx_), (custom_id_), NULL)
#define DCC_FIELD_CHECKED_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_CHECKED((ctx_), (custom_id_), (fallback_))
#define DCC_FIELD_CHECKED(ctx_, custom_id_) DCC_FIELD_CHECKED_OR((ctx_), (custom_id_), 0U)
#define DCC_FIELD_VALUE_COUNT(ctx_, custom_id_) DCC_CTX_FORM_VALUE_COUNT((ctx_), (custom_id_))
#define DCC_FIELD_VALUE_AT(ctx_, custom_id_, index_) \
    DCC_CTX_FORM_VALUE((ctx_), (custom_id_), (index_))
#define DCC_FIELD_FIRST_VALUE_OR(ctx_, custom_id_, fallback_) \
    DCC_CTX_FORM_FIRST_VALUE((ctx_), (custom_id_), (fallback_))
#define DCC_FIELD_FIRST_VALUE(ctx_, custom_id_) \
    DCC_FIELD_FIRST_VALUE_OR((ctx_), (custom_id_), NULL)
#define DCC_FIELD_VALUES_RAW(ctx_, custom_id_, out_count_) \
    dcc_ctx_form_values((ctx_), (custom_id_), (out_count_))

#define DCC_CTX_SELECTED_COUNT(ctx_) dcc_ctx_component_value_count((ctx_))
#define DCC_CTX_SELECTED_VALUE(ctx_, index_) dcc_ctx_component_value((ctx_), (index_))
#define DCC_CTX_SELECTED_VALUE_AT(ctx_, index_) DCC_CTX_SELECTED_VALUE((ctx_), (index_))
#define DCC_CTX_SELECTED_FIRST_OR(ctx_, fallback_) \
    dcc_ctx_component_first_value((ctx_), (fallback_))
#define DCC_CTX_SELECTED_FIRST(ctx_) DCC_CTX_SELECTED_FIRST_OR((ctx_), NULL)
#define DCC_CTX_SELECTED_VALUES(ctx_, out_count_) dcc_ctx_component_values((ctx_), (out_count_))
#define DCC_CTX_SELECT_COUNT(ctx_) DCC_CTX_SELECTED_COUNT((ctx_))
#define DCC_CTX_SELECT_VALUE(ctx_, index_) DCC_CTX_SELECTED_VALUE((ctx_), (index_))
#define DCC_CTX_SELECT_VALUE_AT(ctx_, index_) DCC_CTX_SELECTED_VALUE((ctx_), (index_))
#define DCC_CTX_SELECT_FIRST_OR(ctx_, fallback_) \
    DCC_CTX_SELECTED_FIRST_OR((ctx_), (fallback_))
#define DCC_CTX_SELECT_FIRST(ctx_) DCC_CTX_SELECTED_FIRST((ctx_))
#define DCC_CTX_SELECT_VALUES(ctx_, out_count_) DCC_CTX_SELECTED_VALUES((ctx_), (out_count_))
#define DCC_SELECTED_COUNT(ctx_) DCC_CTX_SELECTED_COUNT((ctx_))
#define DCC_SELECTED_VALUE(ctx_, index_) DCC_CTX_SELECTED_VALUE((ctx_), (index_))
#define DCC_SELECTED_FIRST_OR(ctx_, fallback_) DCC_CTX_SELECTED_FIRST_OR((ctx_), (fallback_))
#define DCC_SELECTED_FIRST(ctx_) DCC_SELECTED_FIRST_OR((ctx_), NULL)
#define DCC_SELECTED_VALUES(ctx_, out_count_) DCC_CTX_SELECTED_VALUES((ctx_), (out_count_))
#define DCC_SELECT_COUNT(ctx_) DCC_CTX_SELECTED_COUNT((ctx_))
#define DCC_SELECT_VALUE_AT(ctx_, index_) DCC_CTX_SELECTED_VALUE((ctx_), (index_))
#define DCC_SELECT_FIRST_OR(ctx_, fallback_) DCC_CTX_SELECTED_FIRST_OR((ctx_), (fallback_))
#define DCC_SELECT_FIRST(ctx_) DCC_SELECT_FIRST_OR((ctx_), NULL)
#define DCC_SELECT_VALUES_RAW(ctx_, out_count_) DCC_CTX_SELECTED_VALUES((ctx_), (out_count_))


#endif /* DCC_SUGAR_CONTEXT_FORMS_H */
