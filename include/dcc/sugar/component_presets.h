#ifndef DCC_SUGAR_COMPONENT_PRESETS_H
#define DCC_SUGAR_COMPONENT_PRESETS_H

#include <dcc/app.h>
#include <dcc/component.h>
#include <dcc/component_v2.h>
#include <dcc/embed.h>
#include <dcc/message.h>
#include <dcc/sugar/reply.h>

#define DCC_SUGAR_PRESET_ARRAY(type_, ...) ((type_[]){ __VA_ARGS__ })
#define DCC_SUGAR_PRESET_ARRAY_LEN(type_, ...) (sizeof((type_[]){ __VA_ARGS__ }) / sizeof(type_))

#define DCC_BUTTON_BUILDER_DISABLED(style_, label_, custom_id_, disabled_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_BUTTON, \
        .style = (style_), \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .disabled = (disabled_), \
        .has_style = 1U, \
        .has_disabled = 1U \
    })

#define DCC_BUTTON_PRIMARY_BUILDER_DISABLED(label_, custom_id_, disabled_) \
    DCC_BUTTON_BUILDER_DISABLED(DCC_BUTTON_PRIMARY, (label_), (custom_id_), (disabled_))
#define DCC_BUTTON_SECONDARY_BUILDER_DISABLED(label_, custom_id_, disabled_) \
    DCC_BUTTON_BUILDER_DISABLED(DCC_BUTTON_SECONDARY, (label_), (custom_id_), (disabled_))
#define DCC_BUTTON_SUCCESS_BUILDER_DISABLED(label_, custom_id_, disabled_) \
    DCC_BUTTON_BUILDER_DISABLED(DCC_BUTTON_SUCCESS, (label_), (custom_id_), (disabled_))
#define DCC_BUTTON_DANGER_BUILDER_DISABLED(label_, custom_id_, disabled_) \
    DCC_BUTTON_BUILDER_DISABLED(DCC_BUTTON_DANGER, (label_), (custom_id_), (disabled_))

#define DCC_V2_BUTTON_DISABLED(style_, label_, custom_id_, disabled_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_BUTTON, \
        .button_style = (style_), \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .disabled = (disabled_), \
        .has_button_style = 1U, \
        .has_disabled = 1U \
    })

#define DCC_V2_BUTTON_PRIMARY_DISABLED(label_, custom_id_, disabled_) \
    DCC_V2_BUTTON_DISABLED(DCC_BUTTON_PRIMARY, (label_), (custom_id_), (disabled_))
#define DCC_V2_BUTTON_SECONDARY_DISABLED(label_, custom_id_, disabled_) \
    DCC_V2_BUTTON_DISABLED(DCC_BUTTON_SECONDARY, (label_), (custom_id_), (disabled_))
#define DCC_V2_BUTTON_SUCCESS_DISABLED(label_, custom_id_, disabled_) \
    DCC_V2_BUTTON_DISABLED(DCC_BUTTON_SUCCESS, (label_), (custom_id_), (disabled_))
#define DCC_V2_BUTTON_DANGER_DISABLED(label_, custom_id_, disabled_) \
    DCC_V2_BUTTON_DISABLED(DCC_BUTTON_DANGER, (label_), (custom_id_), (disabled_))

#define DCC_SUGAR_PRESET_V2_TEXT(content_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_TEXT_DISPLAY, \
        .content = (content_) \
    })

#define DCC_SUGAR_PRESET_V2_ACTION_ROW(...) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_ACTION_ROW, \
        .children = DCC_SUGAR_PRESET_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        .children_count = DCC_SUGAR_PRESET_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__) \
    })

#define DCC_SUGAR_PRESET_V2_CONTAINER(...) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_CONTAINER, \
        .children = DCC_SUGAR_PRESET_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        .children_count = DCC_SUGAR_PRESET_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__) \
    })

#define DCC_SUGAR_PRESET_MESSAGE_COMPONENTS_V2(...) \
    ((dcc_message_builder_t){ \
        .components_v2 = DCC_SUGAR_PRESET_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        .components_v2_count = DCC_SUGAR_PRESET_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__), \
        .flags = DCC_MESSAGE_FLAG_IS_COMPONENTS_V2, \
        .has_flags = 1U \
    })

