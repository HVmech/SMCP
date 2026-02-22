#include <services/motion_planning_service.h>
#include <services/debug_serial_service.h>


#include <core/event_dispatcher.h>
#include <core/event_bus.h>

#include <services/motion_controller_service.h>
#include <services/motion_execution_service.h>

#include <drivers/SysTick_driver.h>

#include <common/debug_assert.h>

static motion_controller_state_t controller = {0};

static inline uint32_t angle_to_steps(int32_t angle) {
    uint32_t abs_angle = (angle >= 0) ? angle : -angle;

    uint64_t steps = MOTOR_FULL_STEPS_PER_REV * DRIVER_MICROSTEP * GEARBOX_RATIO * abs_angle / (360 * 10000);

    return (uint32_t)steps;
}

void motion_controller_init(board_pin_e dir_pin, board_pin_e ena_pin) {
    DEBUG_ASSERT(!controller.busy && !controller.prepared);

    motion_controller_reset();

    motion_executor_init(dir_pin, ena_pin);

    controller.bus = event_dispatcher_get_bus();

    event_bus_subscribe(controller.bus, EVENT_MOTOR_ROTATION_REQUEST, motion_controller_service_event_handler);
    event_bus_subscribe(controller.bus, EVENT_MOTOR_PREPARATION_COMPLETE, motion_controller_service_event_handler);
    event_bus_subscribe(controller.bus, EVENT_MOTOR_ROTATION_COMPLETE, motion_controller_service_event_handler);
    event_bus_subscribe(controller.bus, EVENT_MOTOR_EMERGENCY_STOP, motion_controller_service_event_handler);
    
    event_bus_subscribe(controller.bus, EVENT_MOTOR_ROTATION_TEST_REQUEST, motion_controller_service_event_handler);
}

void motion_controller_prepare(int32_t angle) {
    DEBUG_ASSERT(controller.bus);
    motion_controller_reset();

    bool direction = (angle >= 0);
    uint32_t steps = angle_to_steps(angle);

    if (steps == 0) { return; }

    controller.block = plan_motion(steps, direction);
    controller.prepared = true;

    event_t evt = {0};
    evt.id = EVENT_MOTOR_ROTATION_PREPARE;
    evt.priority = EVENT_PRIORITY_HIGH;
    evt.flags = EVENT_FLAG_NONE;
    evt.timestamp = g_SysTick_cnt;

    event_bus_post(controller.bus, &evt);
}

void motion_controller_prepare_test(bool direction, int32_t f) {
    DEBUG_ASSERT(controller.bus);
    motion_controller_reset();

    //if (f < f_min) { return; }

    controller.block.direction = direction;
    controller.block.motion_phases[PHASE_C].ddf0 = 0;
    controller.block.motion_phases[PHASE_C].df0 = 0;
    controller.block.motion_phases[PHASE_C].f0 = f;
    controller.block.motion_phases[PHASE_C].update_steps = (MOTOR_FULL_STEPS_PER_REV * DRIVER_MICROSTEP) / repetitions;

    debug_serial_printf("f_tim = %u\r\n", f_tim);
    debug_serial_printf("f_max = %d, f_min = %d\r\n", f_max, f_min);
    debug_serial_printf("df_max = %d, ddf_max = %d\r\n", df_max, ddf_max);

    debug_serial_printf("s = %u, f = %d, r = %u\r\n", controller.block.motion_phases[PHASE_C].update_steps, f, repetitions);
    controller.prepared = true;

    event_t evt = {0};
    evt.id = EVENT_MOTOR_ROTATION_PREPARE;
    evt.priority = EVENT_PRIORITY_HIGH;
    evt.flags = EVENT_FLAG_NONE;
    evt.timestamp = g_SysTick_cnt;

    event_bus_post(controller.bus, &evt);
}

void motion_controller_start(void) {
    DEBUG_ASSERT(controller.bus);

    event_t evt = {0};
    evt.id = EVENT_MOTOR_ROTATION_START;
    evt.priority = EVENT_PRIORITY_HIGH;
    evt.flags = EVENT_FLAG_NONE;
    evt.timestamp = g_SysTick_cnt;

    controller.busy = true;

    event_bus_post(controller.bus, &evt);
    motion_executor_start(&controller.block);
}

void motion_controller_reset(void)
{
    controller.prepared = false;
    controller.busy = false;

    controller.block.direction = false;
    controller.block.tail_phase = false;

    for (int i = 0; i < PHASE_COUNT; ++i)
    {
        controller.block.motion_phases[i].f0 = 0;
        controller.block.motion_phases[i].df0 = 0;
        controller.block.motion_phases[i].ddf0 = 0;
        controller.block.motion_phases[i].update_steps = 0;
    }
}

void motion_controller_service_event_handler(const event_t *evt) {
    DEBUG_ASSERT(evt);

    switch (evt->id) {
        case EVENT_MOTOR_ROTATION_REQUEST: {
            if (controller.busy) { return; }
            if (evt->payload.type != EVENT_DATA_SIGNED) { return; }

            motion_controller_prepare(evt->payload.data.signed_value);
            break;
        }
        case EVENT_MOTOR_PREPARATION_COMPLETE: {
            if (!controller.prepared) { return; }
            if (controller.busy) { return; }

            motion_controller_start();
            break;
        }
        case EVENT_MOTOR_ROTATION_COMPLETE:
        case EVENT_MOTOR_EMERGENCY_STOP: {
            controller.busy = false;
            controller.prepared = false;
            break;
        }
        case EVENT_MOTOR_ROTATION_TEST_REQUEST: {
            if (controller.busy) { return; }
            if (evt->payload.type != EVENT_DATA_SIGNED) { return; }

            bool direction = evt->payload.data.signed_value > 0;
            int32_t f = direction ? evt->payload.data.signed_value : (-1 * evt->payload.data.signed_value);
            motion_controller_prepare_test(direction, f);
            break;
        }
        default: { return; }
    }
}
