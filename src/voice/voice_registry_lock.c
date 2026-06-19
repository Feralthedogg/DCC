#include "internal/voice/dcc_voice_internal.h"

void dcc_voice_client_registry_lock(dcc_client_t *client) {
    while (atomic_flag_test_and_set_explicit(&client->voice_clients_lock, memory_order_acquire)) {
        llam_yield();
    }
}

void dcc_voice_client_registry_unlock(dcc_client_t *client) {
    atomic_flag_clear_explicit(&client->voice_clients_lock, memory_order_release);
}
