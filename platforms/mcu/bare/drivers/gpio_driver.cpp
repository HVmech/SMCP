//#include "../../../../common/types.h"
#include "../registers.h"
#include "../../../../components/drivers/gpio_driver.h"

namespace n_GPIO
{
    void init(t_BOOL state)
    {
        RCC_APB2ENR |= (1 << 4) | (1 << 11);

        GPIOC.CRH &= ~(0xF << 20);
        GPIOC.CRH |= (0x6 << 20);

        set(state);
    }

    void set(t_BOOL state)
    {
        if (!state)
        {
            GPIOC.ODR &= ~(1 << 13);
        } else
        {
            GPIOC.ODR |= (1 << 13);
        }
    }

    void toggle()
    {
        GPIOC.ODR ^= (1 << 13);
    }

    t_BOOL get_state()
    {
        return GPIOC.ODR & (1 << 13);
    }
}





