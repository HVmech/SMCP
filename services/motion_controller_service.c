#include <services/motion_planning_service.h>
#include <services/debug_serial_service.h>

#include <core/app_context.h>
#include <core/event_dispatcher.h>
#include <core/event_bus.h>
#include <core/event.h>

#include <services/motion_controller_service.h>
#include <services/motion_execution_service.h>

#include <drivers/time_driver.h>
#include <drivers/SysTick_driver.h>

#include <common/utils.h>
#include <common/debug_assert.h>
#include <stdint.h>

static motion_controller_state_t controller = {0};

static inline uint32_t angle_to_steps(int32_t angle_input) {
    const uint32_t abs_angle = (angle_input >= 0) ? angle_input : -angle_input;

    const uint32_t steps_per_rev = MOTOR_FULL_STEPS_PER_REV * DRIVER_MICROSTEP * GEARBOX_RATIO;

    uint32_t div = 360;

    for (uint8_t i = 0; i < ANGLE_PRECISION; ++i) {
        div *= 10;
    }

    //debug_serial_printf("Begin:\r\n");
    //debug_serial_printf("mul1 = %u\r\n", steps_per_rev);
    //debug_serial_printf("mul2 = %u\r\n", abs_angle);
    //debug_serial_printf("div = %u\r\n", div);
    //debug_serial_printf("\r\n");

    uint32_t mul1 = steps_per_rev;
    uint32_t temp_gcd = gcd(mul1, div);
    mul1 /= temp_gcd;
    div /= temp_gcd;

    uint32_t mul2 = abs_angle;
    temp_gcd = gcd(mul2, div);
    mul2 /= temp_gcd;
    div /= temp_gcd;

    //debug_serial_printf("End:\r\n");
    //debug_serial_printf("mul1 = %u\r\n", mul1);
    //debug_serial_printf("mul2 = %u\r\n", mul2);
    //debug_serial_printf("div = %u\r\n", div);
    //debug_serial_printf("\r\n");

    DEBUG_ASSERT((uint64_t)mul1 * (uint64_t)mul2 < UINT32_MAX);

    const uint32_t steps_reminder = (((mul1 * mul2 * 10) / div) % 10 >= 5) ? 1 : 0;
    const uint32_t steps = mul1 * mul2 / div + steps_reminder;

    debug_serial_printf("AtS: steps = %u, reminder = %u\r\n", steps, steps_reminder);

    return steps;
}

static inline uint32_t steps_to_angle(uint32_t steps) {
    const uint32_t steps_per_rev = MOTOR_FULL_STEPS_PER_REV * DRIVER_MICROSTEP * GEARBOX_RATIO;

    uint32_t div = steps_per_rev;

    uint32_t mul1 = 360;

    for (uint8_t i = 0; i < ANGLE_PRECISION; ++i) {
        mul1 *= 10;
    }

    uint32_t temp_gcd = gcd(mul1, div);
    mul1 /= temp_gcd;
    div /= temp_gcd;

    uint32_t mul2 = steps;
    temp_gcd = gcd(mul2, div);
    mul2 /= temp_gcd;
    div /= temp_gcd;

    DEBUG_ASSERT((uint64_t)mul1 * (uint64_t)mul2 < UINT32_MAX);

    const uint32_t angle_reminder = (((mul1 * mul2 * 10) / div) % 10 >= 5) ? 1 : 0;
    const uint32_t angle = mul1 * mul2 / div + angle_reminder;

    return angle;
}

static inline void motion_controller_update_current_angle(uint32_t steps, bool reverse_direction) {
    const uint32_t steps_per_rev = MOTOR_FULL_STEPS_PER_REV * DRIVER_MICROSTEP * GEARBOX_RATIO;

    DEBUG_ASSERT(steps && steps <= steps_per_rev);

    if (reverse_direction) {
        if (steps > controller.position_in_steps) {
            controller.position_in_steps = steps_per_rev - (steps - controller.position_in_steps);
        }
        else {
            controller.position_in_steps = controller.position_in_steps - steps;
        }
    }
    else {
        if (((uint64_t)steps + (uint64_t)controller.position_in_steps) >= (uint64_t)steps_per_rev) {
            controller.position_in_steps = (((uint64_t)steps + (uint64_t)controller.position_in_steps) - (uint64_t)steps_per_rev);
        }
        else {
            controller.position_in_steps = controller.position_in_steps + steps;
        }
    }

    app_context.current_angle = steps_to_angle(controller.position_in_steps);
}

