#pragma once

#ifndef SMCP_USART_GLOBALS_H
#define SMCP_USART_GLOBALS_H

#include <drivers/USART_driver.h>
#include <common/types.h>

extern volatile bool g_usart_rx_pending[USART_CNT];

#endif // SMCP_USART_GLOBALS_H