#ifndef DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_FORM_ARGS_H
#define DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_FORM_ARGS_H

#include <dcc/sugar/app_typed/args_and_slash/binding_lists.h>

#define DCC_FORM_ARG_STRING(type_, field_, custom_id_, fallback_) \
    DCC_CTX_FORM_FIELD_STRING(type_, field_, custom_id_, fallback_)
#define DCC_FORM_ARG_REQUIRED_STRING(type_, field_, custom_id_) \
    DCC_CTX_FORM_FIELD_REQUIRED_STRING(type_, field_, custom_id_)
#define DCC_FORM_ARG_BOOL(type_, field_, custom_id_, fallback_) \
    DCC_CTX_FORM_FIELD_BOOL(type_, field_, custom_id_, fallback_)
#define DCC_FORM_ARG_REQUIRED_BOOL(type_, field_, custom_id_) \
    DCC_CTX_FORM_FIELD_REQUIRED_BOOL(type_, field_, custom_id_)
#define DCC_FORM_ARG_VALUES(type_, field_, count_field_, custom_id_, fallback_values_, fallback_count_) \
    DCC_CTX_FORM_FIELD_VALUES(type_, field_, count_field_, custom_id_, fallback_values_, fallback_count_)
#define DCC_FORM_ARG_REQUIRED_VALUES(type_, field_, count_field_, custom_id_) \
    DCC_CTX_FORM_FIELD_REQUIRED_VALUES(type_, field_, count_field_, custom_id_)

#define DCC_FORM_TEXT(type_, field_, custom_id_, fallback_) \
    DCC_FORM_ARG_STRING(type_, field_, custom_id_, fallback_)
#define DCC_FORM_REQUIRED_TEXT(type_, field_, custom_id_) \
    DCC_FORM_ARG_REQUIRED_STRING(type_, field_, custom_id_)
#define DCC_FORM_CHECKBOX(type_, field_, custom_id_, fallback_) \
    DCC_FORM_ARG_BOOL(type_, field_, custom_id_, fallback_)
#define DCC_FORM_REQUIRED_CHECKBOX(type_, field_, custom_id_) \
    DCC_FORM_ARG_REQUIRED_BOOL(type_, field_, custom_id_)
#define DCC_FORM_VALUES(type_, field_, count_field_, custom_id_, fallback_values_, fallback_count_) \
    DCC_FORM_ARG_VALUES(type_, field_, count_field_, custom_id_, fallback_values_, fallback_count_)
#define DCC_FORM_REQUIRED_VALUES(type_, field_, count_field_, custom_id_) \
    DCC_FORM_ARG_REQUIRED_VALUES(type_, field_, count_field_, custom_id_)
#define DCC_MODAL_TEXT(type_, field_, custom_id_, fallback_) \
    DCC_FORM_TEXT(type_, field_, custom_id_, fallback_)
#define DCC_MODAL_REQUIRED_TEXT(type_, field_, custom_id_) \
    DCC_FORM_REQUIRED_TEXT(type_, field_, custom_id_)
#define DCC_MODAL_CHECKBOX(type_, field_, custom_id_, fallback_) \
    DCC_FORM_CHECKBOX(type_, field_, custom_id_, fallback_)
#define DCC_MODAL_REQUIRED_CHECKBOX(type_, field_, custom_id_) \
    DCC_FORM_REQUIRED_CHECKBOX(type_, field_, custom_id_)

#endif /* DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_FORM_ARGS_H */
