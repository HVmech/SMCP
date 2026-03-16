#include "common/asm.h"
#include <core/event_bus.h>
#include <common/debug_assert.h>
#include <common/utils.h>
#include <common/sync.h>

#include <services/debug_serial_service.h>
#include <drivers/time_driver.h>

static inline ring_buffer_t *event_bus_get_queue(event_bus_t *bus, event_priority_t priority) { // Получение указателя на очередь
    DEBUG_ASSERT(bus);
    DEBUG_ASSERT(priority < EVENT_PRIORITY_NUM);

    return bus->queues[priority];
}

static bool event_bus_deduplicate_last(ring_buffer_t *rb, const event_t *evt) { // Проверка дубликации последнего события
    DEBUG_ASSERT(rb && evt);

    event_t last;
    if (!ring_buffer_peek_last(rb, &last)) { return false; }

    return last.id == evt->id;
}

void event_bus_init(event_bus_t *bus, ring_buffer_t *queues[EVENT_PRIORITY_NUM]) {
    DEBUG_ASSERT(bus);
    DEBUG_ASSERT(queues);

    for (uint32_t i = 0; i < EVENT_PRIORITY_NUM; ++i) {
        bus->queues[i] = queues[i];
        bus->stats[i].dropped = 0;
        bus->stats[i].deduplicated = 0;
        bus->stats[i].posted_from_isr = 0;
        bus->stats[i].posted_from_main = 0;
        bus->stats[i].invoked = 0;
    }

    for (uint32_t i = 0; i < EVENT_TYPES_NUM; i++) {
        bus->subscriptions[i].handler_count = 0;
    }
}

bool event_bus_subscribe(event_bus_t *bus, event_id_t id, event_handler_t handler) {
    DEBUG_ASSERT(bus);
    DEBUG_ASSERT(handler);
    DEBUG_ASSERT(id < EVENT_TYPES_NUM);

    const uint8_t cnt = bus->subscriptions[id].handler_count;

    if (cnt >= EVENT_BUS_MAX_SUBSCRIBERS) { return false; }

    bus->subscriptions[id].handler[cnt] = handler;
    ++(bus->subscriptions[id].handler_count);

    return true;
}

bool event_bus_unsubscribe(event_bus_t *bus, event_id_t id, event_handler_t handler) {
    DEBUG_ASSERT(bus && handler);
    DEBUG_ASSERT(id < EVENT_TYPES_NUM);

    const uint8_t cnt = bus->subscriptions[id].handler_count;

    for (uint8_t i = 0; i < cnt; i++) {
        if (bus->subscriptions[id].handler[i] == handler) {
            for (uint8_t j = i; j + 1 < cnt; ++j) {
                bus->subscriptions[id].handler[j] = bus->subscriptions[id].handler[j + 1];
            }
            --(bus->subscriptions[id].handler_count);
            return true;
        }
    }

    return false;
}

bool event_bus_post(event_bus_t *bus, const event_t *evt) {
    DEBUG_ASSERT(bus);
    DEBUG_ASSERT(evt);

    ring_buffer_t *rb = event_bus_get_queue(bus, evt->priority);
    DEBUG_ASSERT(rb);

    //debug_serial_printf("[%u] MEP\n", get_current_time_ms());

    bool result = true;
    MACRO_ENTER_CRITICAL_SECTION;

    if (evt->flags & EVENT_FLAG_DEDUPLICATE_LAST) {
        if (event_bus_deduplicate_last(rb, evt)) {
            ++(bus->stats[evt->priority].deduplicated);
            result = false;
        }
    }
    if (result) {
        if (!ring_buffer_push(rb, evt)) {
            ++(bus->stats[evt->priority].dropped);
            result = false;
        } else {
            ++(bus->stats[evt->priority].posted_from_main);
        }
    }

    MACRO_EXIT_CRITICAL_SECTION;
    return result;
}

bool event_bus_post_from_isr(event_bus_t *bus, const event_t *evt) {
    DEBUG_ASSERT(bus);
    DEBUG_ASSERT(evt);

    ring_buffer_t *rb = event_bus_get_queue(bus, evt->priority);
    DEBUG_ASSERT(rb);

    //debug_serial_printf("[%u] IEP\n", get_current_time_ms());

    bool result = true;
    MACRO_ENTER_CRITICAL_SECTION;

    if (evt->flags & EVENT_FLAG_DEDUPLICATE_LAST) {
        if (event_bus_deduplicate_last(rb, evt)) {
            ++(bus->stats[evt->priority].deduplicated);
            result = false;
        }
    }
    if (result) {
        if (!ring_buffer_push(rb, evt)) {
            ++(bus->stats[evt->priority].dropped);
            result = false;
        }
        else {
            ++(bus->stats[evt->priority].posted_from_isr);
        }
    }

    MACRO_ASM_DATA_SYNC_BARRIER;
    MACRO_EXIT_CRITICAL_SECTION;
    return result;
}

void event_bus_invoke(event_bus_t *bus, const event_t *evt) {
    DEBUG_ASSERT(bus);
    DEBUG_ASSERT(evt);
    DEBUG_ASSERT(evt->id < EVENT_TYPES_NUM);

    const uint8_t cnt = bus->subscriptions[evt->id].handler_count;
    //debug_serial_printf("[%u] MEI\n", get_current_time_ms());

    for (uint8_t i = 0; i < cnt; i++) {
        const event_handler_t call_handler = bus->subscriptions[evt->id].handler[i];
        DEBUG_ASSERT(call_handler);
        ++(bus->stats[evt->priority].invoked);
        call_handler(evt);
    }
}

void event_bus_clear(event_bus_t *bus) {
    DEBUG_ASSERT(bus);

    for (uint32_t i = 0; i < EVENT_PRIORITY_NUM; i++) {
        if (bus->queues[i]) {
            ring_buffer_clear(bus->queues[i]);
        }

        bus->stats[i].dropped = 0;
        bus->stats[i].deduplicated = 0;
        bus->stats[i].posted_from_isr = 0;
        bus->stats[i].posted_from_main = 0;
        bus->stats[i].invoked = 0;
    }

    for (uint32_t i = 0; i < EVENT_TYPES_NUM; i++) {
        bus->subscriptions[i].handler_count = 0;
    }
}
