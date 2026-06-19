#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_author(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
) {
    dcc_status_t status = dcc_json_gateway_parse_user_object(
        parser,
        &out->message.author,
        out->author_username,
        sizeof(out->author_username),
        out->author_global_name,
        sizeof(out->author_global_name),
        out->author_avatar,
        sizeof(out->author_avatar)
    );
    if (status != DCC_OK) {
        return status;
    }

    out->gateway.user = out->message.author;
    out->gateway.user_id = out->message.author.id;
    out->has_message = 1;
    return DCC_OK;
}

dcc_status_t dcc_json_gateway_parse_user(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
) {
    dcc_status_t status = dcc_json_gateway_parse_user_object(
        parser,
        &out->gateway.user,
        out->gateway_user_username,
        sizeof(out->gateway_user_username),
        out->gateway_user_global_name,
        sizeof(out->gateway_user_global_name),
        out->gateway_user_avatar,
        sizeof(out->gateway_user_avatar)
    );
    if (status != DCC_OK) {
        return status;
    }

    out->gateway.user_id = out->gateway.user.id;
    out->user = out->gateway.user;
    out->member.user = out->gateway.user;
    out->invite.inviter = out->gateway.user;
    out->soundboard_sound.user = out->gateway.user;
    out->soundboard_sound.user_id = out->gateway.user.id;
    out->guild_ban.user = out->gateway.user;
    out->presence.user = out->gateway.user;
    out->has_user = 1;
    out->has_member = 1;
    out->has_invite = 1;
    out->has_soundboard_sound = 1;
    out->has_guild_ban = 1;
    out->has_presence = 1;
    return DCC_OK;
}
