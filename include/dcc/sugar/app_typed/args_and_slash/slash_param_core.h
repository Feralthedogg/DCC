#ifndef DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_SLASH_PARAM_CORE_H
#define DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_SLASH_PARAM_CORE_H

#include <dcc/sugar/app_typed/args_and_slash/base.h>

#define DCC_SLASH_PARAM_BIND(type_, kind_, ...) DCC_SLASH_PARAM_BIND_##kind_(type_, __VA_ARGS__),
#define DCC_SLASH_PARAM_OPTION(type_, kind_, ...) DCC_SLASH_PARAM_OPTION_##kind_(type_, __VA_ARGS__),

#endif /* DCC_SUGAR_APP_TYPED_ARGS_AND_SLASH_SLASH_PARAM_CORE_H */
