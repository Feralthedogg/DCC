#ifndef DCC_SUGAR_MESSAGE_COMPONENT_UI_BASE_H
#define DCC_SUGAR_MESSAGE_COMPONENT_UI_BASE_H

#include <dcc/app.h>
#include <dcc/component.h>
#include <dcc/component_v2.h>
#include <dcc/message.h>
#include <dcc/modal.h>
#include <dcc/sugar/core_options.h>
#include <dcc/sugar/guild_shortcuts.h>
#include <dcc/sugar/modal.h>
#include <dcc/sugar/reply.h>
#include <stdint.h>

#ifndef DCC_COMPONENT_ID
#define DCC_COMPONENT_ID(namespace_, action_) namespace_ "." action_
#endif
#ifndef DCC_COMPONENT_PREFIX
#define DCC_COMPONENT_PREFIX(namespace_) namespace_ "."
#endif
#ifndef DCC_COMPONENT_ACTION_PREFIX
#define DCC_COMPONENT_ACTION_PREFIX(namespace_, action_prefix_) namespace_ "." action_prefix_
#endif

#endif /* DCC_SUGAR_MESSAGE_COMPONENT_UI_BASE_H */
