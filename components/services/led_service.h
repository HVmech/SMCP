#pragma once

#include "../drivers/gpio_driver.h"
#include "../drivers/timer_driver.h"

namespace n_LED_SERVICE
{
    enum e_LED_STATUS : t_UINT8
    {
        LED_OFF = 0,
        LED_ON = 1,
        LED_BLINK = 2
    };

    class c_LED_SERVICE
    {
    public:
        c_LED_SERVICE() = default;
        ~c_LED_SERVICE() = default;

        void init(void);

        void set(e_LED_STATUS state);

        void set_blink_period_ms(t_UINT32 T_ms);

        e_LED_STATUS get_state() const;
        t_UINT32 get_period() const;
    private:
        e_LED_STATUS status;
        t_UINT32 period_ms;
    };
}

extern n_LED_SERVICE::c_LED_SERVICE g_BUILT_IN_LED;