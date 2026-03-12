#pragma once
#ifndef SMCP_APP_CONTEXT_H
#define SMCP_APP_CONTEXT_H

#include <common/types.h>
#include <core/input_data.h>

typedef struct {
    int32_t current_angle;
    input_context_t input_context;
} app_context_t;

extern app_context_t app_context;

#endif // SMCP_APP_CONTEXT_H