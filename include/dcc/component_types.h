#ifndef DCC_COMPONENT_TYPES_H
#define DCC_COMPONENT_TYPES_H

#include <dcc/snowflake.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_button_style {
  DCC_BUTTON_PRIMARY = 1,
  DCC_BUTTON_SECONDARY = 2,
  DCC_BUTTON_SUCCESS = 3,
  DCC_BUTTON_DANGER = 4,
  DCC_BUTTON_LINK = 5,
  DCC_BUTTON_PREMIUM = 6,
} dcc_button_style_t;

typedef enum dcc_text_input_style {
  DCC_TEXT_INPUT_SHORT = 1,
  DCC_TEXT_INPUT_PARAGRAPH = 2,
} dcc_text_input_style_t;

typedef struct dcc_component_emoji {
  const char *name;
  dcc_snowflake_t id;
  uint8_t animated;
  uint8_t has_id;
  uint8_t has_animated;
} dcc_component_emoji_t;

typedef struct dcc_select_option {
  const char *label;
  const char *value;
  const char *description;
  dcc_component_emoji_t emoji;
  uint8_t is_default;
  uint8_t has_default;
  uint8_t has_emoji;
} dcc_select_option_t;

#ifdef __cplusplus
}
#endif

#endif
