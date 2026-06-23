#ifndef DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_COMPONENT_ARGS_H
#define DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_COMPONENT_ARGS_H

#include <dcc/sugar/app_typed/args_and_slash/binding_lists.h>

#define DCC_COMPONENT_ARG_CUSTOM_ID_SUFFIX(type_, field_, prefix_, fallback_) \
    DCC_CTX_COMPONENT_FIELD_CUSTOM_ID_SUFFIX(type_, field_, prefix_, fallback_)
#define DCC_COMPONENT_ARG_REQUIRED_CUSTOM_ID_SUFFIX(type_, field_, prefix_) \
    DCC_CTX_COMPONENT_FIELD_REQUIRED_CUSTOM_ID_SUFFIX(type_, field_, prefix_)
#define DCC_COMPONENT_ARG_FIRST_VALUE(type_, field_, fallback_) \
    DCC_CTX_COMPONENT_FIELD_FIRST_VALUE(type_, field_, fallback_)
#define DCC_COMPONENT_ARG_REQUIRED_FIRST_VALUE(type_, field_) \
    DCC_CTX_COMPONENT_FIELD_REQUIRED_FIRST_VALUE(type_, field_)
#define DCC_COMPONENT_ARG_VALUES(type_, field_, count_field_, fallback_values_, fallback_count_) \
    DCC_CTX_COMPONENT_FIELD_VALUES(type_, field_, count_field_, fallback_values_, fallback_count_)
#define DCC_COMPONENT_ARG_REQUIRED_VALUES(type_, field_, count_field_) \
    DCC_CTX_COMPONENT_FIELD_REQUIRED_VALUES(type_, field_, count_field_)

#define DCC_COMPONENT_CUSTOM_ID_SUFFIX(type_, field_, prefix_, fallback_) \
    DCC_COMPONENT_ARG_CUSTOM_ID_SUFFIX(type_, field_, prefix_, fallback_)
#define DCC_COMPONENT_REQUIRED_CUSTOM_ID_SUFFIX(type_, field_, prefix_) \
    DCC_COMPONENT_ARG_REQUIRED_CUSTOM_ID_SUFFIX(type_, field_, prefix_)
#define DCC_COMPONENT_ACTION(type_, field_, prefix_, fallback_) \
    DCC_COMPONENT_ARG_CUSTOM_ID_SUFFIX(type_, field_, prefix_, fallback_)
#define DCC_COMPONENT_REQUIRED_ACTION(type_, field_, prefix_) \
    DCC_COMPONENT_ARG_REQUIRED_CUSTOM_ID_SUFFIX(type_, field_, prefix_)
#define DCC_COMPONENT_VALUE(type_, field_, fallback_) \
    DCC_COMPONENT_ARG_FIRST_VALUE(type_, field_, fallback_)
#define DCC_COMPONENT_REQUIRED_VALUE(type_, field_) \
    DCC_COMPONENT_ARG_REQUIRED_FIRST_VALUE(type_, field_)
#define DCC_COMPONENT_VALUES(type_, field_, count_field_, fallback_values_, fallback_count_) \
    DCC_COMPONENT_ARG_VALUES(type_, field_, count_field_, fallback_values_, fallback_count_)
#define DCC_COMPONENT_REQUIRED_VALUES(type_, field_, count_field_) \
    DCC_COMPONENT_ARG_REQUIRED_VALUES(type_, field_, count_field_)
#define DCC_SELECT_VALUE(type_, field_, fallback_) \
    DCC_COMPONENT_VALUE(type_, field_, fallback_)
#define DCC_SELECT_REQUIRED_VALUE(type_, field_) \
    DCC_COMPONENT_REQUIRED_VALUE(type_, field_)
#define DCC_SELECT_VALUES(type_, field_, count_field_, fallback_values_, fallback_count_) \
    DCC_COMPONENT_VALUES(type_, field_, count_field_, fallback_values_, fallback_count_)
#define DCC_SELECT_REQUIRED_VALUES(type_, field_, count_field_) \
    DCC_COMPONENT_REQUIRED_VALUES(type_, field_, count_field_)

#endif /* DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_COMPONENT_ARGS_H */
