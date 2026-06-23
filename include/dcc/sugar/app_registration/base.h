#ifndef DCC_SUGAR_APP_REGISTRATION_BASE_H
#define DCC_SUGAR_APP_REGISTRATION_BASE_H

#include <dcc/app.h>
#include <dcc/sugar/route_policy.h>

#ifndef DCC_COMPONENT_ID
#define DCC_COMPONENT_ID(namespace_, action_) namespace_ "." action_
#endif
#ifndef DCC_COMPONENT_PREFIX
#define DCC_COMPONENT_PREFIX(namespace_) namespace_ "."
#endif
#ifndef DCC_COMPONENT_ACTION_PREFIX
#define DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_) namespace_ "." action_prefix_
#endif

#endif /* DCC_SUGAR_APP_REGISTRATION_BASE_H */
