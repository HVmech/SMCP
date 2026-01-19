#pragma once

#ifndef SMCP_TIME_DRIVER_H
#define SMCP_TIME_DRIVER_H

#include <common/types.h>

uint32_t get_current_time_ms(void); // Получение счетчика текущего времени в мс
uint32_t get_elapsed_time_ms(uint32_t start_time_ms); // Получение времени в мс до текущего момента

#endif // SMCP_TIME_DRIVER_H