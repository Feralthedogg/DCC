#ifndef DCC_APP_H
#define DCC_APP_H

#include <dcc/app/env.h>
#include <dcc/application_command.h>
#include <dcc/autocomplete.h>
#include <dcc/client.h>
#include <dcc/component_session.h>
#include <dcc/command_registry.h>
#include <dcc/embed.h>
#include <dcc/events.h>
#include <dcc/interaction_flow.h>
#include <dcc/interaction_helpers.h>
#include <dcc/managed_message.h>
#include <dcc/message.h>
#include <dcc/modal.h>
#include <dcc/permissions.h>
#include <dcc/rest/base.h>
#include <dcc/rest/types/auto_moderation.h>
#include <dcc/rest/types/channels.h>
#include <dcc/rest/types/emojis_stickers.h>
#include <dcc/rest/types/entitlements.h>
#include <dcc/rest/types/guilds.h>
#include <dcc/rest/types/guild_members.h>
#include <dcc/rest/types/invites.h>
#include <dcc/rest/types/messages.h>
#include <dcc/rest/types/onboarding.h>
#include <dcc/rest/types/roles.h>
#include <dcc/rest/types/scheduled_events.h>
#include <dcc/rest/types/stage.h>
#include <dcc/rest/types/templates.h>
#include <dcc/rest/types/threads.h>
#include <dcc/rest/types/users.h>
#include <dcc/rest/types/voice_states.h>
#include <dcc/rest/types/webhooks.h>
#include <dcc/store.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <dcc/app/core_decls.h>
#include <dcc/app/rest_decls_a.h>
#include <dcc/app/rest_decls_b.h>
#include <dcc/app/context_decls.h>
#ifdef __cplusplus
}
#endif

#endif
