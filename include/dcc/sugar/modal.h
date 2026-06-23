#ifndef DCC_SUGAR_MODAL_H
#define DCC_SUGAR_MODAL_H

#include <dcc/component.h>
#include <dcc/component_v2.h>
#include <dcc/modal.h>

#ifndef DCC_SUGAR_ARRAY
#define DCC_SUGAR_ARRAY(type_, ...) ((type_[]){ __VA_ARGS__ })
#endif

#ifndef DCC_SUGAR_ARRAY_LEN
#define DCC_SUGAR_ARRAY_LEN(type_, ...) (sizeof((type_[]){ __VA_ARGS__ }) / sizeof(type_))
#endif

#ifndef DCC_MODAL_BUILDER_ARRAY
#define DCC_MODAL_BUILDER_ARRAY(custom_id_, title_, components_, component_count_) \
    ((dcc_modal_builder_t){ \
        .custom_id = (custom_id_), \
        .title = (title_), \
        .components = (components_), \
        .components_count = (component_count_), \
        .has_custom_id = 1U, \
        .has_title = 1U \
    })
#endif

#ifndef DCC_MODAL_BUILDER
#define DCC_MODAL_BUILDER(custom_id_, title_, ...) \
    ((dcc_modal_builder_t){ \
        .custom_id = (custom_id_), \
        .title = (title_), \
        .components = DCC_SUGAR_ARRAY(dcc_component_builder_t, __VA_ARGS__), \
        .components_count = DCC_SUGAR_ARRAY_LEN(dcc_component_builder_t, __VA_ARGS__), \
        .has_custom_id = 1U, \
        .has_title = 1U \
    })
#endif

#ifndef DCC_MODAL_V2_BUILDER_ARRAY
#define DCC_MODAL_V2_BUILDER_ARRAY(custom_id_, title_, components_, component_count_) \
    ((dcc_modal_builder_t){ \
        .custom_id = (custom_id_), \
        .title = (title_), \
        .components_v2 = (components_), \
        .components_v2_count = (component_count_), \
        .has_custom_id = 1U, \
        .has_title = 1U \
    })
#endif

#ifndef DCC_MODAL_V2_BUILDER
#define DCC_MODAL_V2_BUILDER(custom_id_, title_, ...) \
    ((dcc_modal_builder_t){ \
        .custom_id = (custom_id_), \
        .title = (title_), \
        .components_v2 = DCC_SUGAR_ARRAY(dcc_component_v2_builder_t, __VA_ARGS__), \
        .components_v2_count = DCC_SUGAR_ARRAY_LEN(dcc_component_v2_builder_t, __VA_ARGS__), \
        .has_custom_id = 1U, \
        .has_title = 1U \
    })
#endif

#ifndef DCC_MODAL_COMPONENTS_JSON
#define DCC_MODAL_COMPONENTS_JSON(custom_id_, title_, components_json_) \
    ((dcc_modal_builder_t){ \
        .custom_id = (custom_id_), \
        .title = (title_), \
        .components_json = (components_json_), \
        .has_custom_id = 1U, \
        .has_title = 1U \
    })
#endif

#ifndef DCC_MODAL_COMPONENTS_V2_JSON
#define DCC_MODAL_COMPONENTS_V2_JSON(custom_id_, title_, components_json_) \
    ((dcc_modal_builder_t){ \
        .custom_id = (custom_id_), \
        .title = (title_), \
        .components_v2_json = (components_json_), \
        .has_custom_id = 1U, \
        .has_title = 1U \
    })
#endif

#ifndef DCC_MODAL
#define DCC_MODAL(custom_id_, title_, ...) DCC_MODAL_BUILDER((custom_id_), (title_), __VA_ARGS__)
#endif

#ifndef DCC_MODAL_V2
#define DCC_MODAL_V2(custom_id_, title_, ...) DCC_MODAL_V2_BUILDER((custom_id_), (title_), __VA_ARGS__)
#endif

#define DCC_SUGAR_MODAL_ROW(child_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_ACTION_ROW, \
        .children = (dcc_component_builder_t[]){ (child_) }, \
        .children_count = 1U \
    })

