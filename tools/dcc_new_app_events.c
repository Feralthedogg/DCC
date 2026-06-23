#include "internal/dcc_new_app.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCC_NEW_APP_HANDLER_MARKER "/* dcc_new_app:command-handlers */"
#define DCC_NEW_APP_EVENT_ROUTE_MARKER "        /* dcc_new_app:event-routes */"
#define DCC_NEW_APP_EVENT_EXTENSION_MARKER "    /* dcc_new_app:event-extension */"

static char *dcc_new_app_event_cog_source_path(const dcc_new_app_options_t *options) {
    char relative_path[320];
    snprintf(relative_path, sizeof(relative_path), "src/%s.c", options->cog_name);
    return dcc_new_app_join(options->path, relative_path);
}

static int dcc_new_app_event_require_cog_source(char **out_path, const dcc_new_app_options_t *options) {
    char *path = dcc_new_app_event_cog_source_path(options);
    if (path == NULL) {
        return -1;
    }
    if (!dcc_new_app_path_exists(path)) {
        fprintf(stderr, "%s does not exist; add the feature first\n", path);
        free(path);
        return -1;
    }
    *out_path = path;
    return 0;
}

static int dcc_new_app_event_file_contains(const char *path, const char *needle) {
    size_t len = 0U;
    char *data = dcc_new_app_read_file(path, &len);
    (void)len;
    if (data == NULL) {
        fprintf(stderr, "could not read %s for marker update\n", path);
        return -1;
    }
    int found = strstr(data, needle) != NULL;
    free(data);
    return found;
}

static int dcc_new_app_insert_event_route_text(
    const char *path,
    const char *route,
    const char *duplicate_needle
) {
    int has_route_marker = dcc_new_app_event_file_contains(path, DCC_NEW_APP_EVENT_ROUTE_MARKER);
    if (has_route_marker < 0) {
        return -1;
    }
    if (has_route_marker) {
        char insertion[1000];
        snprintf(insertion, sizeof(insertion), "        ,\n%s", route);
        return dcc_new_app_insert_before_marker(
            path,
            DCC_NEW_APP_EVENT_ROUTE_MARKER,
            insertion,
            duplicate_needle
        );
    }

    char insertion[1200];
    snprintf(
        insertion,
        sizeof(insertion),
        "    ,\n"
        "    DCC_FEATURE_EVENTS(\n"
        "%s"
        "        /* dcc_new_app:event-routes */\n"
        "    )\n",
        route
    );
    return dcc_new_app_insert_before_marker(
        path,
        DCC_NEW_APP_EVENT_EXTENSION_MARKER,
        insertion,
        duplicate_needle
    );
}

static void dcc_new_app_event_enum_name(const char *event_name, char *out, size_t out_len) {
    size_t written = 0U;
    const char prefix[] = "DCC_EVENT_";
    if (out_len == 0U) {
        return;
    }
    for (size_t i = 0U; prefix[i] != '\0' && written + 1U < out_len; ++i) {
        out[written++] = prefix[i];
    }
    for (const char *p = event_name; p != NULL && *p != '\0' && written + 1U < out_len; ++p) {
        out[written++] = (char)toupper((unsigned char)*p);
    }
    out[written] = '\0';
}

static int dcc_new_app_event_is_message(const char *event_name) {
    return strcmp(event_name, "message") == 0 ||
           strcmp(event_name, "message_create") == 0 ||
           strcmp(event_name, "message_update") == 0 ||
           strcmp(event_name, "message_delete") == 0;
}

typedef struct dcc_new_app_typed_event_template {
    const char *event_name;
    const char *handler_macro;
    const char *route_macro;
    const char *payload_name;
} dcc_new_app_typed_event_template_t;

