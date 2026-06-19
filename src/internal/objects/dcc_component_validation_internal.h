#ifndef DCC_COMPONENT_VALIDATION_INTERNAL_H
#define DCC_COMPONENT_VALIDATION_INTERNAL_H

#include <dcc/component.h>

#ifdef __cplusplus
extern "C" {
#endif

int dcc_button_style_valid(dcc_button_style_t style);
int dcc_text_input_style_valid(dcc_text_input_style_t style);
dcc_status_t dcc_component_validate_emoji(const dcc_component_emoji_t *emoji);
dcc_status_t dcc_component_validate(const dcc_component_builder_t *builder);

#ifdef __cplusplus
}
#endif

#endif
