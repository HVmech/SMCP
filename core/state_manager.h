#pragma once
#ifndef SMCP_STATE_MANAGER_H
#define SMCP_STATE_MANAGER_H

#include <core/app_state.h>
#include <core/idle_state.h>
#include <core/input_state.h>
#include <core/active_state.h>
#include <core/error_state.h>

extern app_state_code_t current_state;

void state_manager_init(void);
void state_manager_event_handler(const event_t *evt);

#endif // SMCP_STATE_MANAGER_H
