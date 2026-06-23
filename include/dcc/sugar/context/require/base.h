#ifndef DCC_SUGAR_CONTEXT_REQUIRE_BASE_H
#define DCC_SUGAR_CONTEXT_REQUIRE_BASE_H

#include <stddef.h>
#include <stdint.h>
#include <dcc/sugar/context/forms.h>

#ifndef DCC_CTX_REQUIRED_OPTION_MESSAGE
#define DCC_CTX_REQUIRED_OPTION_MESSAGE "Missing option."
#endif
#ifndef DCC_CTX_REQUIRED_GUILD_MESSAGE
#define DCC_CTX_REQUIRED_GUILD_MESSAGE "This command can only be used in a server."
#endif
#ifndef DCC_CTX_REQUIRED_DM_MESSAGE
#define DCC_CTX_REQUIRED_DM_MESSAGE "This command can only be used in DMs."
#endif
#ifndef DCC_CTX_REQUIRED_AUTHOR_MESSAGE
#define DCC_CTX_REQUIRED_AUTHOR_MESSAGE "Only the original user can use this."
#endif
#ifndef DCC_CTX_REQUIRED_PERMISSION_MESSAGE
#define DCC_CTX_REQUIRED_PERMISSION_MESSAGE "Missing required permissions."
#endif
#ifndef DCC_CTX_REQUIRED_ROLE_MESSAGE
#define DCC_CTX_REQUIRED_ROLE_MESSAGE "Missing required role."
#endif
#ifndef DCC_CTX_REQUIRED_TEXT_MESSAGE
#define DCC_CTX_REQUIRED_TEXT_MESSAGE "Missing text."
#endif
#ifndef DCC_CTX_REQUIRED_FIELD_MESSAGE
#define DCC_CTX_REQUIRED_FIELD_MESSAGE "Missing field."
#endif
#ifndef DCC_CTX_REQUIRED_SELECT_MESSAGE
#define DCC_CTX_REQUIRED_SELECT_MESSAGE "Choose a value."
#endif
#ifndef DCC_CTX_REQUIRED_COMPONENT_MESSAGE
#define DCC_CTX_REQUIRED_COMPONENT_MESSAGE "Unknown component."
#endif
#ifndef DCC_CTX_REQUIRED_ACTION_MESSAGE
#define DCC_CTX_REQUIRED_ACTION_MESSAGE "Unknown component action."
#endif

#endif /* DCC_SUGAR_CONTEXT_REQUIRE_BASE_H */
