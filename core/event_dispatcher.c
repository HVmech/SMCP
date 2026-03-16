#include <core/event_dispatcher.h>

#include <services/debug_serial_service.h>
#include <services/LED_service.h>
#include <drivers/time_driver.h>
#include <common/asm.h>
#include <common/debug_assert.h>

static event_dispatcher_t dispatcher = {0};

void event_dispatcher_init(event_bus_t *bus) {
    DEBUG_ASSERT(bus);

    dispatcher.bus = bus;
}

event_bus_t* event_dispatcher_get_bus() {
    return dispatcher.bus;
}

bool event_dispatcher_process() {
    DEBUG_ASSERT(dispatcher.bus);

    event_bus_t *bus = dispatcher.bus;
    event_t evt = {0};
    bool result = false;

     // Активация событий по приоритетам: HIGH -> NORMAL -> LOW
    if (bus->queues[EVENT_PRIORITY_HIGH] && ring_buffer_pop(bus->queues[EVENT_PRIORITY_HIGH], &evt)) { // HIGH
        event_bus_invoke(bus, &evt);
        result = true;
    }
    else if (bus->queues[EVENT_PRIORITY_NORMAL] && ring_buffer_pop(bus->queues[EVENT_PRIORITY_NORMAL], &evt)) { // NORMAL
        event_bus_invoke(bus, &evt);
        result = true;
    }
    else if (bus->queues[EVENT_PRIORITY_LOW] && ring_buffer_pop(bus->queues[EVENT_PRIORITY_LOW], &evt)) { // LOW
        event_bus_invoke(bus, &evt);
        result = true;
    }

    if (result) { debug_serial_printf("[%u] EVENT PROCESSED: %d\n", g_SysTick_cnt, evt.id); }

    return result; // Все очереди пусты
}
