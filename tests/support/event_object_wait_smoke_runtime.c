#include "event_object_wait_smoke_support.h"

#if !defined(_WIN32)

#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct object_wait_runtime {
    message_wait_state_t message_wait;
    data_wait_state_t data_wait;
    message_wait_state_t message_any_wait;
    data_wait_state_t data_any_wait;
    member_wait_state_t member_wait;
    role_wait_state_t role_wait;
    user_wait_state_t user_wait;
    user_wait_state_t user_any_wait;
    any_wait_state_t any_wait;
    sequence_wait_state_t sequence_wait;
    set_wait_state_t set_wait;
    count_wait_state_t count_wait;
    quorum_wait_state_t quorum_wait;
    client_wait_state_t wait_state;
} object_wait_runtime_t;

typedef struct object_wait_thread_group {
    pthread_t message_thread;
    pthread_t data_thread;
    pthread_t message_any_thread;
    pthread_t data_any_thread;
    pthread_t member_thread;
    pthread_t role_thread;
    pthread_t user_thread;
    pthread_t user_any_thread;
    pthread_t any_thread;
    pthread_t sequence_thread;
    pthread_t set_thread;
    pthread_t count_thread;
    pthread_t quorum_thread;
    int message_thread_started;
    int data_thread_started;
    int message_any_thread_started;
    int data_any_thread_started;
    int member_thread_started;
    int role_thread_started;
    int user_thread_started;
    int user_any_thread_started;
    int any_thread_started;
    int sequence_thread_started;
    int set_thread_started;
    int count_thread_started;
    int quorum_thread_started;
} object_wait_thread_group_t;

typedef struct object_wait_thread_spec {
    pthread_t *thread;
    int *started;
    void *(*main_fn)(void *);
    void *arg;
} object_wait_thread_spec_t;

