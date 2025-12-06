#pragma once
#include <common/types.h>

typedef struct {
    uint32_t base;
    uint8_t irq;
} smcp_gp_timer_t;

void smcp_gp_timer_init(const smcp_gp_timer_t *t, uint32_t freq_hz);
void smcp_gp_timer_start(const smcp_gp_timer_t *t);
void smcp_gp_timer_stop(const smcp_gp_timer_t *t);
void smcp_gp_timer_clear_irq(const smcp_gp_timer_t *t);