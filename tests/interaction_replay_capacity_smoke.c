#include "internal/interactions/dcc_interaction_server_internal.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

int main(void) {
    time_t now = time(NULL);
    if (now < (time_t)0) {
        fprintf(stderr, "failed to read wall clock\n");
        return 1;
    }
    char timestamp[32];
    int timestamp_len = snprintf(
        timestamp, sizeof(timestamp), "%llu", (unsigned long long)now
    );
    if (timestamp_len < 0 || (size_t)timestamp_len >= sizeof(timestamp)) {
        fprintf(stderr, "failed to format timestamp\n");
        return 1;
    }

    dcc_interaction_server_options_t options = {
        .size = sizeof(options),
        .public_key_hex =
            "0000000000000000000000000000000000000000000000000000000000000000",
    };
    dcc_interaction_server_t *server = NULL;
    if (dcc_interaction_server_create(&options, &server) != DCC_OK ||
        server == NULL) {
        fprintf(stderr, "failed to create interaction server\n");
        return 1;
    }

    if (!dcc_interaction_server_timestamp_fresh(server, timestamp) ||
        dcc_interaction_server_timestamp_fresh(server, "1700000000") ||
        dcc_interaction_server_timestamp_fresh(server, " 1700000000") ||
        dcc_interaction_server_timestamp_fresh(server, "+1700000000")) {
        fprintf(stderr, "timestamp freshness validation failed\n");
        dcc_interaction_server_destroy(server);
        return 1;
    }

    char signature[129];
    memset(signature, '0', sizeof(signature) - 1U);
    signature[sizeof(signature) - 1U] = '\0';
    for (size_t i = 0U; i < DCC_INTERACTION_REPLAY_CAP; ++i) {
        (void)snprintf(
            signature + 120U, sizeof(signature) - 120U, "%08llx",
            (unsigned long long)i
        );
        if (dcc_interaction_server_replay_seen(server, timestamp, signature) !=
            DCC_INTERACTION_REPLAY_NEW) {
            fprintf(stderr, "replay set rejected entry %zu\n", i);
            dcc_interaction_server_destroy(server);
            return 1;
        }
    }
    char new_signature[129];
    memset(new_signature, '0', sizeof(new_signature) - 1U);
    new_signature[119] = '1';
    new_signature[sizeof(new_signature) - 1U] = '\0';
    if (dcc_interaction_server_replay_seen(server, timestamp, signature) !=
            DCC_INTERACTION_REPLAY_SEEN ||
        dcc_interaction_server_replay_seen(
            server, timestamp, new_signature
        ) != DCC_INTERACTION_REPLAY_CAPACITY) {
        fprintf(stderr, "replay capacity policy failed\n");
        dcc_interaction_server_destroy(server);
        return 1;
    }
    dcc_interaction_server_destroy(server);
    return 0;
}
