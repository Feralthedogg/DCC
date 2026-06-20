#include <dcc/component_v2.h>
#include <dcc/objects/base.h>

#include <string.h>

static int dcc_component_v2_index_valid(const dcc_message_t *message, uint8_t index) {
    return message != NULL &&
        message->components != NULL &&
        (size_t)index < message->components_count;
}

const dcc_component_v2_t *dcc_message_component_at(
    const dcc_message_t *message,
    size_t index
) {
    if (message == NULL || message->components == NULL || index >= message->components_count) {
        return NULL;
    }
    return &message->components[index];
}

const dcc_component_v2_t *dcc_message_component_root(
    const dcc_message_t *message,
    size_t index
) {
    if (message == NULL || index >= message->component_root_count) {
        return NULL;
    }
    uint8_t root_index = message->component_root_indices[index];
    if (!dcc_component_v2_index_valid(message, root_index)) {
        return NULL;
    }
    return &message->components[root_index];
}

const dcc_component_v2_t *dcc_message_component_by_custom_id(
    const dcc_message_t *message,
    const char *custom_id
) {
    if (message == NULL || message->components == NULL || custom_id == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < message->components_count; ++i) {
        const dcc_component_v2_t *component = &message->components[i];
        if (component->custom_id != NULL && strcmp(component->custom_id, custom_id) == 0) {
            return component;
        }
    }
    return NULL;
}

const dcc_component_v2_t *dcc_component_v2_child(
    const dcc_message_t *message,
    const dcc_component_v2_t *component,
    size_t index
) {
    if (message == NULL || component == NULL || index >= component->child_count) {
        return NULL;
    }
    uint8_t child_index = component->child_indices[index];
    if (!dcc_component_v2_index_valid(message, child_index)) {
        return NULL;
    }
    return &message->components[child_index];
}

const dcc_component_v2_t *dcc_component_v2_accessory(
    const dcc_message_t *message,
    const dcc_component_v2_t *component
) {
    if (message == NULL || component == NULL || !component->has_accessory) {
        return NULL;
    }
    if (!dcc_component_v2_index_valid(message, component->accessory_index)) {
        return NULL;
    }
    return &message->components[component->accessory_index];
}

const dcc_component_v2_t *dcc_component_v2_labeled_component(
    const dcc_message_t *message,
    const dcc_component_v2_t *label
) {
    if (message == NULL || label == NULL || !label->has_component) {
        return NULL;
    }
    if (!dcc_component_v2_index_valid(message, label->component_index)) {
        return NULL;
    }
    return &message->components[label->component_index];
}
