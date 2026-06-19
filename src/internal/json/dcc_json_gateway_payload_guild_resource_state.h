#ifndef DCC_JSON_GATEWAY_PAYLOAD_GUILD_RESOURCE_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_GUILD_RESOURCE_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_GUILD_RESOURCE_FIELDS \
    int has_guild; \
    dcc_guild_t guild; \
    char guild_name[128]; \
    char guild_description[1024]; \
    char guild_icon[128]; \
    char guild_banner[128]; \
    char guild_vanity_url_code[128]; \
    char guild_preferred_locale[32]; \
    char guild_splash[128]; \
    char guild_discovery_splash[128]

#endif