static void object_wait_runtime_init(object_wait_runtime_t *runtime, dcc_client_t *client) {
    memset(runtime, 0, sizeof(*runtime));
    runtime->message_wait.client = client;
    runtime->message_wait.status = DCC_ERR_RUNTIME;
    runtime->data_wait.client = client;
    runtime->data_wait.status = DCC_ERR_RUNTIME;
    runtime->message_any_wait.client = client;
    runtime->message_any_wait.status = DCC_ERR_RUNTIME;
    runtime->message_any_wait.types[0] = DCC_EVENT_MESSAGE_UPDATE;
    runtime->message_any_wait.types[1] = DCC_EVENT_MESSAGE_CREATE;
    runtime->message_any_wait.type_count = 2U;
    runtime->data_any_wait.client = client;
    runtime->data_any_wait.status = DCC_ERR_RUNTIME;
    runtime->data_any_wait.types[0] = DCC_EVENT_MESSAGE_UPDATE;
    runtime->data_any_wait.types[1] = DCC_EVENT_MESSAGE_CREATE;
    runtime->data_any_wait.type_count = 2U;
    runtime->member_wait.client = client;
    runtime->member_wait.status = DCC_ERR_RUNTIME;
    runtime->role_wait.client = client;
    runtime->role_wait.status = DCC_ERR_RUNTIME;
    runtime->user_wait.client = client;
    runtime->user_wait.status = DCC_ERR_RUNTIME;
    runtime->user_any_wait.client = client;
    runtime->user_any_wait.status = DCC_ERR_RUNTIME;
    runtime->user_any_wait.types[0] = DCC_EVENT_RATE_LIMITED;
    runtime->user_any_wait.types[1] = DCC_EVENT_USER_UPDATE;
    runtime->user_any_wait.type_count = 2U;
    runtime->any_wait.client = client;
    runtime->any_wait.status = DCC_ERR_RUNTIME;
    runtime->any_wait.types[0] = DCC_EVENT_RATE_LIMITED;
    runtime->any_wait.types[1] = DCC_EVENT_USER_UPDATE;
    runtime->any_wait.type_count = 2U;
    runtime->sequence_wait.client = client;
    runtime->sequence_wait.status = DCC_ERR_RUNTIME;
    runtime->sequence_wait.types[0] = DCC_EVENT_MESSAGE_CREATE;
    runtime->sequence_wait.types[1] = DCC_EVENT_GUILD_MEMBER_ADD;
    runtime->sequence_wait.types[2] = DCC_EVENT_GUILD_ROLE_CREATE;
    runtime->sequence_wait.types[3] = DCC_EVENT_USER_UPDATE;
    runtime->sequence_wait.type_count = 4U;
    runtime->set_wait.client = client;
    runtime->set_wait.status = DCC_ERR_RUNTIME;
    runtime->set_wait.types[0] = DCC_EVENT_USER_UPDATE;
    runtime->set_wait.types[1] = DCC_EVENT_GUILD_ROLE_CREATE;
    runtime->set_wait.types[2] = DCC_EVENT_GUILD_MEMBER_ADD;
    runtime->set_wait.types[3] = DCC_EVENT_MESSAGE_CREATE;
    runtime->set_wait.type_count = 4U;
    runtime->count_wait.client = client;
    runtime->count_wait.status = DCC_ERR_RUNTIME;
    runtime->count_wait.types[0] = DCC_EVENT_MESSAGE_CREATE;
    runtime->count_wait.types[1] = DCC_EVENT_USER_UPDATE;
    runtime->count_wait.type_count = 2U;
    runtime->count_wait.target_count = 2U;
    runtime->quorum_wait.client = client;
    runtime->quorum_wait.status = DCC_ERR_RUNTIME;
    runtime->quorum_wait.types[0] = DCC_EVENT_MESSAGE_CREATE;
    runtime->quorum_wait.types[1] = DCC_EVENT_GUILD_MEMBER_ADD;
    runtime->quorum_wait.types[2] = DCC_EVENT_GUILD_ROLE_CREATE;
    runtime->quorum_wait.types[3] = DCC_EVENT_USER_UPDATE;
    runtime->quorum_wait.type_count = 4U;
    runtime->quorum_wait.target_count = 3U;
    runtime->wait_state.client = client;
    runtime->wait_state.status = DCC_ERR_RUNTIME;
}

static void object_wait_thread_specs(
    object_wait_thread_group_t *threads,
    object_wait_runtime_t *runtime,
    object_wait_thread_spec_t specs[13]
) {
    specs[0] = (object_wait_thread_spec_t){
        &threads->message_thread,
        &threads->message_thread_started,
        message_wait_thread_main,
        &runtime->message_wait
    };
    specs[1] = (object_wait_thread_spec_t){
        &threads->data_thread,
        &threads->data_thread_started,
        data_wait_thread_main,
        &runtime->data_wait
    };
    specs[2] = (object_wait_thread_spec_t){
        &threads->message_any_thread,
        &threads->message_any_thread_started,
        message_any_wait_thread_main,
        &runtime->message_any_wait
    };
    specs[3] = (object_wait_thread_spec_t){
        &threads->data_any_thread,
        &threads->data_any_thread_started,
        data_any_wait_thread_main,
        &runtime->data_any_wait
    };
    specs[4] = (object_wait_thread_spec_t){
        &threads->member_thread,
        &threads->member_thread_started,
        member_wait_thread_main,
        &runtime->member_wait
    };
    specs[5] = (object_wait_thread_spec_t){
        &threads->role_thread,
        &threads->role_thread_started,
        role_wait_thread_main,
        &runtime->role_wait
    };
    specs[6] = (object_wait_thread_spec_t){
        &threads->user_thread,
        &threads->user_thread_started,
        user_wait_thread_main,
        &runtime->user_wait
    };
    specs[7] = (object_wait_thread_spec_t){
        &threads->user_any_thread,
        &threads->user_any_thread_started,
        user_any_wait_thread_main,
        &runtime->user_any_wait
    };
    specs[8] = (object_wait_thread_spec_t){
        &threads->any_thread,
        &threads->any_thread_started,
        any_wait_thread_main,
        &runtime->any_wait
    };
    specs[9] = (object_wait_thread_spec_t){
        &threads->sequence_thread,
        &threads->sequence_thread_started,
        sequence_wait_thread_main,
        &runtime->sequence_wait
    };
    specs[10] = (object_wait_thread_spec_t){
        &threads->set_thread,
        &threads->set_thread_started,
        set_wait_thread_main,
        &runtime->set_wait
    };
    specs[11] = (object_wait_thread_spec_t){
        &threads->count_thread,
        &threads->count_thread_started,
        count_wait_thread_main,
        &runtime->count_wait
    };
    specs[12] = (object_wait_thread_spec_t){
        &threads->quorum_thread,
        &threads->quorum_thread_started,
        quorum_wait_thread_main,
        &runtime->quorum_wait
    };
}