void motion_controller_init(board_pin_e dir_pin, board_pin_e ena_pin) {
    DEBUG_ASSERT(!controller.busy && !controller.prepared);

    motion_controller_reset();
    controller.position_in_steps = 0; // TODO: load from eeprom?

    motion_executor_init(dir_pin, ena_pin);

    controller.bus = event_dispatcher_get_bus();

    event_bus_subscribe(controller.bus, EVENT_MOTOR_ROTATION_REQUEST, motion_controller_service_event_handler);
    event_bus_subscribe(controller.bus, EVENT_MOTOR_PREPARATION_COMPLETE, motion_controller_service_event_handler);
    event_bus_subscribe(controller.bus, EVENT_MOTOR_ROTATION_COMPLETE, motion_controller_service_event_handler);
    event_bus_subscribe(controller.bus, EVENT_MOTOR_EMERGENCY_STOP, motion_controller_service_event_handler);
    event_bus_subscribe(controller.bus, EVENT_MOTOR_RECOVERY_REQUEST, motion_controller_service_event_handler);
    
    event_bus_subscribe(controller.bus, EVENT_MOTOR_ROTATION_TEST_REQUEST, motion_controller_service_event_handler);
}

void motion_controller_prepare(int32_t angle) {
    DEBUG_ASSERT(controller.bus);
    motion_controller_reset();

    bool reverse = (angle < 0);
    uint32_t steps = angle_to_steps(angle);

    if (steps == 0) { 
        controller.prepared = false;
        event_t evt = {0};
        evt.id = EVENT_MOTOR_ROTATION_COMPLETE;
        evt.priority = EVENT_PRIORITY_HIGH;
        evt.flags = EVENT_FLAG_NONE;
        evt.timestamp = g_SysTick_cnt;

        event_bus_post(controller.bus, &evt);
    }
    else {
        //debug_serial_printf("r = %u\r\n", repetitions);
        //debug_serial_printf("steps = %u\r\n", steps);

        controller.block = plan_motion(steps, reverse);
        controller.prepared = true;

        event_t evt = {0};
        evt.id = EVENT_MOTOR_ROTATION_PREPARE;
        evt.priority = EVENT_PRIORITY_HIGH;
        evt.flags = EVENT_FLAG_NONE;
        evt.timestamp = g_SysTick_cnt;

        event_bus_post(controller.bus, &evt);
    }
}

void motion_controller_prepare_test(bool reverse, int32_t f) {
    DEBUG_ASSERT(controller.bus);

    motion_controller_reset();

    //if (f < f_min) { return; }

    controller.block.reverse_direction = reverse;
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

    controller.block.reverse_direction = false;
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

            debug_serial_printf("angle = %d\r\n", evt->payload.data.signed_value);

            //debug_serial_printf("delay = %u\r\n", 10000);
            //delay_ms(10000);

            motion_controller_prepare(evt->payload.data.signed_value);
            break;
        }
        case EVENT_MOTOR_PREPARATION_COMPLETE: {
            if (!controller.prepared) { return; }
            if (controller.busy) { return; }

            motion_controller_start();
            break;
        }
        case EVENT_MOTOR_ROTATION_COMPLETE: {
            const uint32_t steps_made = motion_executor_get_position_change_in_steps();
            const bool is_reversed = controller.block.reverse_direction;

            motion_controller_update_current_angle(steps_made, is_reversed);
            // TODO: save to eeprom?

            controller.busy = false;
            controller.prepared = false;
            break;
        }
        case EVENT_MOTOR_EMERGENCY_STOP: {
            controller.busy = false;
            controller.prepared = false;
            break;
        }
        case EVENT_MOTOR_RECOVERY_REQUEST: {
            motion_executor_recovery();

            controller.position_in_steps = 0;
            app_context.current_angle = 0;
            // so simple?
            // TODO: planning, executing zero position seek attempt, validating result 

            DEBUG_ASSERT(controller.bus);

            event_t evt = {0};
            evt.id = EVENT_MOTOR_RECOVERY_COMPLETE;
            evt.priority = EVENT_PRIORITY_NORMAL;
            evt.flags = EVENT_FLAG_NONE;
            evt.timestamp = g_SysTick_cnt;

            event_bus_post(controller.bus, &evt);
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
