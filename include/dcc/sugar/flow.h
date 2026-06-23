#ifndef DCC_SUGAR_FLOW_H
#define DCC_SUGAR_FLOW_H

#include <dcc/interaction_flow.h>
#include <dcc/message.h>
#include <dcc/modal.h>
#include <dcc/sugar/flags.h>

#define DCC_FLOW_MESSAGE_TEXT(content_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .has_content = 1U \
    })

#define DCC_FLOW(client_, interaction_) \
    ((dcc_interaction_flow_t){ \
        .size = sizeof(dcc_interaction_flow_t), \
        .client = (client_), \
        .interaction = (interaction_), \
        .state = DCC_INTERACTION_FLOW_READY \
    })

#define DCC_FLOW_STARTED(client_, interaction_, started_at_ms_) \
    ((dcc_interaction_flow_t){ \
        .size = sizeof(dcc_interaction_flow_t), \
        .client = (client_), \
        .interaction = (interaction_), \
        .state = DCC_INTERACTION_FLOW_READY, \
        .started_at_ms = (started_at_ms_) \
    })

#define DCC_FLOW_AUTO_DEFER(client_, interaction_, after_ms_) \
    ((dcc_interaction_flow_t){ \
        .size = sizeof(dcc_interaction_flow_t), \
        .client = (client_), \
        .interaction = (interaction_), \
        .state = DCC_INTERACTION_FLOW_READY, \
        .auto_defer_after_ms = (after_ms_) \
    })

#define DCC_FLOW_AUTO_DEFER_STARTED(client_, interaction_, started_at_ms_, after_ms_) \
    ((dcc_interaction_flow_t){ \
        .size = sizeof(dcc_interaction_flow_t), \
        .client = (client_), \
        .interaction = (interaction_), \
        .state = DCC_INTERACTION_FLOW_READY, \
        .started_at_ms = (started_at_ms_), \
        .auto_defer_after_ms = (after_ms_) \
    })

#define DCC_FLOW_AUTO_DEFER_EPHEMERAL(client_, interaction_, after_ms_) \
    ((dcc_interaction_flow_t){ \
        .size = sizeof(dcc_interaction_flow_t), \
        .client = (client_), \
        .interaction = (interaction_), \
        .state = DCC_INTERACTION_FLOW_READY, \
        .auto_defer_after_ms = (after_ms_), \
        .auto_defer_ephemeral = 1U \
    })

#define DCC_FLOW_AUTO_DEFER_EPHEMERAL_STARTED(client_, interaction_, started_at_ms_, after_ms_) \
    ((dcc_interaction_flow_t){ \
        .size = sizeof(dcc_interaction_flow_t), \
        .client = (client_), \
        .interaction = (interaction_), \
        .state = DCC_INTERACTION_FLOW_READY, \
        .started_at_ms = (started_at_ms_), \
        .auto_defer_after_ms = (after_ms_), \
        .auto_defer_ephemeral = 1U \
    })

#define DCC_FLOW_AUTO_DEFER_PRIVATE(client_, interaction_, after_ms_) \
    DCC_FLOW_AUTO_DEFER_EPHEMERAL((client_), (interaction_), (after_ms_))

#define DCC_FLOW_AUTO_DEFER_PRIVATE_STARTED(client_, interaction_, started_at_ms_, after_ms_) \
    DCC_FLOW_AUTO_DEFER_EPHEMERAL_STARTED((client_), (interaction_), (started_at_ms_), (after_ms_))

#define DCC_FLOW_DEFER(flow_) dcc_flow_defer((flow_), NULL, NULL)
#define DCC_FLOW_DEFER_CB(flow_, cb_, user_data_) \
    dcc_flow_defer((flow_), (cb_), (user_data_))
#define DCC_FLOW_DEFER_EPHEMERAL(flow_) \
    dcc_flow_defer_ephemeral((flow_), NULL, NULL)
#define DCC_FLOW_DEFER_EPHEMERAL_CB(flow_, cb_, user_data_) \
    dcc_flow_defer_ephemeral((flow_), (cb_), (user_data_))
#define DCC_FLOW_DEFER_PRIVATE(flow_) DCC_FLOW_DEFER_EPHEMERAL((flow_))
#define DCC_FLOW_DEFER_PRIVATE_CB(flow_, cb_, user_data_) \
    DCC_FLOW_DEFER_EPHEMERAL_CB((flow_), (cb_), (user_data_))
#define DCC_FLOW_DEFER_UPDATE(flow_) dcc_flow_defer_update((flow_), NULL, NULL)
#define DCC_FLOW_DEFER_UPDATE_CB(flow_, cb_, user_data_) \
    dcc_flow_defer_update((flow_), (cb_), (user_data_))

#define DCC_FLOW_MAYBE_AUTO_DEFER(flow_, now_ms_) \
    dcc_flow_maybe_auto_defer((flow_), (now_ms_), NULL, NULL)
#define DCC_FLOW_MAYBE_AUTO_DEFER_CB(flow_, now_ms_, cb_, user_data_) \
    dcc_flow_maybe_auto_defer((flow_), (now_ms_), (cb_), (user_data_))