#define DCC_CONFIRM_ROW_LABELS(confirm_label_, confirm_id_, cancel_label_, cancel_id_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_ACTION_ROW, \
        .children = DCC_SUGAR_PRESET_ARRAY( \
            dcc_component_builder_t, \
            DCC_BUTTON_SUCCESS_BUILDER_DISABLED((confirm_label_), (confirm_id_), 0U), \
            DCC_BUTTON_DANGER_BUILDER_DISABLED((cancel_label_), (cancel_id_), 0U) \
        ), \
        .children_count = 2U \
    })

#define DCC_CONFIRM_ROW(confirm_id_, cancel_id_) \
    DCC_CONFIRM_ROW_LABELS("Confirm", (confirm_id_), "Cancel", (cancel_id_))

#define DCC_CONFIRM_MESSAGE_LABELS(content_, confirm_label_, confirm_id_, cancel_label_, cancel_id_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .components = DCC_SUGAR_PRESET_ARRAY( \
            dcc_component_builder_t, \
            DCC_CONFIRM_ROW_LABELS((confirm_label_), (confirm_id_), (cancel_label_), (cancel_id_)) \
        ), \
        .components_count = 1U, \
        .has_content = 1U \
    })

#define DCC_CONFIRM_MESSAGE(content_, confirm_id_, cancel_id_) \
    DCC_CONFIRM_MESSAGE_LABELS((content_), "Confirm", (confirm_id_), "Cancel", (cancel_id_))

#define DCC_CONFIRM_EMBED_MESSAGE_LABELS(embed_, confirm_label_, confirm_id_, cancel_label_, cancel_id_) \
    ((dcc_message_builder_t){ \
        .embeds = DCC_SUGAR_PRESET_ARRAY(dcc_embed_builder_t, (embed_)), \
        .embeds_count = 1U, \
        .components = DCC_SUGAR_PRESET_ARRAY( \
            dcc_component_builder_t, \
            DCC_CONFIRM_ROW_LABELS((confirm_label_), (confirm_id_), (cancel_label_), (cancel_id_)) \
        ), \
        .components_count = 1U \
    })

#define DCC_CONFIRM_EMBED_MESSAGE(embed_, confirm_id_, cancel_id_) \
    DCC_CONFIRM_EMBED_MESSAGE_LABELS((embed_), "Confirm", (confirm_id_), "Cancel", (cancel_id_))

#define DCC_REPLY_CONFIRM(ctx_, content_, confirm_id_, cancel_id_) \
    DCC_REPLY((ctx_), DCC_CONFIRM_MESSAGE((content_), (confirm_id_), (cancel_id_)))
#define DCC_REPLY_CONFIRM_LABELS(ctx_, content_, confirm_label_, confirm_id_, cancel_label_, cancel_id_) \
    DCC_REPLY( \
        (ctx_), \
        DCC_CONFIRM_MESSAGE_LABELS((content_), (confirm_label_), (confirm_id_), (cancel_label_), (cancel_id_)) \
    )

#define DCC_CONFIRM_V2_LABELS(content_, confirm_label_, confirm_id_, cancel_label_, cancel_id_) \
    DCC_SUGAR_PRESET_MESSAGE_COMPONENTS_V2( \
        DCC_SUGAR_PRESET_V2_CONTAINER( \
            DCC_SUGAR_PRESET_V2_TEXT((content_)), \
            DCC_SUGAR_PRESET_V2_ACTION_ROW( \
                DCC_V2_BUTTON_SUCCESS_DISABLED((confirm_label_), (confirm_id_), 0U), \
                DCC_V2_BUTTON_DANGER_DISABLED((cancel_label_), (cancel_id_), 0U) \
            ) \
        ) \
    )

#define DCC_CONFIRM_V2(content_, confirm_id_, cancel_id_) \
    DCC_CONFIRM_V2_LABELS((content_), "Confirm", (confirm_id_), "Cancel", (cancel_id_))

#define DCC_REPLY_CONFIRM_V2(ctx_, content_, confirm_id_, cancel_id_) \
    DCC_REPLY((ctx_), DCC_CONFIRM_V2((content_), (confirm_id_), (cancel_id_)))

#define DCC_PAGINATOR_ROW_LABELS(prev_label_, prev_id_, next_label_, next_id_, at_start_, at_end_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_ACTION_ROW, \
        .children = DCC_SUGAR_PRESET_ARRAY( \
            dcc_component_builder_t, \
            DCC_BUTTON_SECONDARY_BUILDER_DISABLED((prev_label_), (prev_id_), (at_start_)), \
            DCC_BUTTON_SECONDARY_BUILDER_DISABLED((next_label_), (next_id_), (at_end_)) \
        ), \
        .children_count = 2U \
    })

