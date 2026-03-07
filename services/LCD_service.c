#include <services/LCD_service.h>

#include <core/event_dispatcher.h>
#include <core/event_bus.h>
#include <core/event.h>

#include <drivers/time_driver.h>

void LCD_update_request(void) {
    event_bus_t *bus = event_dispatcher_get_bus();

    event_t evt = {0};
    evt.id = EVENT_LCD_UPDATE_REQUEST;
    evt.priority = EVENT_PRIORITY_LOW;
    evt.flags = EVENT_FLAG_DEDUPLICATE_LAST;
    evt.timestamp = get_current_time_ms();

    event_bus_post(bus, &evt);
}