#define DCC_FLOW_REPLY(flow_, message_) \
    dcc_flow_reply((flow_), &((dcc_message_builder_t[]){ (message_) })[0], NULL, NULL)
#define DCC_FLOW_REPLY_CB(flow_, message_, cb_, user_data_) \
    dcc_flow_reply((flow_), &((dcc_message_builder_t[]){ (message_) })[0], (cb_), (user_data_))
#define DCC_FLOW_REPLY_TEXT(flow_, content_) \
    DCC_FLOW_REPLY((flow_), DCC_FLOW_MESSAGE_TEXT((content_)))
#define DCC_FLOW_REPLY_TEXT_CB(flow_, content_, cb_, user_data_) \
    DCC_FLOW_REPLY_CB((flow_), DCC_FLOW_MESSAGE_TEXT((content_)), (cb_), (user_data_))
#define DCC_FLOW_REPLY_PRIVATE(flow_, content_) \
    DCC_FLOW_REPLY((flow_), DCC_MESSAGE_TEXT_PRIVATE((content_)))
#define DCC_FLOW_REPLY_PRIVATE_CB(flow_, content_, cb_, user_data_) \
    DCC_FLOW_REPLY_CB((flow_), DCC_MESSAGE_TEXT_PRIVATE((content_)), (cb_), (user_data_))
#define DCC_FLOW_REPLY_SILENT(flow_, content_) \
    DCC_FLOW_REPLY((flow_), DCC_MESSAGE_TEXT_SILENT((content_)))
#define DCC_FLOW_REPLY_SILENT_CB(flow_, content_, cb_, user_data_) \
    DCC_FLOW_REPLY_CB((flow_), DCC_MESSAGE_TEXT_SILENT((content_)), (cb_), (user_data_))

#define DCC_FLOW_EDIT_ORIGINAL(flow_, message_) \
    dcc_flow_edit_original((flow_), &((dcc_message_builder_t[]){ (message_) })[0], NULL, NULL)
#define DCC_FLOW_EDIT_ORIGINAL_CB(flow_, message_, cb_, user_data_) \
    dcc_flow_edit_original( \
        (flow_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        (cb_), \
        (user_data_) \
    )
#define DCC_FLOW_EDIT_TEXT(flow_, content_) \
    DCC_FLOW_EDIT_ORIGINAL((flow_), DCC_FLOW_MESSAGE_TEXT((content_)))
#define DCC_FLOW_EDIT_TEXT_CB(flow_, content_, cb_, user_data_) \
    DCC_FLOW_EDIT_ORIGINAL_CB((flow_), DCC_FLOW_MESSAGE_TEXT((content_)), (cb_), (user_data_))

#define DCC_FLOW_FOLLOWUP(flow_, message_) \
    dcc_flow_followup((flow_), &((dcc_message_builder_t[]){ (message_) })[0], NULL, NULL)
#define DCC_FLOW_FOLLOWUP_CB(flow_, message_, cb_, user_data_) \
    dcc_flow_followup((flow_), &((dcc_message_builder_t[]){ (message_) })[0], (cb_), (user_data_))
#define DCC_FLOW_FOLLOWUP_TEXT(flow_, content_) \
    DCC_FLOW_FOLLOWUP((flow_), DCC_FLOW_MESSAGE_TEXT((content_)))
#define DCC_FLOW_FOLLOWUP_TEXT_CB(flow_, content_, cb_, user_data_) \
    DCC_FLOW_FOLLOWUP_CB((flow_), DCC_FLOW_MESSAGE_TEXT((content_)), (cb_), (user_data_))
#define DCC_FLOW_FOLLOWUP_PRIVATE(flow_, content_) \
    DCC_FLOW_FOLLOWUP((flow_), DCC_MESSAGE_TEXT_PRIVATE((content_)))
#define DCC_FLOW_FOLLOWUP_PRIVATE_CB(flow_, content_, cb_, user_data_) \
    DCC_FLOW_FOLLOWUP_CB((flow_), DCC_MESSAGE_TEXT_PRIVATE((content_)), (cb_), (user_data_))
#define DCC_FLOW_FOLLOWUP_SILENT(flow_, content_) \
    DCC_FLOW_FOLLOWUP((flow_), DCC_MESSAGE_TEXT_SILENT((content_)))
#define DCC_FLOW_FOLLOWUP_SILENT_CB(flow_, content_, cb_, user_data_) \
    DCC_FLOW_FOLLOWUP_CB((flow_), DCC_MESSAGE_TEXT_SILENT((content_)), (cb_), (user_data_))

#define DCC_FLOW_SHOW_MODAL(flow_, modal_) \
    dcc_flow_show_modal((flow_), &((dcc_modal_builder_t[]){ (modal_) })[0], NULL, NULL)
#define DCC_FLOW_SHOW_MODAL_CB(flow_, modal_, cb_, user_data_) \
    dcc_flow_show_modal((flow_), &((dcc_modal_builder_t[]){ (modal_) })[0], (cb_), (user_data_))

#define DCC_FLOW_STATE(flow_) dcc_flow_state((flow_))
#define DCC_FLOW_STATE_NAME(flow_) dcc_flow_state_string(dcc_flow_state((flow_)))

#endif
