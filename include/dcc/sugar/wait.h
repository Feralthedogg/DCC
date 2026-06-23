#ifndef DCC_SUGAR_WAIT_H
#define DCC_SUGAR_WAIT_H

#include <dcc/app.h>

#define DCC_WAIT_FILTER() \
    ((dcc_app_wait_filter_t){ \
        .size = sizeof(dcc_app_wait_filter_t) \
    })

#define DCC_WAIT_FILTER_USER(user_id_) \
    ((dcc_app_wait_filter_t){ \
        .size = sizeof(dcc_app_wait_filter_t), \
        .user_id = (user_id_) \
    })

#define DCC_WAIT_FILTER_CHANNEL(channel_id_) \
    ((dcc_app_wait_filter_t){ \
        .size = sizeof(dcc_app_wait_filter_t), \
        .channel_id = (channel_id_) \
    })

#define DCC_WAIT_FILTER_GUILD(guild_id_) \
    ((dcc_app_wait_filter_t){ \
        .size = sizeof(dcc_app_wait_filter_t), \
        .guild_id = (guild_id_) \
    })

#define DCC_WAIT_FILTER_COMPONENT(custom_id_) \
    ((dcc_app_wait_filter_t){ \
        .size = sizeof(dcc_app_wait_filter_t), \
        .custom_id = (custom_id_) \
    })

#define DCC_WAIT_FILTER_COMPONENT_PREFIX(custom_id_prefix_) \
    ((dcc_app_wait_filter_t){ \
        .size = sizeof(dcc_app_wait_filter_t), \
        .custom_id_prefix = (custom_id_prefix_) \
    })

#define DCC_WAIT_FILTER_CTX(ctx_) \
    ((dcc_app_wait_filter_t){ \
        .size = sizeof(dcc_app_wait_filter_t), \
        .user_id = dcc_ctx_user_id((ctx_)), \
        .channel_id = dcc_ctx_channel_id((ctx_)), \
        .guild_id = dcc_ctx_guild_id((ctx_)) \
    })

#define DCC_WAIT_RESULT_DEINIT(result_) dcc_app_wait_result_deinit((result_))
#define DCC_WAIT_RESULT_FREE(result_) DCC_WAIT_RESULT_DEINIT((result_))

#define DCC_WAIT_TYPE(result_) dcc_app_wait_result_type((result_))
#define DCC_WAIT_INTERACTION(result_) dcc_app_wait_result_interaction((result_))
#define DCC_WAIT_MESSAGE_OBJECT(result_) dcc_app_wait_result_message((result_))
#define DCC_WAIT_ID(result_) dcc_app_wait_result_id((result_))
#define DCC_WAIT_USER_ID(result_) dcc_app_wait_result_user_id((result_))
#define DCC_WAIT_CHANNEL_ID(result_) dcc_app_wait_result_channel_id((result_))
#define DCC_WAIT_GUILD_ID(result_) dcc_app_wait_result_guild_id((result_))
#define DCC_WAIT_CUSTOM_ID(result_) dcc_app_wait_result_custom_id((result_))
#define DCC_WAIT_MESSAGE_CONTENT(result_, fallback_) \
    dcc_app_wait_result_message_content((result_), (fallback_))
#define DCC_WAIT_VALUE_COUNT(result_) dcc_app_wait_result_value_count((result_))
#define DCC_WAIT_VALUE(result_, index_) dcc_app_wait_result_value((result_), (index_))
#define DCC_WAIT_FORM_FIELD(result_, custom_id_) \
    dcc_app_wait_result_form_field((result_), (custom_id_))
#define DCC_WAIT_FORM_TEXT(result_, custom_id_, fallback_) \
    dcc_app_wait_result_form_string((result_), (custom_id_), (fallback_))
#define DCC_WAIT_FORM_CHECKED(result_, custom_id_, fallback_) \
    dcc_app_wait_result_form_bool((result_), (custom_id_), (fallback_))
#define DCC_WAIT_FORM_VALUE_COUNT(result_, custom_id_) \
    dcc_app_wait_result_form_value_count((result_), (custom_id_))
#define DCC_WAIT_FORM_VALUE(result_, custom_id_, index_) \
    dcc_app_wait_result_form_value((result_), (custom_id_), (index_))

