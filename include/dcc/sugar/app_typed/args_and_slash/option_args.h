#ifndef DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_OPTION_ARGS_H
#define DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_OPTION_ARGS_H

#include <dcc/sugar/app_typed/args_and_slash/binding_lists.h>

#define DCC_ARG_STRING(type_, field_, name_, fallback_) \
    DCC_CTX_OPTION_FIELD_STRING(type_, field_, name_, fallback_)
#define DCC_ARG_REQUIRED_STRING(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_STRING(type_, field_, name_)
#define DCC_ARG_INT(type_, field_, name_, fallback_) \
    DCC_CTX_OPTION_FIELD_INT(type_, field_, name_, fallback_)
#define DCC_ARG_REQUIRED_INT(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_INT(type_, field_, name_)
#define DCC_ARG_NUMBER(type_, field_, name_, fallback_) \
    DCC_CTX_OPTION_FIELD_NUMBER(type_, field_, name_, fallback_)
#define DCC_ARG_REQUIRED_NUMBER(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_NUMBER(type_, field_, name_)
#define DCC_ARG_BOOL(type_, field_, name_, fallback_) \
    DCC_CTX_OPTION_FIELD_BOOL(type_, field_, name_, fallback_)
#define DCC_ARG_REQUIRED_BOOL(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_BOOL(type_, field_, name_)
#define DCC_ARG_SNOWFLAKE(type_, field_, name_, fallback_) \
    DCC_CTX_OPTION_FIELD_SNOWFLAKE(type_, field_, name_, fallback_)
#define DCC_ARG_REQUIRED_SNOWFLAKE(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_SNOWFLAKE(type_, field_, name_)
#define DCC_ARG_USER(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_USER(type_, field_, name_)
#define DCC_ARG_REQUIRED_USER(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_USER(type_, field_, name_)
#define DCC_ARG_MEMBER(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_MEMBER(type_, field_, name_)
#define DCC_ARG_REQUIRED_MEMBER(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_MEMBER(type_, field_, name_)
#define DCC_ARG_ROLE(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_ROLE(type_, field_, name_)
#define DCC_ARG_REQUIRED_ROLE(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_ROLE(type_, field_, name_)
#define DCC_ARG_CHANNEL(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_CHANNEL(type_, field_, name_)
#define DCC_ARG_REQUIRED_CHANNEL(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_CHANNEL(type_, field_, name_)
#define DCC_ARG_MESSAGE(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_MESSAGE(type_, field_, name_)
#define DCC_ARG_REQUIRED_MESSAGE(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_MESSAGE(type_, field_, name_)
#define DCC_ARG_ATTACHMENT(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_ATTACHMENT(type_, field_, name_)
#define DCC_ARG_REQUIRED_ATTACHMENT(type_, field_, name_) \
    DCC_CTX_OPTION_FIELD_REQUIRED_ATTACHMENT(type_, field_, name_)

#endif /* DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_OPTION_ARGS_H */
