#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_finish_sticker_array(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated
) {
    out->gateway.ids = out->gateway_ids;
    out->gateway.ids_count = stored;
    out->gateway.ids_truncated = truncated;
    out->guild_id_list_update.ids = out->gateway.ids;
    out->guild_id_list_update.ids_count = out->gateway.ids_count;
    out->guild_id_list_update.ids_truncated = out->gateway.ids_truncated;
    out->guild_id_list_update.stickers = out->gateway_stickers;
    out->guild_id_list_update.stickers_count = stored;
    out->guild_id_list_update.stickers_truncated = truncated;
    out->guild.stickers = out->gateway_stickers;
    out->guild.stickers_count = stored;
    out->guild.stickers_truncated = truncated;
    out->has_guild = 1;
    out->has_guild_id_list_update = 1;
}