static const dcc_new_app_typed_event_template_t *dcc_new_app_typed_event_template(
    const char *event_name
) {
    static const dcc_new_app_typed_event_template_t templates[] = {
        { "guild_create", "DCC_GUILD_CREATE_FN", "DCC_GUILD_CREATE_DATA", "guild" },
        { "guild_update", "DCC_GUILD_UPDATE_FN", "DCC_GUILD_UPDATE_DATA", "guild" },
        { "guild_delete", "DCC_GUILD_DELETE_FN", "DCC_GUILD_DELETE_DATA", "guild" },
        { "member_join", "DCC_MEMBER_JOIN_FN", "DCC_MEMBER_JOIN_DATA", "member" },
        { "member_add", "DCC_MEMBER_JOIN_FN", "DCC_MEMBER_JOIN_DATA", "member" },
        { "guild_member_add", "DCC_MEMBER_JOIN_FN", "DCC_MEMBER_JOIN_DATA", "member" },
        { "member_update", "DCC_MEMBER_UPDATE_FN", "DCC_MEMBER_UPDATE_DATA", "member" },
        { "guild_member_update", "DCC_MEMBER_UPDATE_FN", "DCC_MEMBER_UPDATE_DATA", "member" },
        { "member_remove", "DCC_MEMBER_REMOVE_FN", "DCC_MEMBER_REMOVE_DATA", "member" },
        { "guild_member_remove", "DCC_MEMBER_REMOVE_FN", "DCC_MEMBER_REMOVE_DATA", "member" },
        { "role_create", "DCC_ROLE_CREATE_FN", "DCC_ROLE_CREATE_DATA", "role" },
        { "guild_role_create", "DCC_ROLE_CREATE_FN", "DCC_ROLE_CREATE_DATA", "role" },
        { "role_update", "DCC_ROLE_UPDATE_FN", "DCC_ROLE_UPDATE_DATA", "role" },
        { "guild_role_update", "DCC_ROLE_UPDATE_FN", "DCC_ROLE_UPDATE_DATA", "role" },
        { "role_delete", "DCC_ROLE_DELETE_FN", "DCC_ROLE_DELETE_EVENT_DATA", "role" },
        { "guild_role_delete", "DCC_ROLE_DELETE_FN", "DCC_ROLE_DELETE_EVENT_DATA", "role" },
        { "channel_create", "DCC_CHANNEL_CREATE_FN", "DCC_CHANNEL_CREATE_DATA", "channel" },
        { "channel_update", "DCC_CHANNEL_UPDATE_FN", "DCC_CHANNEL_UPDATE_DATA", "channel" },
        { "channel_delete", "DCC_CHANNEL_DELETE_FN", "DCC_CHANNEL_DELETE_EVENT_DATA", "channel" },
        { "thread_create", "DCC_THREAD_CREATE_FN", "DCC_THREAD_CREATE_DATA", "thread" },
        { "thread_update", "DCC_THREAD_UPDATE_FN", "DCC_THREAD_UPDATE_DATA", "thread" },
        { "thread_delete", "DCC_THREAD_DELETE_FN", "DCC_THREAD_DELETE_EVENT_DATA", "thread" },
        { "reaction_add", "DCC_REACTION_ADD_FN", "DCC_REACTION_ADD_DATA", "reaction" },
        {
            "message_reaction_add",
            "DCC_REACTION_ADD_FN",
            "DCC_REACTION_ADD_DATA",
            "reaction"
        },
        { "reaction_remove", "DCC_REACTION_REMOVE_FN", "DCC_REACTION_REMOVE_DATA", "reaction" },
        {
            "message_reaction_remove",
            "DCC_REACTION_REMOVE_FN",
            "DCC_REACTION_REMOVE_DATA",
            "reaction"
        },
        { "reaction_clear", "DCC_REACTION_CLEAR_FN", "DCC_REACTION_CLEAR_DATA", "reaction" },
        {
            "message_reaction_remove_all",
            "DCC_REACTION_CLEAR_FN",
            "DCC_REACTION_CLEAR_DATA",
            "reaction"
        },
        {
            "reaction_emoji_clear",
            "DCC_REACTION_EMOJI_CLEAR_FN",
            "DCC_REACTION_EMOJI_CLEAR_DATA",
            "reaction"
        },
        {
            "message_reaction_remove_emoji",
            "DCC_REACTION_EMOJI_CLEAR_FN",
            "DCC_REACTION_EMOJI_CLEAR_DATA",
            "reaction"
        },
        { "poll_vote_add", "DCC_POLL_VOTE_ADD_FN", "DCC_POLL_VOTE_ADD_DATA", "poll_vote" },
        {
            "message_poll_vote_add",
            "DCC_POLL_VOTE_ADD_FN",
            "DCC_POLL_VOTE_ADD_DATA",
            "poll_vote"
        },
        {
            "poll_vote_remove",
            "DCC_POLL_VOTE_REMOVE_FN",
            "DCC_POLL_VOTE_REMOVE_DATA",
            "poll_vote"
        },
        {
            "message_poll_vote_remove",
            "DCC_POLL_VOTE_REMOVE_FN",
            "DCC_POLL_VOTE_REMOVE_DATA",
            "poll_vote"
        },
        {
            "voice_state_update",
            "DCC_VOICE_STATE_UPDATE_FN",
            "DCC_VOICE_STATE_UPDATE_DATA",
            "voice_state"
        },
        { "typing_start", "DCC_TYPING_START_FN", "DCC_TYPING_START_DATA", "typing" },
        { "presence_update", "DCC_PRESENCE_UPDATE_FN", "DCC_PRESENCE_UPDATE_DATA", "presence" },
        { "user_update", "DCC_USER_UPDATE_FN", "DCC_USER_UPDATE_DATA", "user" },
        { "guild_ban_add", "DCC_GUILD_BAN_ADD_FN", "DCC_GUILD_BAN_ADD_DATA", "ban" },
        {
            "guild_ban_remove",
            "DCC_GUILD_BAN_REMOVE_FN",
            "DCC_GUILD_BAN_REMOVE_DATA",
            "ban"
        }
    };
    for (size_t i = 0U; i < sizeof(templates) / sizeof(templates[0]); ++i) {
        if (strcmp(event_name, templates[i].event_name) == 0) {
            return &templates[i];
        }
    }
    return NULL;
}

