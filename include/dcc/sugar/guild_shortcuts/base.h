#ifndef DCC_SUGAR_GUILD_SHORTCUTS_BASE_H
#define DCC_SUGAR_GUILD_SHORTCUTS_BASE_H

#include <dcc/app.h>
#include <dcc/sugar/core_options.h>

/* Included by <dcc/sugar.h>. */
#define DCC_APP_INFER_GUILD_FROM_CHANNEL(app_, channel_id_, cb_, user_data_) \
    dcc_app_infer_guild_id_from_channel((app_), (channel_id_), (cb_), (user_data_))
#define DCC_INFER_GUILD_FROM_CHANNEL(app_, channel_id_, cb_, user_data_) \
    DCC_APP_INFER_GUILD_FROM_CHANNEL((app_), (channel_id_), (cb_), (user_data_))

#endif /* DCC_SUGAR_GUILD_SHORTCUTS_BASE_H */
