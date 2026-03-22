#pragma once
#ifndef SMCP_MOTION_CONTROLLER_SERVICE_H
#define SMCP_MOTION_CONTROLLER_SERVICE_H

#include <core/event_bus.h>
#include <core/event.h>

#include <services/motion_planning_service.h>

#include <common/board.h>

#define ANGLE_PRECISION 4

typedef struct {
    motion_block_t block;
    bool prepared;
    bool busy;
    event_bus_t *bus; // ?
    uint32_t position_in_steps;
} motion_controller_state_t;

void motion_controller_init(board_pin_e dir_pin, board_pin_e ena_pin);
void motion_controller_prepare(int32_t angle);
void motion_controller_start(void);
void motion_controller_reset(void);
void motion_controller_service_event_handler(const event_t *evt);

#endif // SMCP_MOTION_CONTROLLER_SERVICE_H