#define DCC_SUGAR_MODAL_TEXT_INPUT(custom_id_, label_, style_, placeholder_, required_) \
    ((dcc_component_builder_t){ \
        .type = DCC_COMPONENT_TEXT_INPUT, \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .text_input_style = (style_), \
        .placeholder = (placeholder_), \
        .required = (required_), \
        .has_text_input_style = 1U, \
        .has_required = 1U \
    })

#define DCC_MODAL_FIELD_TEXT_REQUIRED(custom_id_, label_, required_) \
    DCC_SUGAR_MODAL_ROW( \
        DCC_SUGAR_MODAL_TEXT_INPUT((custom_id_), (label_), DCC_TEXT_INPUT_SHORT, NULL, (required_)) \
    )
#define DCC_MODAL_FIELD_TEXT(custom_id_, label_) \
    DCC_MODAL_FIELD_TEXT_REQUIRED((custom_id_), (label_), 1U)
#define DCC_MODAL_FIELD_TEXT_OPTIONAL(custom_id_, label_) \
    DCC_MODAL_FIELD_TEXT_REQUIRED((custom_id_), (label_), 0U)
#define DCC_MODAL_FIELD_TEXT_PLACEHOLDER(custom_id_, label_, placeholder_) \
    DCC_SUGAR_MODAL_ROW( \
        DCC_SUGAR_MODAL_TEXT_INPUT((custom_id_), (label_), DCC_TEXT_INPUT_SHORT, (placeholder_), 1U) \
    )
#define DCC_MODAL_FIELD_TEXT_OPTIONAL_PLACEHOLDER(custom_id_, label_, placeholder_) \
    DCC_SUGAR_MODAL_ROW( \
        DCC_SUGAR_MODAL_TEXT_INPUT((custom_id_), (label_), DCC_TEXT_INPUT_SHORT, (placeholder_), 0U) \
    )

#define DCC_MODAL_FIELD_PARAGRAPH_REQUIRED(custom_id_, label_, required_) \
    DCC_SUGAR_MODAL_ROW( \
        DCC_SUGAR_MODAL_TEXT_INPUT((custom_id_), (label_), DCC_TEXT_INPUT_PARAGRAPH, NULL, (required_)) \
    )
#define DCC_MODAL_FIELD_PARAGRAPH(custom_id_, label_) \
    DCC_MODAL_FIELD_PARAGRAPH_REQUIRED((custom_id_), (label_), 1U)
#define DCC_MODAL_FIELD_PARAGRAPH_OPTIONAL(custom_id_, label_) \
    DCC_MODAL_FIELD_PARAGRAPH_REQUIRED((custom_id_), (label_), 0U)
#define DCC_MODAL_FIELD_PARAGRAPH_PLACEHOLDER(custom_id_, label_, placeholder_) \
    DCC_SUGAR_MODAL_ROW( \
        DCC_SUGAR_MODAL_TEXT_INPUT((custom_id_), (label_), DCC_TEXT_INPUT_PARAGRAPH, (placeholder_), 1U) \
    )
#define DCC_MODAL_FIELD_PARAGRAPH_OPTIONAL_PLACEHOLDER(custom_id_, label_, placeholder_) \
    DCC_SUGAR_MODAL_ROW( \
        DCC_SUGAR_MODAL_TEXT_INPUT((custom_id_), (label_), DCC_TEXT_INPUT_PARAGRAPH, (placeholder_), 0U) \
    )

#define DCC_SUGAR_MODAL_V2_LABEL(label_, child_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_LABEL, \
        .label = (label_), \
        .children = (dcc_component_v2_builder_t[]){ (child_) }, \
        .children_count = 1U \
    })

#define DCC_SUGAR_MODAL_V2_TEXT_INPUT(custom_id_, label_, style_, placeholder_, required_) \
    ((dcc_component_v2_builder_t){ \
        .type = DCC_COMPONENT_V2_TEXT_INPUT, \
        .custom_id = (custom_id_), \
        .label = (label_), \
        .text_input_style = (style_), \
        .placeholder = (placeholder_), \
        .required = (required_), \
        .has_text_input_style = 1U, \
        .has_required = 1U \
    })

