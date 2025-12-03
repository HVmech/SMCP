#include "led_service.h"

namespace n_LED_SERVICE
{
    constexpr t_UINT32 CONST_DEFAULT_PERIOD = 1000;

    void c_LED_SERVICE::init(void)
    {
        n_GPIO::init(true);
        n_TIM1::init(CONST_DEFAULT_PERIOD, false);
        n_TIM1::set_callback(n_GPIO::toggle);
        period_ms = CONST_DEFAULT_PERIOD;
        status = LED_OFF;
    }

    void c_LED_SERVICE::set(e_LED_STATUS state)
    {
        switch (state)
        {
            case LED_OFF :
            {
                n_GPIO::set(true);
                if (n_GPIO::get_state() == true) { status = LED_OFF; }
                n_TIM1::set(false);
                break;
            }
            case LED_ON :
            {
                n_GPIO::set(false);
                if (n_GPIO::get_state() == false) { status = LED_ON; }
                n_TIM1::set(false);
                break;
            }
            case LED_BLINK :
            {
                n_GPIO::set(true);
                n_TIM1::init(period_ms, false);
                n_TIM1::set_callback(n_GPIO::toggle);
                n_TIM1::set(true);
                status = LED_BLINK;
                break;
            }
        }
    }

    void c_LED_SERVICE::set_blink_period_ms(t_UINT32 T_ms)
    {
        if (status == LED_BLINK)
        {
            n_TIM1::set(false);
            period_ms = T_ms;
            n_TIM1::init(period_ms, true);
        } else
        {
            period_ms = T_ms;
        }
    }

    e_LED_STATUS c_LED_SERVICE::get_state() const
    {
        return status;
    }

    t_UINT32 c_LED_SERVICE::get_period() const
    {
        return period_ms;
    }
}

n_LED_SERVICE::c_LED_SERVICE g_BUILT_IN_LED {};