#define DCC_WAIT_COMPONENT(ctx_, timeout_ms_, out_) \
    dcc_ctx_wait_for_component((ctx_), (timeout_ms_), (out_))

#define DCC_WAIT_BUTTON(ctx_, custom_id_, timeout_ms_, out_) \
    dcc_ctx_wait_for_button((ctx_), (custom_id_), (timeout_ms_), (out_))

#define DCC_WAIT_BUTTON_PREFIX(ctx_, custom_id_prefix_, timeout_ms_, out_) \
    dcc_ctx_wait_for_button_prefix((ctx_), (custom_id_prefix_), (timeout_ms_), (out_))

#define DCC_WAIT_SELECT(ctx_, custom_id_, timeout_ms_, out_) \
    dcc_ctx_wait_for_select((ctx_), (custom_id_), (timeout_ms_), (out_))

#define DCC_WAIT_SELECT_PREFIX(ctx_, custom_id_prefix_, timeout_ms_, out_) \
    dcc_ctx_wait_for_select_prefix((ctx_), (custom_id_prefix_), (timeout_ms_), (out_))

#define DCC_WAIT_MODAL(ctx_, custom_id_, timeout_ms_, out_) \
    dcc_ctx_wait_for_modal((ctx_), (custom_id_), (timeout_ms_), (out_))

#define DCC_WAIT_MODAL_PREFIX(ctx_, custom_id_prefix_, timeout_ms_, out_) \
    dcc_ctx_wait_for_modal_prefix((ctx_), (custom_id_prefix_), (timeout_ms_), (out_))

#define DCC_WAIT_MESSAGE(ctx_, timeout_ms_, out_) \
    dcc_ctx_wait_for_message((ctx_), (timeout_ms_), (out_))

#define DCC_WAIT_FOR_COMPONENT(ctx_, timeout_ms_, out_) \
    DCC_WAIT_COMPONENT((ctx_), (timeout_ms_), (out_))

#define DCC_WAIT_FOR_BUTTON(ctx_, custom_id_, timeout_ms_, out_) \
    DCC_WAIT_BUTTON((ctx_), (custom_id_), (timeout_ms_), (out_))

#define DCC_WAIT_FOR_BUTTON_PREFIX(ctx_, custom_id_prefix_, timeout_ms_, out_) \
    DCC_WAIT_BUTTON_PREFIX((ctx_), (custom_id_prefix_), (timeout_ms_), (out_))

#define DCC_WAIT_FOR_SELECT(ctx_, custom_id_, timeout_ms_, out_) \
    DCC_WAIT_SELECT((ctx_), (custom_id_), (timeout_ms_), (out_))

#define DCC_WAIT_FOR_SELECT_PREFIX(ctx_, custom_id_prefix_, timeout_ms_, out_) \
    DCC_WAIT_SELECT_PREFIX((ctx_), (custom_id_prefix_), (timeout_ms_), (out_))

#define DCC_WAIT_FOR_MODAL(ctx_, custom_id_, timeout_ms_, out_) \
    DCC_WAIT_MODAL((ctx_), (custom_id_), (timeout_ms_), (out_))

#define DCC_WAIT_FOR_MODAL_PREFIX(ctx_, custom_id_prefix_, timeout_ms_, out_) \
    DCC_WAIT_MODAL_PREFIX((ctx_), (custom_id_prefix_), (timeout_ms_), (out_))

#define DCC_WAIT_FOR_MESSAGE(ctx_, timeout_ms_, out_) \
    DCC_WAIT_MESSAGE((ctx_), (timeout_ms_), (out_))

#define DCC_COLLECT_COMPONENT(ctx_, timeout_ms_, out_) \
    DCC_WAIT_FOR_COMPONENT((ctx_), (timeout_ms_), (out_))

#define DCC_COLLECT_BUTTON(ctx_, custom_id_, timeout_ms_, out_) \
    DCC_WAIT_FOR_BUTTON((ctx_), (custom_id_), (timeout_ms_), (out_))

#define DCC_COLLECT_BUTTON_PREFIX(ctx_, custom_id_prefix_, timeout_ms_, out_) \
    DCC_WAIT_FOR_BUTTON_PREFIX((ctx_), (custom_id_prefix_), (timeout_ms_), (out_))

