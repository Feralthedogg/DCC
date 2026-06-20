#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

static void dcc_free_component_v2_options(dcc_component_v2_t *component) {
    if (component == NULL || component->options == NULL) {
        return;
    }
    dcc_select_option_t *options = (dcc_select_option_t *)component->options;
    for (size_t i = 0; i < component->options_count; ++i) {
        free((void *)options[i].label);
        free((void *)options[i].value);
        free((void *)options[i].description);
        free((void *)options[i].emoji.name);
    }
    free(options);
    component->options = NULL;
    component->options_count = 0;
}

static void dcc_free_component_v2_media(dcc_component_v2_t *component) {
    if (component == NULL || component->media == NULL) {
        return;
    }
    dcc_component_v2_media_t *media = (dcc_component_v2_media_t *)component->media;
    for (size_t i = 0; i < component->media_count; ++i) {
        free((void *)media[i].url);
        free((void *)media[i].description);
    }
    free(media);
    component->media = NULL;
    component->media_count = 0;
}

static void dcc_free_component_v2_fields(dcc_component_v2_t *component) {
    if (component == NULL) {
        return;
    }
    free((void *)component->custom_id);
    free((void *)component->label);
    free((void *)component->description);
    free((void *)component->placeholder);
    free((void *)component->content);
    free((void *)component->url);
    free((void *)component->value);
    dcc_free_component_v2_options(component);
    dcc_free_component_v2_media(component);
    free((void *)component->default_values);
    free((void *)component->channel_types);
    memset(component, 0, sizeof(*component));
}

static int dcc_clone_component_v2_options(
    dcc_component_v2_t *copy,
    const dcc_component_v2_t *component
) {
    if (component->options_count == 0) {
        copy->options = NULL;
        return 0;
    }
    if (component->options == NULL ||
        component->options_count > (size_t)-1 / sizeof(*component->options)) {
        return -1;
    }
    dcc_select_option_t *options =
        (dcc_select_option_t *)calloc(component->options_count, sizeof(*options));
    if (options == NULL) {
        return -1;
    }
    copy->options = options;
    for (size_t i = 0; i < component->options_count; ++i) {
        options[i] = component->options[i];
        options[i].label = dcc_clone_string_or_null(component->options[i].label);
        options[i].value = dcc_clone_string_or_null(component->options[i].value);
        options[i].description = dcc_clone_string_or_null(component->options[i].description);
        options[i].emoji.name = dcc_clone_string_or_null(component->options[i].emoji.name);
        if ((component->options[i].label != NULL && options[i].label == NULL) ||
            (component->options[i].value != NULL && options[i].value == NULL) ||
            (component->options[i].description != NULL && options[i].description == NULL) ||
            (component->options[i].emoji.name != NULL && options[i].emoji.name == NULL)) {
            return -1;
        }
    }
    return 0;
}

static int dcc_clone_component_v2_media(
    dcc_component_v2_t *copy,
    const dcc_component_v2_t *component
) {
    if (component->media_count == 0) {
        copy->media = NULL;
        return 0;
    }
    if (component->media == NULL ||
        component->media_count > (size_t)-1 / sizeof(*component->media)) {
        return -1;
    }
    dcc_component_v2_media_t *media =
        (dcc_component_v2_media_t *)calloc(component->media_count, sizeof(*media));
    if (media == NULL) {
        return -1;
    }
    copy->media = media;
    for (size_t i = 0; i < component->media_count; ++i) {
        media[i] = component->media[i];
        media[i].url = dcc_clone_string_or_null(component->media[i].url);
        media[i].description = dcc_clone_string_or_null(component->media[i].description);
        if ((component->media[i].url != NULL && media[i].url == NULL) ||
            (component->media[i].description != NULL && media[i].description == NULL)) {
            return -1;
        }
    }
    return 0;
}

static int dcc_clone_component_v2_arrays(
    dcc_component_v2_t *copy,
    const dcc_component_v2_t *component
) {
    if (dcc_clone_component_v2_options(copy, component) != 0 ||
        dcc_clone_component_v2_media(copy, component) != 0) {
        return -1;
    }
    if (component->default_value_count != 0) {
        if (component->default_values == NULL ||
            component->default_value_count > (size_t)-1 / sizeof(*component->default_values)) {
            return -1;
        }
        dcc_component_v2_select_default_value_t *defaults =
            (dcc_component_v2_select_default_value_t *)calloc(
                component->default_value_count,
                sizeof(*defaults)
            );
        if (defaults == NULL) {
            return -1;
        }
        memcpy(defaults, component->default_values, component->default_value_count * sizeof(*defaults));
        copy->default_values = defaults;
    } else {
        copy->default_values = NULL;
    }
    if (component->channel_type_count != 0) {
        if (component->channel_types == NULL ||
            component->channel_type_count > (size_t)-1 / sizeof(*component->channel_types)) {
            return -1;
        }
        uint32_t *channel_types =
            (uint32_t *)calloc(component->channel_type_count, sizeof(*channel_types));
        if (channel_types == NULL) {
            return -1;
        }
        memcpy(channel_types, component->channel_types, component->channel_type_count * sizeof(*channel_types));
        copy->channel_types = channel_types;
    } else {
        copy->channel_types = NULL;
    }
    return 0;
}

