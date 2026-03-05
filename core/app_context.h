#pragma once
#ifndef SMCP_APP_CONTEXT_H
#define SMCP_APP_CONTEXT_H

#include <common/types.h>

typedef struct {
    uint8_t pos;
    int32_t value;
    bool mode;
} input_data_t;

typedef struct {
    int32_t current_angle;
    input_data_t input_data;
} app_context_t;

extern app_context_t app_context;

#endif // SMCP_APP_CONTEXT_H