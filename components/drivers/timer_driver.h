#pragma once

#include "../../common/types.h"

namespace n_RCC // Должно быть с отдельным драйвером, пока для примера здесь
{
    inline constexpr t_UINT8 CONST_OSC_FREQ_MHZ = 8;
}

namespace n_TIM1
{
    using t_TIMER1_CALLBACK = void (*)(void);

    void init(t_UINT32 T_ms, t_BOOL status);

    void set(t_BOOL state);

    void toggle(void);

    void set_callback(t_TIMER1_CALLBACK func);
    void interrupt_handler(); 

    t_BOOL getState();
    // Метод с возвратом текущей конфигурации делать не буду, т.к. пример
}