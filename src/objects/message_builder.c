#include <dcc/message.h>

#include <stdlib.h>
#include <string.h>

void dcc_message_builder_init(dcc_message_builder_t *builder) {
    if (builder != NULL) {
        memset(builder, 0, sizeof(*builder));
    }
}

void dcc_message_builder_json_free(char *json) {
    free(json);
}
