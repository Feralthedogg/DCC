#include "internal/events/dcc_events_internal.h"

void dcc_event_bus_lock(dcc_event_bus_t *bus) {
    while (atomic_flag_test_and_set_explicit(&bus->lock, memory_order_acquire)) {
        llam_yield();
    }
}

void dcc_event_bus_unlock(dcc_event_bus_t *bus) {
    atomic_flag_clear_explicit(&bus->lock, memory_order_release);
}