static int object_wait_start_threads(object_wait_thread_group_t *threads, object_wait_runtime_t *runtime) {
    object_wait_thread_spec_t specs[13];
    object_wait_thread_specs(threads, runtime, specs);
    for (size_t i = 0; i < sizeof(specs) / sizeof(specs[0]); ++i) {
        if (pthread_create(specs[i].thread, NULL, specs[i].main_fn, specs[i].arg) != 0) {
            return 1;
        }
        *specs[i].started = 1;
    }
    return 0;
}

static void object_wait_join_threads(object_wait_thread_group_t *threads, object_wait_runtime_t *runtime) {
    object_wait_thread_spec_t specs[13];
    object_wait_thread_specs(threads, runtime, specs);
    for (size_t i = 0; i < sizeof(specs) / sizeof(specs[0]); ++i) {
        if (*specs[i].started) {
            (void)pthread_join(*specs[i].thread, NULL);
            *specs[i].started = 0;
        }
    }
}

static void object_wait_free_results(object_wait_runtime_t *runtime) {
    dcc_message_free(runtime->message_wait.message);
    dcc_gateway_event_data_free(runtime->data_wait.data);
    dcc_message_free(runtime->message_any_wait.message);
    dcc_gateway_event_data_free(runtime->data_any_wait.data);
    dcc_member_free(runtime->member_wait.member);
    dcc_role_free(runtime->role_wait.role);
    dcc_user_free(runtime->user_wait.user);
    dcc_user_free(runtime->user_any_wait.user);
}

