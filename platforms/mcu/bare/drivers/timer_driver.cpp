//#include "../../../../common/types.h"
#include "../registers.h"
#include "../../../../components/drivers/timer_driver.h"

namespace n_TIM1 
{
    static t_TIMER1_CALLBACK l_CALLBACK = nullptr;

    void init(t_UINT32 T_ms, t_BOOL state) {
        RCC_APB2ENR |= (1 << 11);

        const t_UINT32 SYSTEM_CORE_CLOCK = 8000000;
        const t_UINT32 TIMER_FREQ = 1000;

        TIM1_PSC = (SYSTEM_CORE_CLOCK / TIMER_FREQ) - 1;

        TIM1_ARR = (TIMER_FREQ * T_ms / 1000) - 1;

        TIM1_DIER |= 1;

        TIM1_CR1 |= (1 << 3);
        TIM1_CR1 &= ~(1 << 3);

        NVIC_ISERO |= (1 << 25);
        
        set(state);
    }

    void set(t_BOOL state)
    {
        if (state)
        {
            TIM1_CR1 |= (1 << 0);
        }
        else
        {
            TIM1_CR1 &= ~(1 << 0);
        }
    }

    void toggle(void)
    {
        TIM1_CR1 ^= (1 << 0);
    }

    void set_callback(t_TIMER1_CALLBACK func)
    {
        l_CALLBACK = func;
    }

    void interrupt_handler()
    {
        TIM1_SR = 0;

        if (l_CALLBACK != nullptr) {
            l_CALLBACK();
        }
    }

    t_BOOL getState()
    {
        return TIM1_CR1 & (1 << 0);
    }
}