#include <libopencm3/cm3/cortex.h>
#include <common/asm.h>

void assert(const char* file, int line) { // Проверка истинности утверждения
    cm_disable_interrupts(); // Отключение прерываний

    (void)file; (void)line; // Подавление предупреждений

    while (1) { MACRO_ASM_DO_NOTHING; } // Зависание в цикле
}