static const char *dcc_new_app_message_event_macro(const char *event_name) {
    if (strcmp(event_name, "message_update") == 0) {
        return "DCC_MESSAGE_UPDATE_DATA";
    }
    if (strcmp(event_name, "message_delete") == 0) {
        return "DCC_MESSAGE_DELETE_EVENT_DATA";
    }
    return "DCC_MESSAGE_CREATE_DATA";
}

static int dcc_new_app_insert_event_handler(
    const char *path,
    const dcc_new_app_options_t *options,
    char *handler_name,
    size_t handler_name_len
) {
    char insertion[1200];
    snprintf(handler_name, handler_name_len, "on_%s_%s", options->cog_name, options->command_name);
    if (strcmp(options->command_name, "ready") == 0) {
        snprintf(
            insertion,
            sizeof(insertion),
            "DCC_READY_FN(%s) {\n"
            "    (void)app;\n"
            "    (void)ready;\n"
            "    (void)user_data;\n"
            "}\n"
            "\n",
            handler_name
        );
    } else if (dcc_new_app_event_is_message(options->command_name)) {
        snprintf(
            insertion,
            sizeof(insertion),
            "DCC_MESSAGE_FN(%s) {\n"
            "    (void)app;\n"
            "    (void)message;\n"
            "    (void)event;\n"
            "    (void)user_data;\n"
            "}\n"
            "\n",
            handler_name
        );
    } else {
        const dcc_new_app_typed_event_template_t *typed =
            dcc_new_app_typed_event_template(options->command_name);
        if (typed != NULL) {
            snprintf(
                insertion,
                sizeof(insertion),
                "%s(%s) {\n"
                "    (void)app;\n"
                "    (void)%s;\n"
                "    (void)event;\n"
                "    (void)user_data;\n"
                "}\n"
                "\n",
                typed->handler_macro,
                handler_name,
                typed->payload_name
            );
            return dcc_new_app_insert_before_marker(
                path,
                DCC_NEW_APP_HANDLER_MARKER,
                insertion,
                handler_name
            );
        }
        snprintf(
            insertion,
            sizeof(insertion),
            "DCC_EVENT_FN(%s) {\n"
            "    (void)app;\n"
            "    (void)event;\n"
            "    (void)user_data;\n"
            "}\n"
            "\n",
            handler_name
        );
    }
    return dcc_new_app_insert_before_marker(
        path,
        DCC_NEW_APP_HANDLER_MARKER,
        insertion,
        handler_name
    );
}

static int dcc_new_app_insert_event_route(
    const char *path,
    const dcc_new_app_options_t *options,
    const char *handler_name
) {
    char route[700];
    char duplicate_needle[360];
    if (strcmp(options->command_name, "ready") == 0) {
        snprintf(duplicate_needle, sizeof(duplicate_needle), "DCC_READY_DATA(%s", handler_name);
        snprintf(route, sizeof(route), "        DCC_READY_DATA(%s, user_data)\n", handler_name);
    } else if (dcc_new_app_event_is_message(options->command_name)) {
        const char *macro = dcc_new_app_message_event_macro(options->command_name);
        snprintf(duplicate_needle, sizeof(duplicate_needle), "%s(%s", macro, handler_name);
        snprintf(route, sizeof(route), "        %s(%s, user_data)\n", macro, handler_name);
    } else {
        const dcc_new_app_typed_event_template_t *typed =
            dcc_new_app_typed_event_template(options->command_name);
        if (typed != NULL) {
            snprintf(duplicate_needle, sizeof(duplicate_needle), "%s(%s", typed->route_macro, handler_name);
            snprintf(route, sizeof(route), "        %s(%s, user_data)\n", typed->route_macro, handler_name);
            return dcc_new_app_insert_event_route_text(path, route, duplicate_needle);
        }
        char event_enum[256];
        dcc_new_app_event_enum_name(options->command_name, event_enum, sizeof(event_enum));
        snprintf(
            duplicate_needle,
            sizeof(duplicate_needle),
            "DCC_EVENT_DATA(%s, %s",
            event_enum,
            handler_name
        );
        snprintf(
            route,
            sizeof(route),
            "        DCC_EVENT_DATA(%s, %s, user_data)\n",
            event_enum,
            handler_name
        );
    }
    return dcc_new_app_insert_event_route_text(path, route, duplicate_needle);
}

int dcc_new_app_generate_event(const dcc_new_app_options_t *options) {
    char *path = NULL;
    if (dcc_new_app_event_require_cog_source(&path, options) != 0) {
        return -1;
    }
    char handler_name[320];
    int status = dcc_new_app_insert_event_handler(path, options, handler_name, sizeof(handler_name));
    if (status == 0) {
        status = dcc_new_app_insert_event_route(path, options, handler_name);
    }
    free(path);
    if (status != 0) {
        return -1;
    }
    printf("created DCC event %s in feature %s under %s\n", options->command_name, options->cog_name, options->path);
    return 0;
}