#define DCC_PAGINATOR_ROW(prev_id_, next_id_, at_start_, at_end_) \
    DCC_PAGINATOR_ROW_LABELS("Previous", (prev_id_), "Next", (next_id_), (at_start_), (at_end_))

#define DCC_PAGINATOR_MESSAGE_LABELS(content_, prev_label_, prev_id_, next_label_, next_id_, at_start_, at_end_) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .components = DCC_SUGAR_PRESET_ARRAY( \
            dcc_component_builder_t, \
            DCC_PAGINATOR_ROW_LABELS((prev_label_), (prev_id_), (next_label_), (next_id_), (at_start_), (at_end_)) \
        ), \
        .components_count = 1U, \
        .has_content = 1U \
    })

#define DCC_PAGINATOR_MESSAGE(content_, prev_id_, next_id_, at_start_, at_end_) \
    DCC_PAGINATOR_MESSAGE_LABELS((content_), "Previous", (prev_id_), "Next", (next_id_), (at_start_), (at_end_))

#define DCC_PAGINATOR_EMBED_MESSAGE_LABELS( \
    embed_, \
    prev_label_, \
    prev_id_, \
    next_label_, \
    next_id_, \
    at_start_, \
    at_end_ \
) \
    ((dcc_message_builder_t){ \
        .embeds = DCC_SUGAR_PRESET_ARRAY(dcc_embed_builder_t, (embed_)), \
        .embeds_count = 1U, \
        .components = DCC_SUGAR_PRESET_ARRAY( \
            dcc_component_builder_t, \
            DCC_PAGINATOR_ROW_LABELS((prev_label_), (prev_id_), (next_label_), (next_id_), (at_start_), (at_end_)) \
        ), \
        .components_count = 1U \
    })

#define DCC_PAGINATOR_EMBED_MESSAGE(embed_, prev_id_, next_id_, at_start_, at_end_) \
    DCC_PAGINATOR_EMBED_MESSAGE_LABELS( \
        (embed_), \
        "Previous", \
        (prev_id_), \
        "Next", \
        (next_id_), \
        (at_start_), \
        (at_end_) \
    )

#define DCC_REPLY_PAGINATOR(ctx_, content_, prev_id_, next_id_, at_start_, at_end_) \
    DCC_REPLY((ctx_), DCC_PAGINATOR_MESSAGE((content_), (prev_id_), (next_id_), (at_start_), (at_end_)))

#define DCC_PAGINATOR_V2_LABELS(content_, prev_label_, prev_id_, next_label_, next_id_, at_start_, at_end_) \
    DCC_SUGAR_PRESET_MESSAGE_COMPONENTS_V2( \
        DCC_SUGAR_PRESET_V2_CONTAINER( \
            DCC_SUGAR_PRESET_V2_TEXT((content_)), \
            DCC_SUGAR_PRESET_V2_ACTION_ROW( \
                DCC_V2_BUTTON_SECONDARY_DISABLED((prev_label_), (prev_id_), (at_start_)), \
                DCC_V2_BUTTON_SECONDARY_DISABLED((next_label_), (next_id_), (at_end_)) \
            ) \
        ) \
    )

#define DCC_PAGINATOR_V2(content_, prev_id_, next_id_, at_start_, at_end_) \
    DCC_PAGINATOR_V2_LABELS((content_), "Previous", (prev_id_), "Next", (next_id_), (at_start_), (at_end_))

#define DCC_REPLY_PAGINATOR_V2(ctx_, content_, prev_id_, next_id_, at_start_, at_end_) \
    DCC_REPLY((ctx_), DCC_PAGINATOR_V2((content_), (prev_id_), (next_id_), (at_start_), (at_end_)))

#define DCC_SETTINGS_PANEL_ROW_LABELS( \
    edit_label_, \
    edit_id_, \
    enable_label_, \
    enable_id_, \
    disable_label_, \
    disable_id_, \
    reset_label_, \
    reset_id_, \
    enabled_ \
) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_ACTION_ROW, \
        .children = DCC_SUGAR_PRESET_ARRAY( \
            dcc_component_builder_t, \
            DCC_BUTTON_PRIMARY_BUILDER_DISABLED((edit_label_), (edit_id_), 0U), \
            DCC_BUTTON_SUCCESS_BUILDER_DISABLED((enable_label_), (enable_id_), (enabled_)), \
            DCC_BUTTON_DANGER_BUILDER_DISABLED((disable_label_), (disable_id_), !(enabled_)), \
            DCC_BUTTON_SECONDARY_BUILDER_DISABLED((reset_label_), (reset_id_), 0U) \
        ), \
        .children_count = 4U \
    })

