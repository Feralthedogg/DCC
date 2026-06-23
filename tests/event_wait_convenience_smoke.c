#include <dcc/dcc.h>

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include "support/event_wait_convenience_smoke_support.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int event_wait_convenience_dispatch_ready_burst(dcc_client_t *client) {
    for (unsigned i = 0; i < 25U; ++i) {
        if (event_wait_convenience_dispatch_gateway_ready(client) != 0) {
            return -1;
        }
        usleep(2000);
    }
    return 0;
}

static int event_wait_convenience_dispatch_slash_burst(dcc_client_t *client) {
    for (unsigned i = 0; i < 25U; ++i) {
        if (event_wait_convenience_dispatch_slash_command(client) != 0) {
            return -1;
        }
        usleep(2000);
    }
    return 0;
}

static int event_wait_convenience_dispatch_close_burst(dcc_client_t *client) {
    for (unsigned i = 0; i < 25U; ++i) {
        if (event_wait_convenience_dispatch_socket_close(client) != 0) {
            return -1;
        }
        usleep(2000);
    }
    return 0;
}

int main(void) {
    dcc_client_t *client = NULL;
    dcc_client_options_t options = {
        .size = sizeof(options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_status_t status = dcc_client_create(&options, &client);
    if (status != DCC_OK) {
        fprintf(stderr, "client create failed: %s\n", dcc_status_string(status));
        return 1;
    }

    if (event_wait_convenience_check_invalid_contracts(client) != 0) {
        dcc_client_destroy(client);
        return 1;
    }

    status = dcc_client_start(client);
    if (status != DCC_OK) {
        fprintf(stderr, "client start failed: %s\n", dcc_status_string(status));
        dcc_client_destroy(client);
        return 1;
    }

    dcc_event_wait_thread_state_t ready_wait;
    memset(&ready_wait, 0, sizeof(ready_wait));
    ready_wait.client = client;
    ready_wait.status = DCC_ERR_RUNTIME;
    dcc_event_wait_thread_state_t ready_or_close_wait;
    memset(&ready_or_close_wait, 0, sizeof(ready_or_close_wait));
    ready_or_close_wait.client = client;
    ready_or_close_wait.status = DCC_ERR_RUNTIME;
    dcc_event_wait_thread_state_t admission_ready_wait;
    memset(&admission_ready_wait, 0, sizeof(admission_ready_wait));
    admission_ready_wait.client = client;
    admission_ready_wait.status = DCC_ERR_RUNTIME;

    pthread_t ready_thread;
    if (pthread_create(
            &ready_thread,
            NULL,
            event_wait_convenience_gateway_ready_wait_thread_main,
            &ready_wait
        ) != 0) {
        fprintf(stderr, "ready wait thread failed: %s\n", strerror(errno));
        (void)dcc_client_stop(client);
        dcc_client_destroy(client);
        return 1;
    }
    pthread_t ready_or_close_thread;
    if (pthread_create(
            &ready_or_close_thread,
            NULL,
            event_wait_convenience_gateway_ready_or_close_wait_thread_main,
            &ready_or_close_wait
        ) != 0) {
        fprintf(stderr, "ready-or-close wait thread failed: %s\n", strerror(errno));
        (void)dcc_client_stop(client);
        (void)pthread_join(ready_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }
    pthread_t admission_ready_thread;
    if (pthread_create(
            &admission_ready_thread,
            NULL,
            event_wait_convenience_gateway_admission_wait_thread_main,
            &admission_ready_wait
        ) != 0) {
        fprintf(stderr, "gateway admission wait thread failed: %s\n", strerror(errno));
        (void)dcc_client_stop(client);
        (void)pthread_join(ready_thread, NULL);
        (void)pthread_join(ready_or_close_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }

    if (event_wait_convenience_dispatch_ready_burst(client) != 0) {
        fprintf(stderr, "gateway ready dispatch failed\n");
        (void)dcc_client_stop(client);
        (void)pthread_join(ready_thread, NULL);
        (void)pthread_join(ready_or_close_thread, NULL);
        (void)pthread_join(admission_ready_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }

    dcc_client_wait_thread_state_t wait_state = {
        .client = client,
        .status = DCC_ERR_RUNTIME,
    };
    pthread_t wait_thread;
    if (pthread_create(&wait_thread, NULL, event_wait_convenience_client_wait_thread_main, &wait_state) != 0) {
        fprintf(stderr, "client wait thread failed: %s\n", strerror(errno));
        (void)dcc_client_stop(client);
        (void)pthread_join(ready_thread, NULL);
        (void)pthread_join(ready_or_close_thread, NULL);
        (void)pthread_join(admission_ready_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }

    (void)pthread_join(ready_thread, NULL);
    (void)pthread_join(ready_or_close_thread, NULL);
    (void)pthread_join(admission_ready_thread, NULL);
    if (ready_wait.status != DCC_OK ||
        ready_wait.snapshot.type != DCC_EVENT_READY ||
        strcmp(ready_wait.snapshot.name, "READY") != 0 ||
        strcmp(ready_wait.snapshot.raw_name, "READY") != 0 ||
        ready_or_close_wait.status != DCC_OK ||
        ready_or_close_wait.snapshot.type != DCC_EVENT_READY ||
        strcmp(ready_or_close_wait.snapshot.name, "READY") != 0 ||
        strcmp(ready_or_close_wait.snapshot.raw_name, "READY") != 0 ||
        admission_ready_wait.status != DCC_OK ||
        admission_ready_wait.snapshot.type != DCC_EVENT_READY ||
        strcmp(admission_ready_wait.snapshot.name, "READY") != 0 ||
        strcmp(admission_ready_wait.snapshot.raw_name, "READY") != 0) {
        fprintf(
            stderr,
            "gateway ready wait failed: status=%s close_status=%s admission_status=%s "
            "type=%d close_type=%d admission_type=%d name=%s close_name=%s admission_name=%s\n",
            dcc_status_string(ready_wait.status),
            dcc_status_string(ready_or_close_wait.status),
            dcc_status_string(admission_ready_wait.status),
            (int)ready_wait.snapshot.type,
            (int)ready_or_close_wait.snapshot.type,
            (int)admission_ready_wait.snapshot.type,
            ready_wait.snapshot.name,
            ready_or_close_wait.snapshot.name,
            admission_ready_wait.snapshot.name
        );
        (void)dcc_client_stop(client);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }

    dcc_event_wait_thread_state_t interaction_or_close_wait;
    memset(&interaction_or_close_wait, 0, sizeof(interaction_or_close_wait));
    interaction_or_close_wait.client = client;
    interaction_or_close_wait.status = DCC_ERR_RUNTIME;
    pthread_t interaction_thread;
    if (pthread_create(
            &interaction_thread,
            NULL,
            event_wait_convenience_interaction_or_close_wait_thread_main,
            &interaction_or_close_wait
        ) != 0) {
        fprintf(stderr, "interaction-or-close wait thread failed: %s\n", strerror(errno));
        (void)dcc_client_stop(client);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }
    if (event_wait_convenience_dispatch_slash_burst(client) != 0) {
        fprintf(stderr, "slash command dispatch failed\n");
        (void)dcc_client_stop(client);
        (void)pthread_join(interaction_thread, NULL);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }
    (void)pthread_join(interaction_thread, NULL);
    if (interaction_or_close_wait.status != DCC_OK ||
        interaction_or_close_wait.snapshot.type != DCC_EVENT_SLASH_COMMAND ||
        interaction_or_close_wait.snapshot.raw_type != DCC_EVENT_INTERACTION_CREATE ||
        interaction_or_close_wait.snapshot.raw_sequence != 42 ||
        strcmp(interaction_or_close_wait.snapshot.name, "INTERACTION_CREATE") != 0 ||
        strcmp(interaction_or_close_wait.snapshot.raw_name, "INTERACTION_CREATE") != 0) {
        fprintf(
            stderr,
            "interaction-or-close wait failed: status=%s type=%d raw=%d seq=%llu name=%s raw_name=%s\n",
            dcc_status_string(interaction_or_close_wait.status),
            (int)interaction_or_close_wait.snapshot.type,
            (int)interaction_or_close_wait.snapshot.raw_type,
            (unsigned long long)interaction_or_close_wait.snapshot.raw_sequence,
            interaction_or_close_wait.snapshot.name,
            interaction_or_close_wait.snapshot.raw_name
        );
        (void)dcc_client_stop(client);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }

    dcc_event_wait_thread_state_t interaction_admission_wait;
    memset(&interaction_admission_wait, 0, sizeof(interaction_admission_wait));
    interaction_admission_wait.client = client;
    interaction_admission_wait.status = DCC_ERR_RUNTIME;
    if (pthread_create(
            &interaction_thread,
            NULL,
            event_wait_convenience_interaction_admission_wait_thread_main,
            &interaction_admission_wait
        ) != 0) {
        fprintf(stderr, "interaction admission wait thread failed: %s\n", strerror(errno));
        (void)dcc_client_stop(client);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }
    if (event_wait_convenience_dispatch_slash_burst(client) != 0) {
        fprintf(stderr, "slash command admission dispatch failed\n");
        (void)dcc_client_stop(client);
        (void)pthread_join(interaction_thread, NULL);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }
    (void)pthread_join(interaction_thread, NULL);
    if (interaction_admission_wait.status != DCC_OK ||
        interaction_admission_wait.snapshot.type != DCC_EVENT_SLASH_COMMAND ||
        interaction_admission_wait.snapshot.raw_type != DCC_EVENT_INTERACTION_CREATE ||
        interaction_admission_wait.snapshot.raw_sequence != 42 ||
        strcmp(interaction_admission_wait.snapshot.name, "INTERACTION_CREATE") != 0 ||
        strcmp(interaction_admission_wait.snapshot.raw_name, "INTERACTION_CREATE") != 0) {
        fprintf(
            stderr,
            "interaction admission wait failed: status=%s type=%d raw=%d seq=%llu name=%s raw_name=%s\n",
            dcc_status_string(interaction_admission_wait.status),
            (int)interaction_admission_wait.snapshot.type,
            (int)interaction_admission_wait.snapshot.raw_type,
            (unsigned long long)interaction_admission_wait.snapshot.raw_sequence,
            interaction_admission_wait.snapshot.name,
            interaction_admission_wait.snapshot.raw_name
        );
        (void)dcc_client_stop(client);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }

    dcc_event_wait_thread_state_t interaction_owned_wait;
    memset(&interaction_owned_wait, 0, sizeof(interaction_owned_wait));
    interaction_owned_wait.client = client;
    interaction_owned_wait.status = DCC_ERR_RUNTIME;
    if (pthread_create(
            &interaction_thread,
            NULL,
            event_wait_convenience_interaction_or_close_owned_wait_thread_main,
            &interaction_owned_wait
        ) != 0) {
        fprintf(stderr, "interaction-or-close owned wait thread failed: %s\n", strerror(errno));
        (void)dcc_client_stop(client);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }
    if (event_wait_convenience_dispatch_slash_burst(client) != 0) {
        fprintf(stderr, "slash command owned dispatch failed\n");
        (void)dcc_client_stop(client);
        (void)pthread_join(interaction_thread, NULL);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }
    (void)pthread_join(interaction_thread, NULL);
    if (interaction_owned_wait.status != DCC_OK ||
        interaction_owned_wait.snapshot.type != DCC_EVENT_SLASH_COMMAND ||
        interaction_owned_wait.interaction == NULL ||
        interaction_owned_wait.interaction->name == NULL ||
        strcmp(interaction_owned_wait.interaction->name, "wait-helper") != 0) {
        fprintf(
            stderr,
            "interaction-or-close owned wait failed: status=%s type=%d interaction=%p name=%s\n",
            dcc_status_string(interaction_owned_wait.status),
            (int)interaction_owned_wait.snapshot.type,
            (void *)interaction_owned_wait.interaction,
            interaction_owned_wait.interaction != NULL && interaction_owned_wait.interaction->name != NULL
                ? interaction_owned_wait.interaction->name
                : ""
        );
        dcc_interaction_free(interaction_owned_wait.interaction);
        (void)dcc_client_stop(client);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }
    dcc_interaction_free(interaction_owned_wait.interaction);

    dcc_event_wait_thread_state_t interaction_admission_owned_wait;
    memset(&interaction_admission_owned_wait, 0, sizeof(interaction_admission_owned_wait));
    interaction_admission_owned_wait.client = client;
    interaction_admission_owned_wait.status = DCC_ERR_RUNTIME;
    if (pthread_create(
            &interaction_thread,
            NULL,
            event_wait_convenience_interaction_admission_owned_wait_thread_main,
            &interaction_admission_owned_wait
        ) != 0) {
        fprintf(stderr, "interaction admission owned wait thread failed: %s\n", strerror(errno));
        (void)dcc_client_stop(client);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }
    if (event_wait_convenience_dispatch_slash_burst(client) != 0) {
        fprintf(stderr, "slash command admission owned dispatch failed\n");
        (void)dcc_client_stop(client);
        (void)pthread_join(interaction_thread, NULL);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }
    (void)pthread_join(interaction_thread, NULL);
    if (interaction_admission_owned_wait.status != DCC_OK ||
        interaction_admission_owned_wait.snapshot.type != DCC_EVENT_SLASH_COMMAND ||
        interaction_admission_owned_wait.interaction == NULL ||
        interaction_admission_owned_wait.interaction->name == NULL ||
        strcmp(interaction_admission_owned_wait.interaction->name, "wait-helper") != 0) {
        fprintf(
            stderr,
            "interaction admission owned wait failed: status=%s type=%d interaction=%p name=%s\n",
            dcc_status_string(interaction_admission_owned_wait.status),
            (int)interaction_admission_owned_wait.snapshot.type,
            (void *)interaction_admission_owned_wait.interaction,
            interaction_admission_owned_wait.interaction != NULL &&
                interaction_admission_owned_wait.interaction->name != NULL
                ? interaction_admission_owned_wait.interaction->name
                : ""
        );
        dcc_interaction_free(interaction_admission_owned_wait.interaction);
        (void)dcc_client_stop(client);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }
    dcc_interaction_free(interaction_admission_owned_wait.interaction);

    dcc_event_wait_thread_state_t close_owned_wait;
    memset(&close_owned_wait, 0, sizeof(close_owned_wait));
    close_owned_wait.client = client;
    close_owned_wait.status = DCC_ERR_RUNTIME;
    dcc_event_wait_thread_state_t admission_close_wait;
    memset(&admission_close_wait, 0, sizeof(admission_close_wait));
    admission_close_wait.client = client;
    admission_close_wait.status = DCC_ERR_RUNTIME;
    dcc_event_wait_thread_state_t interaction_admission_close_wait;
    memset(&interaction_admission_close_wait, 0, sizeof(interaction_admission_close_wait));
    interaction_admission_close_wait.client = client;
    interaction_admission_close_wait.status = DCC_ERR_RUNTIME;
    dcc_event_wait_thread_state_t interaction_admission_owned_close_wait;
    memset(&interaction_admission_owned_close_wait, 0, sizeof(interaction_admission_owned_close_wait));
    interaction_admission_owned_close_wait.client = client;
    interaction_admission_owned_close_wait.status = DCC_ERR_RUNTIME;
    if (pthread_create(
            &interaction_thread,
            NULL,
            event_wait_convenience_interaction_or_close_owned_wait_thread_main,
            &close_owned_wait
        ) != 0) {
        fprintf(stderr, "interaction-or-close close wait thread failed: %s\n", strerror(errno));
        (void)dcc_client_stop(client);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }
    pthread_t admission_close_thread;
    if (pthread_create(
            &admission_close_thread,
            NULL,
            event_wait_convenience_gateway_admission_wait_thread_main,
            &admission_close_wait
        ) != 0) {
        fprintf(stderr, "gateway admission close wait thread failed: %s\n", strerror(errno));
        (void)dcc_client_stop(client);
        (void)pthread_join(interaction_thread, NULL);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }
    pthread_t interaction_admission_close_thread;
    if (pthread_create(
            &interaction_admission_close_thread,
            NULL,
            event_wait_convenience_interaction_admission_wait_thread_main,
            &interaction_admission_close_wait
        ) != 0) {
        fprintf(stderr, "interaction admission close wait thread failed: %s\n", strerror(errno));
        (void)dcc_client_stop(client);
        (void)pthread_join(interaction_thread, NULL);
        (void)pthread_join(admission_close_thread, NULL);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }
    pthread_t interaction_admission_owned_close_thread;
    if (pthread_create(
            &interaction_admission_owned_close_thread,
            NULL,
            event_wait_convenience_interaction_admission_owned_wait_thread_main,
            &interaction_admission_owned_close_wait
        ) != 0) {
        fprintf(stderr, "interaction admission owned close wait thread failed: %s\n", strerror(errno));
        (void)dcc_client_stop(client);
        (void)pthread_join(interaction_thread, NULL);
        (void)pthread_join(admission_close_thread, NULL);
        (void)pthread_join(interaction_admission_close_thread, NULL);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }
    if (event_wait_convenience_dispatch_close_burst(client) != 0) {
        fprintf(stderr, "socket close dispatch failed\n");
        (void)dcc_client_stop(client);
        (void)pthread_join(interaction_thread, NULL);
        (void)pthread_join(admission_close_thread, NULL);
        (void)pthread_join(interaction_admission_close_thread, NULL);
        (void)pthread_join(interaction_admission_owned_close_thread, NULL);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }
    (void)pthread_join(interaction_thread, NULL);
    (void)pthread_join(admission_close_thread, NULL);
    (void)pthread_join(interaction_admission_close_thread, NULL);
    (void)pthread_join(interaction_admission_owned_close_thread, NULL);
    if (close_owned_wait.status != DCC_OK ||
        close_owned_wait.snapshot.type != DCC_EVENT_SOCKET_CLOSE ||
        close_owned_wait.snapshot.raw_sequence != 43 ||
        close_owned_wait.interaction != NULL ||
        admission_close_wait.status != DCC_ERR_STATE ||
        admission_close_wait.snapshot.type != DCC_EVENT_SOCKET_CLOSE ||
        admission_close_wait.snapshot.raw_sequence != 43 ||
        interaction_admission_close_wait.status != DCC_ERR_STATE ||
        interaction_admission_close_wait.snapshot.type != DCC_EVENT_SOCKET_CLOSE ||
        interaction_admission_close_wait.snapshot.raw_sequence != 43 ||
        interaction_admission_owned_close_wait.status != DCC_ERR_STATE ||
        interaction_admission_owned_close_wait.snapshot.type != DCC_EVENT_SOCKET_CLOSE ||
        interaction_admission_owned_close_wait.snapshot.raw_sequence != 43 ||
        interaction_admission_owned_close_wait.interaction != NULL) {
        fprintf(
            stderr,
            "interaction-or-close close wait failed: status=%s type=%d seq=%llu interaction=%p "
            "admission_status=%s admission_type=%d admission_seq=%llu "
            "interaction_admission_status=%s interaction_admission_type=%d interaction_admission_seq=%llu "
            "interaction_admission_owned_status=%s interaction_admission_owned_type=%d "
            "interaction_admission_owned_seq=%llu interaction_admission_owned=%p\n",
            dcc_status_string(close_owned_wait.status),
            (int)close_owned_wait.snapshot.type,
            (unsigned long long)close_owned_wait.snapshot.raw_sequence,
            (void *)close_owned_wait.interaction,
            dcc_status_string(admission_close_wait.status),
            (int)admission_close_wait.snapshot.type,
            (unsigned long long)admission_close_wait.snapshot.raw_sequence,
            dcc_status_string(interaction_admission_close_wait.status),
            (int)interaction_admission_close_wait.snapshot.type,
            (unsigned long long)interaction_admission_close_wait.snapshot.raw_sequence,
            dcc_status_string(interaction_admission_owned_close_wait.status),
            (int)interaction_admission_owned_close_wait.snapshot.type,
            (unsigned long long)interaction_admission_owned_close_wait.snapshot.raw_sequence,
            (void *)interaction_admission_owned_close_wait.interaction
        );
        dcc_interaction_free(close_owned_wait.interaction);
        dcc_interaction_free(interaction_admission_owned_close_wait.interaction);
        (void)dcc_client_stop(client);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }

    (void)dcc_client_stop(client);
    (void)pthread_join(wait_thread, NULL);
    dcc_client_destroy(client);
    return wait_state.status == DCC_OK ? 0 : 1;
}
#endif