#define DCC_COLLECT_SELECT(ctx_, custom_id_, timeout_ms_, out_) \
    DCC_WAIT_FOR_SELECT((ctx_), (custom_id_), (timeout_ms_), (out_))

#define DCC_COLLECT_SELECT_PREFIX(ctx_, custom_id_prefix_, timeout_ms_, out_) \
    DCC_WAIT_FOR_SELECT_PREFIX((ctx_), (custom_id_prefix_), (timeout_ms_), (out_))

#define DCC_COLLECT_MODAL(ctx_, custom_id_, timeout_ms_, out_) \
    DCC_WAIT_FOR_MODAL((ctx_), (custom_id_), (timeout_ms_), (out_))

#define DCC_COLLECT_MODAL_PREFIX(ctx_, custom_id_prefix_, timeout_ms_, out_) \
    DCC_WAIT_FOR_MODAL_PREFIX((ctx_), (custom_id_prefix_), (timeout_ms_), (out_))

#define DCC_COLLECT_MESSAGE(ctx_, timeout_ms_, out_) \
    DCC_WAIT_FOR_MESSAGE((ctx_), (timeout_ms_), (out_))

#define DCC_APP_WAIT_FOR_INTERACTION(app_, type_, timeout_ms_, out_) \
    dcc_app_wait_for_interaction((app_), (type_), (timeout_ms_), (out_))

#define DCC_APP_WAIT_FOR_INTERACTION_FILTERED(app_, type_, filter_, timeout_ms_, out_) \
    dcc_app_wait_for_interaction_filtered((app_), (type_), (filter_), (timeout_ms_), (out_))

#define DCC_APP_WAIT_FOR_COMPONENT(app_, timeout_ms_, out_) \
    dcc_app_wait_for_component((app_), (timeout_ms_), (out_))

#define DCC_APP_WAIT_FOR_COMPONENT_FILTERED(app_, filter_, timeout_ms_, out_) \
    dcc_app_wait_for_component_filtered((app_), (filter_), (timeout_ms_), (out_))

#define DCC_APP_WAIT_FOR_BUTTON(app_, timeout_ms_, out_) \
    dcc_app_wait_for_button((app_), (timeout_ms_), (out_))

#define DCC_APP_WAIT_FOR_SELECT(app_, timeout_ms_, out_) \
    dcc_app_wait_for_select((app_), (timeout_ms_), (out_))

#define DCC_APP_WAIT_FOR_MODAL(app_, timeout_ms_, out_) \
    dcc_app_wait_for_modal((app_), (timeout_ms_), (out_))

#define DCC_APP_WAIT_FOR_MESSAGE_TYPE(app_, type_, timeout_ms_, out_) \
    dcc_app_wait_for_message((app_), (type_), (timeout_ms_), (out_))

#define DCC_APP_WAIT_FOR_MESSAGE(app_, timeout_ms_, out_) \
    DCC_APP_WAIT_FOR_MESSAGE_TYPE((app_), DCC_EVENT_MESSAGE_CREATE, (timeout_ms_), (out_))

#define DCC_APP_WAIT_FOR_MESSAGE_CREATE(app_, timeout_ms_, out_) \
    DCC_APP_WAIT_FOR_MESSAGE_TYPE((app_), DCC_EVENT_MESSAGE_CREATE, (timeout_ms_), (out_))

#define DCC_APP_WAIT_FOR_MESSAGE_UPDATE(app_, timeout_ms_, out_) \
    DCC_APP_WAIT_FOR_MESSAGE_TYPE((app_), DCC_EVENT_MESSAGE_UPDATE, (timeout_ms_), (out_))

#define DCC_APP_WAIT_FOR_MESSAGE_DELETE(app_, timeout_ms_, out_) \
    DCC_APP_WAIT_FOR_MESSAGE_TYPE((app_), DCC_EVENT_MESSAGE_DELETE, (timeout_ms_), (out_))

#define DCC_APP_WAIT_FOR_MESSAGE_FILTERED(app_, type_, filter_, timeout_ms_, out_) \
    dcc_app_wait_for_message_filtered((app_), (type_), (filter_), (timeout_ms_), (out_))

#endif