#define DCC_MODAL_V2_FIELD_TEXT_REQUIRED(custom_id_, label_, required_) \
    DCC_SUGAR_MODAL_V2_LABEL( \
        (label_), \
        DCC_SUGAR_MODAL_V2_TEXT_INPUT((custom_id_), (label_), DCC_TEXT_INPUT_SHORT, NULL, (required_)) \
    )
#define DCC_MODAL_V2_FIELD_TEXT(custom_id_, label_) \
    DCC_MODAL_V2_FIELD_TEXT_REQUIRED((custom_id_), (label_), 1U)
#define DCC_MODAL_V2_FIELD_TEXT_OPTIONAL(custom_id_, label_) \
    DCC_MODAL_V2_FIELD_TEXT_REQUIRED((custom_id_), (label_), 0U)
#define DCC_MODAL_V2_FIELD_TEXT_PLACEHOLDER(custom_id_, label_, placeholder_) \
    DCC_SUGAR_MODAL_V2_LABEL( \
        (label_), \
        DCC_SUGAR_MODAL_V2_TEXT_INPUT((custom_id_), (label_), DCC_TEXT_INPUT_SHORT, (placeholder_), 1U) \
    )
#define DCC_MODAL_V2_FIELD_TEXT_OPTIONAL_PLACEHOLDER(custom_id_, label_, placeholder_) \
    DCC_SUGAR_MODAL_V2_LABEL( \
        (label_), \
        DCC_SUGAR_MODAL_V2_TEXT_INPUT((custom_id_), (label_), DCC_TEXT_INPUT_SHORT, (placeholder_), 0U) \
    )

#define DCC_MODAL_V2_FIELD_PARAGRAPH_REQUIRED(custom_id_, label_, required_) \
    DCC_SUGAR_MODAL_V2_LABEL( \
        (label_), \
        DCC_SUGAR_MODAL_V2_TEXT_INPUT((custom_id_), (label_), DCC_TEXT_INPUT_PARAGRAPH, NULL, (required_)) \
    )
#define DCC_MODAL_V2_FIELD_PARAGRAPH(custom_id_, label_) \
    DCC_MODAL_V2_FIELD_PARAGRAPH_REQUIRED((custom_id_), (label_), 1U)
#define DCC_MODAL_V2_FIELD_PARAGRAPH_OPTIONAL(custom_id_, label_) \
    DCC_MODAL_V2_FIELD_PARAGRAPH_REQUIRED((custom_id_), (label_), 0U)
#define DCC_MODAL_V2_FIELD_PARAGRAPH_PLACEHOLDER(custom_id_, label_, placeholder_) \
    DCC_SUGAR_MODAL_V2_LABEL( \
        (label_), \
        DCC_SUGAR_MODAL_V2_TEXT_INPUT((custom_id_), (label_), DCC_TEXT_INPUT_PARAGRAPH, (placeholder_), 1U) \
    )
#define DCC_MODAL_V2_FIELD_PARAGRAPH_OPTIONAL_PLACEHOLDER(custom_id_, label_, placeholder_) \
    DCC_SUGAR_MODAL_V2_LABEL( \
        (label_), \
        DCC_SUGAR_MODAL_V2_TEXT_INPUT((custom_id_), (label_), DCC_TEXT_INPUT_PARAGRAPH, (placeholder_), 0U) \
    )

#define DCC_MODAL_V2_FIELD_CHECKBOX(custom_id_, label_, checked_) \
    DCC_SUGAR_MODAL_V2_LABEL( \
        (label_), \
        ((dcc_component_v2_builder_t){ \
            .type = DCC_COMPONENT_V2_CHECKBOX, \
            .custom_id = (custom_id_), \
            .label = (label_), \
            .checked = (checked_), \
            .has_checked = 1U \
        }) \
    )

#define DCC_MODAL_V2_FIELD_FILE(custom_id_, label_) \
    DCC_SUGAR_MODAL_V2_LABEL( \
        (label_), \
        ((dcc_component_v2_builder_t){ \
            .type = DCC_COMPONENT_V2_FILE_UPLOAD, \
            .custom_id = (custom_id_) \
        }) \
    )

#endif