static int dcc_clone_component_v2_fields(
    dcc_component_v2_t *copy,
    const dcc_component_v2_t *component
) {
    *copy = *component;
    copy->custom_id = dcc_clone_string_or_null(component->custom_id);
    copy->label = dcc_clone_string_or_null(component->label);
    copy->description = dcc_clone_string_or_null(component->description);
    copy->placeholder = dcc_clone_string_or_null(component->placeholder);
    copy->content = dcc_clone_string_or_null(component->content);
    copy->url = dcc_clone_string_or_null(component->url);
    copy->value = dcc_clone_string_or_null(component->value);
    copy->options = NULL;
    copy->media = NULL;
    copy->default_values = NULL;
    copy->channel_types = NULL;
    if ((component->custom_id != NULL && copy->custom_id == NULL) ||
        (component->label != NULL && copy->label == NULL) ||
        (component->description != NULL && copy->description == NULL) ||
        (component->placeholder != NULL && copy->placeholder == NULL) ||
        (component->content != NULL && copy->content == NULL) ||
        (component->url != NULL && copy->url == NULL) ||
        (component->value != NULL && copy->value == NULL)) {
        return -1;
    }
    return dcc_clone_component_v2_arrays(copy, component);
}

static void dcc_free_message_components(dcc_message_t *message) {
    if (message == NULL || message->components == NULL) {
        return;
    }
    dcc_component_v2_t *components = (dcc_component_v2_t *)message->components;
    for (size_t i = 0; i < message->components_count; ++i) {
        dcc_free_component_v2_fields(&components[i]);
    }
    free(components);
    message->components = NULL;
    message->components_count = 0;
    message->component_root_count = 0;
}

static int dcc_clone_message_components(dcc_message_t *copy, const dcc_message_t *message) {
    copy->components = NULL;
    if (message->components_count == 0) {
        return 0;
    }
    if (message->components == NULL ||
        message->components_count > (size_t)-1 / sizeof(*message->components)) {
        return -1;
    }

    dcc_component_v2_t *components =
        (dcc_component_v2_t *)calloc(message->components_count, sizeof(*components));
    if (components == NULL) {
        return -1;
    }
    copy->components = components;
    for (size_t i = 0; i < message->components_count; ++i) {
        if (dcc_clone_component_v2_fields(&components[i], &message->components[i]) != 0) {
            return -1;
        }
    }
    return 0;
}

void dcc_free_message_fields(dcc_message_t *message) {
    if (message == NULL) {
        return;
    }
    dcc_free_user_fields(&message->author);
    free((void *)message->content);
    free((void *)message->timestamp);
    free((void *)message->edited_timestamp);
    free((void *)message->components_json);
    dcc_free_message_components(message);
    message->content = NULL;
    message->timestamp = NULL;
    message->edited_timestamp = NULL;
    message->components_json = NULL;
}

int dcc_clone_message_fields(dcc_message_t *copy, const dcc_message_t *message) {
    if (copy == NULL || message == NULL) {
        return -1;
    }

    *copy = *message;
    memset(&copy->author, 0, sizeof(copy->author));
    copy->content = NULL;
    copy->timestamp = NULL;
    copy->edited_timestamp = NULL;
    copy->components_json = NULL;
    copy->components = NULL;
    if (dcc_clone_user_fields(&copy->author, &message->author) != 0) {
        memset(copy, 0, sizeof(*copy));
        return -1;
    }
    copy->content = dcc_clone_string_or_null(message->content);
    copy->timestamp = dcc_clone_string_or_null(message->timestamp);
    copy->edited_timestamp = dcc_clone_string_or_null(message->edited_timestamp);
    copy->components_json = dcc_clone_string_or_null(message->components_json);
    if ((message->content != NULL && copy->content == NULL) ||
        (message->timestamp != NULL && copy->timestamp == NULL) ||
        (message->edited_timestamp != NULL && copy->edited_timestamp == NULL) ||
        (message->components_json != NULL && copy->components_json == NULL) ||
        dcc_clone_message_components(copy, message) != 0) {
        dcc_free_message_fields(copy);
        memset(copy, 0, sizeof(*copy));
        return -1;
    }

    return 0;
}
