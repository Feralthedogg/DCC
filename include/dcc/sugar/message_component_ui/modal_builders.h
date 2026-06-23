#ifndef DCC_SUGAR_MESSAGE_COMPONENT_UI_MODAL_BUILDERS_H
#define DCC_SUGAR_MESSAGE_COMPONENT_UI_MODAL_BUILDERS_H

#include <dcc/sugar/message_component_ui/legacy_components.h>
#include <dcc/sugar/message_component_ui/v2_inputs.h>

#define DCC_MODAL_BUILDER_ARRAY(custom_id_, title_, components_, component_count_) \
    ((dcc_modal_builder_t){ \
        .custom_id = (custom_id_), \
        .title = (title_), \
        .components = (components_), \
        .components_count = (component_count_), \
        .has_custom_id = 1U, \
        .has_title = 1U \
    })
#define DCC_MODAL_BUILDER(custom_id_, title_, ...) \
    ((dcc_modal_builder_t){ \
        .custom_id = (custom_id_), \
        .title = (title_), \
        .components = DCC_SUGAR_ARRAY(dcc_component_builder_t, __VA_ARGS__), \
        .components_count = DCC_SUGAR_ARRAY_LEN(dcc_component_builder_t, __VA_ARGS__), \
        .has_custom_id = 1U, \
        .has_title = 1U \
    })
#define DCC_MODAL_V2_BUILDER_ARRAY(custom_id_, title_, components_, component_count_) \
    ((dcc_modal_builder_t){ \
        .custom_id = (custom_id_), \
        .title = (title_), \
        .components_v2 = (components_), \
        .components_v2_count = (component_count_), \
        .has_custom_id = 1U, \
        .has_title = 1U \
    })
#define DCC_MODAL_V2_BUILDER(custom_id_, title_, ...) \
    ((dcc_modal_builder_t){ \
        .custom_id = (custom_id_), \
        .title = (title_), \
        .components_v2 = DCC_SUGAR_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        .components_v2_count = DCC_SUGAR_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__), \
        .has_custom_id = 1U, \
        .has_title = 1U \
    })

#endif /* DCC_SUGAR_MESSAGE_COMPONENT_UI_MODAL_BUILDERS_H */
