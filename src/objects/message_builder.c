#include <dcc/message.h>

#include <stdlib.h>
#include <string.h>

void dcc_message_builder_init(dcc_message_builder_t *builder) {
    if (builder != NULL) {
        memset(builder, 0, sizeof(*builder));
        builder->size = sizeof(*builder);
        builder->version = DCC_MESSAGE_BUILDER_VERSION;
    }
}

void dcc_poll_builder_init(dcc_poll_builder_t *builder) {
    if (builder != NULL) {
        memset(builder, 0, sizeof(*builder));
        builder->size = sizeof(*builder);
        builder->version = DCC_POLL_BUILDER_VERSION;
    }
}

void dcc_allowed_mentions_builder_init(dcc_allowed_mentions_builder_t *builder) {
    if (builder != NULL) {
        memset(builder, 0, sizeof(*builder));
        builder->size = sizeof(*builder);
        builder->version = DCC_ALLOWED_MENTIONS_BUILDER_VERSION;
    }
}

void dcc_message_builder_json_free(char *json) {
    free(json);
}