#define DCC_SETTINGS_PANEL_ROW(edit_id_, enable_id_, disable_id_, reset_id_, enabled_) \
    DCC_SETTINGS_PANEL_ROW_LABELS( \
        "Edit", \
        (edit_id_), \
        "Enable", \
        (enable_id_), \
        "Disable", \
        (disable_id_), \
        "Reset", \
        (reset_id_), \
        (enabled_) \
    )

#define DCC_SETTINGS_PANEL_MESSAGE_LABELS( \
    content_, \
    edit_label_, \
    edit_id_, \
    enable_label_, \
    enable_id_, \
    disable_label_, \
    disable_id_, \
    reset_label_, \
    reset_id_, \
    enabled_ \
) \
    ((dcc_message_builder_t){ \
        .content = (content_), \
        .components = DCC_SUGAR_PRESET_ARRAY( \
            dcc_component_builder_t, \
            DCC_SETTINGS_PANEL_ROW_LABELS( \
                (edit_label_), \
                (edit_id_), \
                (enable_label_), \
                (enable_id_), \
                (disable_label_), \
                (disable_id_), \
                (reset_label_), \
                (reset_id_), \
                (enabled_) \
            ) \
        ), \
        .components_count = 1U, \
        .has_content = 1U \
    })

#define DCC_SETTINGS_PANEL_MESSAGE(content_, edit_id_, enable_id_, disable_id_, reset_id_, enabled_) \
    DCC_SETTINGS_PANEL_MESSAGE_LABELS( \
        (content_), \
        "Edit", \
        (edit_id_), \
        "Enable", \
        (enable_id_), \
        "Disable", \
        (disable_id_), \
        "Reset", \
        (reset_id_), \
        (enabled_) \
    )

#define DCC_REPLY_SETTINGS_PANEL(ctx_, content_, edit_id_, enable_id_, disable_id_, reset_id_, enabled_) \
    DCC_REPLY((ctx_), DCC_SETTINGS_PANEL_MESSAGE((content_), (edit_id_), (enable_id_), (disable_id_), (reset_id_), (enabled_)))

#define DCC_SETTINGS_PANEL_V2_LABELS( \
    content_, \
    edit_label_, \
    edit_id_, \
    enable_label_, \
    enable_id_, \
    disable_label_, \
    disable_id_, \
    reset_label_, \
    reset_id_, \
    enabled_ \
) \
    DCC_SUGAR_PRESET_MESSAGE_COMPONENTS_V2( \
        DCC_SUGAR_PRESET_V2_CONTAINER( \
            DCC_SUGAR_PRESET_V2_TEXT((content_)), \
            DCC_SUGAR_PRESET_V2_ACTION_ROW( \
                DCC_V2_BUTTON_PRIMARY_DISABLED((edit_label_), (edit_id_), 0U), \
                DCC_V2_BUTTON_SUCCESS_DISABLED((enable_label_), (enable_id_), (enabled_)), \
                DCC_V2_BUTTON_DANGER_DISABLED((disable_label_), (disable_id_), !(enabled_)), \
                DCC_V2_BUTTON_SECONDARY_DISABLED((reset_label_), (reset_id_), 0U) \
            ) \
        ) \
    )

#define DCC_SETTINGS_PANEL_V2(content_, edit_id_, enable_id_, disable_id_, reset_id_, enabled_) \
    DCC_SETTINGS_PANEL_V2_LABELS( \
        (content_), \
        "Edit", \
        (edit_id_), \
        "Enable", \
        (enable_id_), \
        "Disable", \
        (disable_id_), \
        "Reset", \
        (reset_id_), \
        (enabled_) \
    )

#define DCC_REPLY_SETTINGS_PANEL_V2(ctx_, content_, edit_id_, enable_id_, disable_id_, reset_id_, enabled_) \
    DCC_REPLY((ctx_), DCC_SETTINGS_PANEL_V2((content_), (edit_id_), (enable_id_), (disable_id_), (reset_id_), (enabled_)))

#endif
