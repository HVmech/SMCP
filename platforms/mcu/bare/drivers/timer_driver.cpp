//#include "../../../../common/types.h"
#include "../registers.h"
#include "../../../../components/drivers/timer_driver.h"

namespace n_TIM1 
{
    static t_TIMER1_CALLBACK l_CALLBACK = nullptr;

    void init(t_UINT32 T_ms, t_BOOL state)
    {
        // Упрощенный расчет, просто для примера
        t_UINT32 prsclr = n_RCC::CONST_OSC_FREQ_MHZ * T_ms - 1;
        t_UINT32 atrld = (n_RCC::CONST_OSC_FREQ_MHZ * T_ms / 1000) - 1;

        NVIC_ISERO = (1 << 25);
        
        TIM1_PSC = prsclr;
        TIM1_ARR = atrld;

        TIM1_DIER |= 1;

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

        if (l_CALLBACK != nullptr)
        {
            l_CALLBACK();
        }
    }

    t_BOOL getState()
    {
        return TIM1_CR1 & (1 << 0);
    }
}