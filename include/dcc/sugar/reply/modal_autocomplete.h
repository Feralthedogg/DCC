#ifndef DCC_SUGAR_REPLY_MODAL_AUTOCOMPLETE_H
#define DCC_SUGAR_REPLY_MODAL_AUTOCOMPLETE_H

#include <dcc/sugar/reply/base.h>

#define DCC_SHOW_MODAL(ctx_, modal_) \
    dcc_ctx_show_modal((ctx_), &((dcc_modal_builder_t[]){ (modal_) })[0], NULL, NULL)

#define DCC_SHOW_MODAL_CB(ctx_, modal_, cb_, user_data_) \
    dcc_ctx_show_modal((ctx_), &((dcc_modal_builder_t[]){ (modal_) })[0], (cb_), (user_data_))

#define DCC_SHOW_MODAL_FLOW(ctx_, flow_) \
    dcc_ctx_show_modal_flow((ctx_), &((dcc_app_modal_flow_t[]){ (flow_) })[0], NULL, NULL)

#define DCC_SHOW_MODAL_FLOW_CB(ctx_, flow_, cb_, user_data_) \
    dcc_ctx_show_modal_flow((ctx_), &((dcc_app_modal_flow_t[]){ (flow_) })[0], (cb_), (user_data_))

#define DCC_SHOW_MODAL_BUILDER(ctx_, custom_id_, title_, ...) \
    DCC_SHOW_MODAL((ctx_), DCC_MODAL((custom_id_), (title_), __VA_ARGS__))
#define DCC_SHOW_MODAL_BUILDER_CB(ctx_, custom_id_, title_, cb_, user_data_, ...) \
    DCC_SHOW_MODAL_CB((ctx_), DCC_MODAL((custom_id_), (title_), __VA_ARGS__), (cb_), (user_data_))
#define DCC_SHOW_MODAL_V1(ctx_, custom_id_, title_, ...) \
    DCC_SHOW_MODAL_BUILDER((ctx_), (custom_id_), (title_), __VA_ARGS__)
#define DCC_SHOW_MODAL_V1_CB(ctx_, custom_id_, title_, cb_, user_data_, ...) \
    DCC_SHOW_MODAL_BUILDER_CB((ctx_), (custom_id_), (title_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_SHOW_MODAL_V2(ctx_, custom_id_, title_, ...) \
    DCC_SHOW_MODAL((ctx_), DCC_MODAL_V2((custom_id_), (title_), __VA_ARGS__))
#define DCC_SHOW_MODAL_V2_CB(ctx_, custom_id_, title_, cb_, user_data_, ...) \
    DCC_SHOW_MODAL_CB((ctx_), DCC_MODAL_V2((custom_id_), (title_), __VA_ARGS__), (cb_), (user_data_))
#define DCC_CTX_SHOW_MODAL_V2(ctx_, custom_id_, title_, ...) \
    DCC_SHOW_MODAL_V2((ctx_), (custom_id_), (title_), __VA_ARGS__)
#define DCC_CTX_SHOW_MODAL_V2_CB(ctx_, custom_id_, title_, cb_, user_data_, ...) \
    DCC_SHOW_MODAL_V2_CB((ctx_), (custom_id_), (title_), (cb_), (user_data_), __VA_ARGS__)

#define DCC_REPLY_AUTOCOMPLETE(ctx_, autocomplete_) \
    dcc_ctx_reply_autocomplete((ctx_), &((dcc_autocomplete_builder_t[]){ (autocomplete_) })[0], NULL, NULL)

#define DCC_REPLY_AUTOCOMPLETE_CB(ctx_, autocomplete_, cb_, user_data_) \
    dcc_ctx_reply_autocomplete((ctx_), &((dcc_autocomplete_builder_t[]){ (autocomplete_) })[0], (cb_), (user_data_))

#define DCC_REPLY_AUTOCOMPLETE_MATCHING(ctx_, ...) \
    dcc_ctx_reply_autocomplete_matching( \
        (ctx_), \
        (dcc_autocomplete_choice_t[]){ __VA_ARGS__ }, \
        sizeof((dcc_autocomplete_choice_t[]){ __VA_ARGS__ }) / sizeof(dcc_autocomplete_choice_t), \
        DCC_AUTOCOMPLETE_MAX_CHOICES, \
        NULL, \
        NULL \
    )

#define DCC_REPLY_AUTOCOMPLETE_MATCHING_CB(ctx_, cb_, user_data_, ...) \
    dcc_ctx_reply_autocomplete_matching( \
        (ctx_), \
        (dcc_autocomplete_choice_t[]){ __VA_ARGS__ }, \
        sizeof((dcc_autocomplete_choice_t[]){ __VA_ARGS__ }) / sizeof(dcc_autocomplete_choice_t), \
        DCC_AUTOCOMPLETE_MAX_CHOICES, \
        (cb_), \
        (user_data_) \
    )

#endif /* DCC_SUGAR_REPLY_MODAL_AUTOCOMPLETE_H */
