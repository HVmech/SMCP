#pragma once
#include <libopencm3/cm3/sync.h>

#ifndef SMCP_ASM_H
#define SMCP_ASM_H

#define MACRO_ASM_DO_NOTHING __asm__ volatile ("nop") // Простой процессора
#define MACRO_ASM_DATA_SYNC_BARRIER __asm__ volatile ("dsb") // Переход в режим ожидания прерывания
#define MACRO_ASM_DATA_MEMORY_BARRIER __asm__ volatile ("dmb") // Указание на завершение всех предшествующих операций с памятью
#define MACRO_ASM_WAIT_FOR_INTERRUPT __asm__ volatile ("wfi") // Переход в режим ожидания прерывания

#endif // SMCP_ASM_H