static int object_wait_validate_runtime(
    const object_wait_runtime_t *runtime,
    const object_wait_server_t *server,
    dcc_status_t start_status
) {
    const dcc_message_t *message = runtime->message_wait.message;
    const dcc_gateway_event_data_t *data = runtime->data_wait.data;
    const dcc_message_t *message_any = runtime->message_any_wait.message;
    const dcc_gateway_event_data_t *data_any = runtime->data_any_wait.data;
    const dcc_member_t *member = runtime->member_wait.member;
    const dcc_role_t *role = runtime->role_wait.role;
    const dcc_user_t *user = runtime->user_wait.user;
    const dcc_user_t *user_any = runtime->user_any_wait.user;
    int ok = start_status == DCC_OK &&
        runtime->wait_state.status == DCC_OK &&
        runtime->message_wait.status == DCC_OK &&
        runtime->data_wait.status == DCC_OK &&
        runtime->message_any_wait.status == DCC_OK &&
        runtime->data_any_wait.status == DCC_OK &&
        runtime->member_wait.status == DCC_OK &&
        runtime->role_wait.status == DCC_OK &&
        runtime->user_wait.status == DCC_OK &&
        runtime->user_any_wait.status == DCC_OK &&
        runtime->any_wait.status == DCC_OK &&
        runtime->sequence_wait.status == DCC_OK &&
        runtime->sequence_wait.completed == 4U &&
        runtime->set_wait.status == DCC_OK &&
        runtime->set_wait.completed == 4U &&
        runtime->count_wait.status == DCC_OK &&
        runtime->count_wait.completed == 2U &&
        runtime->quorum_wait.status == DCC_OK &&
        runtime->quorum_wait.completed == 3U &&
        message != NULL &&
        data != NULL &&
        message_any != NULL &&
        data_any != NULL &&
        member != NULL &&
        role != NULL &&
        user != NULL &&
        user_any != NULL &&
        runtime->message_wait.snapshot.type == DCC_EVENT_MESSAGE_CREATE &&
        runtime->data_wait.snapshot.type == DCC_EVENT_MESSAGE_CREATE &&
        runtime->message_any_wait.snapshot.type == DCC_EVENT_MESSAGE_CREATE &&
        runtime->data_any_wait.snapshot.type == DCC_EVENT_MESSAGE_CREATE &&
        runtime->member_wait.snapshot.type == DCC_EVENT_GUILD_MEMBER_ADD &&
        runtime->role_wait.snapshot.type == DCC_EVENT_GUILD_ROLE_CREATE &&
        runtime->user_wait.snapshot.type == DCC_EVENT_USER_UPDATE &&
        runtime->user_any_wait.snapshot.type == DCC_EVENT_USER_UPDATE &&
        runtime->any_wait.snapshot.type == DCC_EVENT_USER_UPDATE &&
        runtime->sequence_wait.snapshots[0].type == DCC_EVENT_MESSAGE_CREATE &&
        runtime->sequence_wait.snapshots[1].type == DCC_EVENT_GUILD_MEMBER_ADD &&
        runtime->sequence_wait.snapshots[2].type == DCC_EVENT_GUILD_ROLE_CREATE &&
        runtime->sequence_wait.snapshots[3].type == DCC_EVENT_USER_UPDATE &&
        runtime->set_wait.snapshots[0].type == DCC_EVENT_USER_UPDATE &&
        runtime->set_wait.snapshots[1].type == DCC_EVENT_GUILD_ROLE_CREATE &&
        runtime->set_wait.snapshots[2].type == DCC_EVENT_GUILD_MEMBER_ADD &&
        runtime->set_wait.snapshots[3].type == DCC_EVENT_MESSAGE_CREATE &&
        runtime->count_wait.snapshots[0].type == DCC_EVENT_MESSAGE_CREATE &&
        runtime->count_wait.snapshots[1].type == DCC_EVENT_USER_UPDATE &&
        runtime->quorum_wait.snapshots[0].type == DCC_EVENT_MESSAGE_CREATE &&
        runtime->quorum_wait.snapshots[1].type == DCC_EVENT_GUILD_MEMBER_ADD &&
        runtime->quorum_wait.snapshots[2].type == DCC_EVENT_GUILD_ROLE_CREATE &&
        strcmp(runtime->message_wait.snapshot.name, "MESSAGE_CREATE") == 0 &&
        strcmp(runtime->data_wait.snapshot.raw_name, "MESSAGE_CREATE") == 0 &&
        strcmp(runtime->message_any_wait.snapshot.name, "MESSAGE_CREATE") == 0 &&
        strcmp(runtime->data_any_wait.snapshot.raw_name, "MESSAGE_CREATE") == 0 &&
        strcmp(runtime->member_wait.snapshot.name, "GUILD_MEMBER_ADD") == 0 &&
        strcmp(runtime->role_wait.snapshot.name, "GUILD_ROLE_CREATE") == 0 &&
        strcmp(runtime->user_wait.snapshot.name, "USER_UPDATE") == 0 &&
        strcmp(runtime->user_any_wait.snapshot.name, "USER_UPDATE") == 0 &&
        strcmp(runtime->any_wait.snapshot.name, "USER_UPDATE") == 0 &&
        runtime->message_wait.snapshot.raw_sequence == 7 &&
        runtime->data_wait.snapshot.raw_sequence == 7 &&
        runtime->message_any_wait.snapshot.raw_sequence == 7 &&
        runtime->data_any_wait.snapshot.raw_sequence == 7 &&
        runtime->member_wait.snapshot.raw_sequence == 8 &&
        runtime->role_wait.snapshot.raw_sequence == 9 &&
        runtime->user_wait.snapshot.raw_sequence == 10 &&
        runtime->user_any_wait.snapshot.raw_sequence == 10 &&
        runtime->any_wait.snapshot.raw_sequence == 10 &&
        runtime->sequence_wait.snapshots[0].raw_sequence == 7 &&
        runtime->sequence_wait.snapshots[1].raw_sequence == 8 &&
        runtime->sequence_wait.snapshots[2].raw_sequence == 9 &&
        runtime->sequence_wait.snapshots[3].raw_sequence == 10 &&
        runtime->set_wait.snapshots[0].raw_sequence == 10 &&
        runtime->set_wait.snapshots[1].raw_sequence == 9 &&
        runtime->set_wait.snapshots[2].raw_sequence == 8 &&
        runtime->set_wait.snapshots[3].raw_sequence == 7 &&
        runtime->count_wait.snapshots[0].raw_sequence == 7 &&
        runtime->count_wait.snapshots[1].raw_sequence == 10 &&
        runtime->quorum_wait.snapshots[0].raw_sequence == 7 &&
        runtime->quorum_wait.snapshots[1].raw_sequence == 8 &&
        runtime->quorum_wait.snapshots[2].raw_sequence == 9 &&
        message->id == 111 &&
        message->channel_id == 222 &&
        message->guild_id == 333 &&
        message->author.id == 444 &&
        message->content != NULL &&
        strcmp(message->content, "waited object") == 0 &&
        message->author.username != NULL &&
        strcmp(message->author.username, "waiter") == 0 &&
        message_any->id == 111 &&
        message_any->content != NULL &&
        strcmp(message_any->content, "waited object") == 0 &&
        data->content != NULL &&
        strcmp(data->content, "waited object") == 0 &&
        data->timestamp_text != NULL &&
        strcmp(data->timestamp_text, "2026-06-16T00:00:00.000000+00:00") == 0 &&
        data_any->content != NULL &&
        strcmp(data_any->content, "waited object") == 0 &&
        member->guild_id == 333 &&
        member->user.id == 446 &&
        member->user.username != NULL &&
        strcmp(member->user.username, "member") == 0 &&
        member->user.global_name != NULL &&
        strcmp(member->user.global_name, "Member") == 0 &&
        role->id == 555 &&
        role->guild_id == 333 &&
        role->name != NULL &&
        strcmp(role->name, "mods") == 0 &&
        role->permissions == 1024 &&
        role->mentionable == 1 &&
        user->id == 447 &&
        user->username != NULL &&
        strcmp(user->username, "self") == 0 &&
        user->avatar != NULL &&
        strcmp(user->avatar, "user-avatar") == 0 &&
        user->flags == 64 &&
        user_any->id == 447 &&
        user_any->username != NULL &&
        strcmp(user_any->username, "self") == 0 &&
        user_any->flags == 64 &&
        atomic_load(&server->bad) == 0U &&
        atomic_load(&server->accepts) == 1U &&
        atomic_load(&server->identifies) == 1U;

    if (ok) {
        return 0;
    }

    fprintf(
        stderr,
        "event object wait failed: start=%s runtime=%s message=%s data=%s message_any=%s data_any=%s "
        "member=%s role=%s user=%s user_any=%s any=%s sequence=%s completed=%zu set=%s set_completed=%zu "
        "count=%s count_completed=%zu quorum=%s quorum_completed=%zu "
        "server_bad=%u accepts=%u identifies=%u "
        "msg=%p data=%p msg_any=%p data_any=%p member=%p role=%p user=%p user_any=%p "
        "msg_type=%d data_type=%d msg_any_type=%d data_any_type=%d member_type=%d role_type=%d "
        "user_type=%d user_any_type=%d any_type=%d msg_seq=%llu data_seq=%llu msg_any_seq=%llu "
        "data_any_seq=%llu member_seq=%llu role_seq=%llu user_seq=%llu user_any_seq=%llu any_seq=%llu\n",
        dcc_status_string(start_status),
        dcc_status_string(runtime->wait_state.status),
        dcc_status_string(runtime->message_wait.status),
        dcc_status_string(runtime->data_wait.status),
        dcc_status_string(runtime->message_any_wait.status),
        dcc_status_string(runtime->data_any_wait.status),
        dcc_status_string(runtime->member_wait.status),
        dcc_status_string(runtime->role_wait.status),
        dcc_status_string(runtime->user_wait.status),
        dcc_status_string(runtime->user_any_wait.status),
        dcc_status_string(runtime->any_wait.status),
        dcc_status_string(runtime->sequence_wait.status),
        runtime->sequence_wait.completed,
        dcc_status_string(runtime->set_wait.status),
        runtime->set_wait.completed,
        dcc_status_string(runtime->count_wait.status),
        runtime->count_wait.completed,
        dcc_status_string(runtime->quorum_wait.status),
        runtime->quorum_wait.completed,
        atomic_load(&server->bad),
        atomic_load(&server->accepts),
        atomic_load(&server->identifies),
        (void *)message,
        (void *)data,
        (void *)message_any,
        (void *)data_any,
        (void *)member,
        (void *)role,
        (void *)user,
        (void *)user_any,
        (int)runtime->message_wait.snapshot.type,
        (int)runtime->data_wait.snapshot.type,
        (int)runtime->message_any_wait.snapshot.type,
        (int)runtime->data_any_wait.snapshot.type,
        (int)runtime->member_wait.snapshot.type,
        (int)runtime->role_wait.snapshot.type,
        (int)runtime->user_wait.snapshot.type,
        (int)runtime->user_any_wait.snapshot.type,
        (int)runtime->any_wait.snapshot.type,
        (unsigned long long)runtime->message_wait.snapshot.raw_sequence,
        (unsigned long long)runtime->data_wait.snapshot.raw_sequence,
        (unsigned long long)runtime->message_any_wait.snapshot.raw_sequence,
        (unsigned long long)runtime->data_any_wait.snapshot.raw_sequence,
        (unsigned long long)runtime->member_wait.snapshot.raw_sequence,
        (unsigned long long)runtime->role_wait.snapshot.raw_sequence,
        (unsigned long long)runtime->user_wait.snapshot.raw_sequence,
        (unsigned long long)runtime->user_any_wait.snapshot.raw_sequence,
        (unsigned long long)runtime->any_wait.snapshot.raw_sequence
    );
    return 1;
}

