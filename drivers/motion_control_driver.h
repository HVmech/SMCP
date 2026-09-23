#pragma once
#ifndef SMCP_MOTION_CONTROL_DRIVER_H
#define SMCP_MOTION_CONTROL_DRIVER_H

#include <common/board.h>
#include <common/types.h>

#define CONST_ENA_DELAY_TIME_MS 10
#define CONST_DIR_DELAY_TIME_MS 1

bool motion_control_init(board_pin_e dir_pin, board_pin_e ena_pin);
void set_motion_control_enable(bool state);
void set_motion_control_direction(bool direction);

#endif // SMCP_MOTION_CONTROL_DRIVER_H