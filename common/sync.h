#pragma once

#ifndef SMCP_SYNC_H
#define SMCP_SYNC_H

#include <libopencm3/cm3/cortex.h>

#define MACRO_ENTER_CRITICAL_SECTION cm_disable_interrupts()
#define MACRO_EXIT_CRITICAL_SECTION cm_enable_interrupts()

#endif // SMCP_SYNC_H