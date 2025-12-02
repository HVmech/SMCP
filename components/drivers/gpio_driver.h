#pragma once

#include "../../common/types.h"

namespace n_GPIO
{
    void init(t_BOOL state = false);

    void set(t_BOOL state);

    void toggle(void);

    t_BOOL get_state();
};