int event_object_wait_smoke_run_waits(
    dcc_client_t *client,
    object_wait_server_t *server,
    pthread_t server_thread,
    dcc_status_t start_status
) {
    object_wait_runtime_t runtime;
    object_wait_thread_group_t threads;
    memset(&threads, 0, sizeof(threads));
    object_wait_runtime_init(&runtime, client);

    if (object_wait_start_threads(&threads, &runtime) != 0) {
        fprintf(stderr, "failed to start object wait threads\n");
        (void)dcc_client_stop(client);
        object_wait_join_threads(&threads, &runtime);
        close(server->fd);
        (void)pthread_join(server_thread, NULL);
        return 1;
    }

    usleep(10000);

    pthread_t wait_thread;
    if (pthread_create(&wait_thread, NULL, client_wait_thread_main, &runtime.wait_state) != 0) {
        fprintf(stderr, "failed to start client wait thread\n");
        (void)dcc_client_stop(client);
        object_wait_join_threads(&threads, &runtime);
        close(server->fd);
        (void)pthread_join(server_thread, NULL);
        return 1;
    }

    object_wait_join_threads(&threads, &runtime);
    (void)dcc_client_stop(client);
    (void)pthread_join(wait_thread, NULL);
    (void)pthread_join(server_thread, NULL);
    close(server->fd);

    int failed = object_wait_validate_runtime(&runtime, server, start_status);
    object_wait_free_results(&runtime);
    return failed;
}

#endif
