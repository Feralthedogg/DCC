#ifndef DCC_TEST_EVENT_OBJECT_WAIT_SMOKE_SUPPORT_H
#define DCC_TEST_EVENT_OBJECT_WAIT_SMOKE_SUPPORT_H

#include <dcc/dcc.h>

#if !defined(_WIN32)
#include <pthread.h>
#include <stdatomic.h>
#include <stddef.h>

typedef struct object_wait_server {
    int fd;
    unsigned short port;
    atomic_uint accepts;
    atomic_uint identifies;
    atomic_uint bad;
} object_wait_server_t;

typedef struct client_wait_state {
    dcc_client_t *client;
    dcc_status_t status;
} client_wait_state_t;

typedef struct message_wait_state {
    dcc_client_t *client;
    dcc_status_t status;
    dcc_event_snapshot_t snapshot;
    dcc_event_type_t types[2];
    size_t type_count;
    dcc_message_t *message;
} message_wait_state_t;

typedef struct data_wait_state {
    dcc_client_t *client;
    dcc_status_t status;
    dcc_event_snapshot_t snapshot;
    dcc_event_type_t types[2];
    size_t type_count;
    dcc_gateway_event_data_t *data;
} data_wait_state_t;

typedef struct member_wait_state {
    dcc_client_t *client;
    dcc_status_t status;
    dcc_event_snapshot_t snapshot;
    dcc_member_t *member;
} member_wait_state_t;

typedef struct role_wait_state {
    dcc_client_t *client;
    dcc_status_t status;
    dcc_event_snapshot_t snapshot;
    dcc_role_t *role;
} role_wait_state_t;

typedef struct user_wait_state {
    dcc_client_t *client;
    dcc_status_t status;
    dcc_event_snapshot_t snapshot;
    dcc_event_type_t types[2];
    size_t type_count;
    dcc_user_t *user;
} user_wait_state_t;

typedef struct any_wait_state {
    dcc_client_t *client;
    dcc_status_t status;
    dcc_event_snapshot_t snapshot;
    dcc_event_type_t types[2];
    size_t type_count;
} any_wait_state_t;

typedef struct sequence_wait_state {
    dcc_client_t *client;
    dcc_status_t status;
    dcc_event_type_t types[4];
    dcc_event_snapshot_t snapshots[4];
    size_t type_count;
    size_t completed;
} sequence_wait_state_t;

typedef struct set_wait_state {
    dcc_client_t *client;
    dcc_status_t status;
    dcc_event_type_t types[4];
    dcc_event_snapshot_t snapshots[4];
    size_t type_count;
    size_t completed;
} set_wait_state_t;

typedef struct count_wait_state {
    dcc_client_t *client;
    dcc_status_t status;
    dcc_event_type_t types[4];
    dcc_event_snapshot_t snapshots[4];
    size_t type_count;
    size_t target_count;
    size_t completed;
} count_wait_state_t;

typedef struct quorum_wait_state {
    dcc_client_t *client;
    dcc_status_t status;
    dcc_event_type_t types[4];
    dcc_event_snapshot_t snapshots[4];
    size_t type_count;
    size_t target_count;
    size_t completed;
} quorum_wait_state_t;

int start_listener(int *out_fd, unsigned short *out_port);
void *server_thread_main(void *arg);
void *client_wait_thread_main(void *arg);
void *message_wait_thread_main(void *arg);
void *data_wait_thread_main(void *arg);
void *message_any_wait_thread_main(void *arg);
void *data_any_wait_thread_main(void *arg);
void *member_wait_thread_main(void *arg);
void *role_wait_thread_main(void *arg);
void *user_wait_thread_main(void *arg);
void *user_any_wait_thread_main(void *arg);
void *any_wait_thread_main(void *arg);
void *sequence_wait_thread_main(void *arg);
void *set_wait_thread_main(void *arg);
void *count_wait_thread_main(void *arg);
void *quorum_wait_thread_main(void *arg);

int event_object_wait_smoke_check_invalid_handling(dcc_client_t *client);
int event_object_wait_smoke_run_waits(
    dcc_client_t *client,
    object_wait_server_t *server,
    pthread_t server_thread,
    dcc_status_t start_status
);

#endif

#endif
