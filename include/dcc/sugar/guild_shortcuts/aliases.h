#ifndef DCC_SUGAR_GUILD_SHORTCUTS_ALIASES_H
#define DCC_SUGAR_GUILD_SHORTCUTS_ALIASES_H

#include <dcc/sugar/guild_shortcuts/application.h>

#define DCC_APPLICATION_FETCH(app_) \
    DCC_APP_GET_CURRENT_APPLICATION((app_))
#define DCC_GATEWAY_BOT_FETCH(app_) \
    DCC_APP_GET_GATEWAY_BOT((app_))
#define DCC_SKUS_FETCH(app_, application_id_) \
    DCC_APP_GET_SKUS((app_), (application_id_))
#define DCC_ENTITLEMENTS_FETCH(app_, application_id_) \
    DCC_APP_GET_ENTITLEMENTS((app_), (application_id_))
#define DCC_ENTITLEMENTS_FETCH_QUERY(app_, application_id_, query_) \
    DCC_APP_GET_ENTITLEMENTS_QUERY((app_), (application_id_), (query_))
#define DCC_ENTITLEMENTS_FETCH_PAGE( \
    app_, \
    application_id_, \
    user_id_, \
    sku_ids_, \
    sku_id_count_, \
    before_id_, \
    after_id_, \
    limit_, \
    guild_id_, \
    exclude_ended_ \
) \
    DCC_APP_GET_ENTITLEMENTS_PAGE( \
        (app_), \
        (application_id_), \
        (user_id_), \
        (sku_ids_), \
        (sku_id_count_), \
        (before_id_), \
        (after_id_), \
        (limit_), \
        (guild_id_), \
        (exclude_ended_) \
    )
#define DCC_TEST_ENTITLEMENT_CREATE(app_, application_id_, json_body_) \
    DCC_APP_CREATE_TEST_ENTITLEMENT((app_), (application_id_), (json_body_))
#define DCC_TEST_ENTITLEMENT_CREATE_PARAMS(app_, params_) \
    DCC_APP_CREATE_TEST_ENTITLEMENT_PARAMS((app_), (params_))
#define DCC_TEST_ENTITLEMENT_DELETE(app_, application_id_, entitlement_id_) \
    DCC_APP_DELETE_TEST_ENTITLEMENT((app_), (application_id_), (entitlement_id_))
#define DCC_ENTITLEMENT_CONSUME(app_, application_id_, entitlement_id_) \
    DCC_APP_CONSUME_ENTITLEMENT((app_), (application_id_), (entitlement_id_))
#define DCC_ME_FETCH(app_) \
    DCC_APP_GET_CURRENT_USER((app_))
#define DCC_ME_EDIT_USER(app_, json_body_) \
    DCC_APP_EDIT_CURRENT_USER((app_), (json_body_))

#endif /* DCC_SUGAR_GUILD_SHORTCUTS_ALIASES_H */
