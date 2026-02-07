#pragma once

#ifndef SMCP_ASM_H
#define SMCP_ASM_H

#define MACRO_ASM_DO_NOTHING __asm__ volatile ("nop") // Простой процессора
#define MACRO_ASM_DATA_SYNC_BARRIER __asm__ volatile ("dsb") // Переход в режим ожидания прерывания
#define MACRO_ASM_DATA_MEMORY_BARRIER __asm__ volatile ("dmb") // Указание на завершение всех предшествующих операций с памятью
#define MACRO_ASM_WAIT_FOR_INTERRUPT __asm__ volatile ("wfi") // Переход в режим ожидания прерывания
#define MACRO_ASM_WAIT_FOR_EVENT __asm__ volatile ("wfe") // Переход в режим ожидания события
#define MACRO_ASM_SET_EVENT __asm__ volatile ("sev") // Выдача события
#define MACRO_ASM_INSTRUCTION_SYNC_BARRIER __asm__ volatile ("isb") // Переход в режим завершщения инструкций

#endif // SMCP_ASM_H