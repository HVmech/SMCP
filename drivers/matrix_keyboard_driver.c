#include <drivers/matrix_keyboard_driver.h>

#include <core/event_dispatcher.h>
#include <core/event_bus.h>
#include <core/event.h>

#include <drivers/debounce_timer_driver.h>
#include <drivers/time_driver.h>

#include <common/board.h>
#include <common/debug_assert.h>
#include <common/asm.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>

typedef struct {
    bool stable_state;
    bool raw_state;
    uint8_t debounce_counter;
    uint32_t last_change_time;
    uint32_t last_repeat_time;
} key_state_t;

static const matrix_keyboard_config_t *kbd_cfg;
static key_state_t keys[KEYBOARD_ROWS][KEYBOARD_COLUMNS];
static bool scanning_active = false;
static uint32_t last_activity_time = 0;
static uint16_t EXTI_activation_mask = 0;

static inline uint8_t gpio_pin_to_exti_line(uint16_t pin) {
    DEBUG_ASSERT(pin <= GPIO4);
    switch (pin) {
        case GPIO0: return 0;
        case GPIO1: return 1;
        case GPIO2: return 2;
        case GPIO3: return 3;
        case GPIO4: return 4;
        default: return 0;
    }
}

static inline void keyboard_connect_EXTI(void) {
    DEBUG_ASSERT(kbd_cfg);
    EXTI_activation_mask = 0;

    for (uint8_t i = 0; i < KEYBOARD_COLUMNS; ++i) {
        const MCU_pin_t pin = get_MCU_pin(kbd_cfg->column_pins[i]);
        const uint8_t line = gpio_pin_to_exti_line(pin.pin);
        EXTI_activation_mask |= (1 << line);
    }
}

static inline void set_keyboard_EXTI_enable(uint8_t column_index) {
    DEBUG_ASSERT(kbd_cfg);
    const MCU_pin_t pin = get_MCU_pin(kbd_cfg->column_pins[column_index]);
    const uint8_t line = gpio_pin_to_exti_line(pin.pin);
    exti_enable_request(1 << line);
}

static inline void set_keyboard_EXTI_disable(uint8_t column_index) {
    DEBUG_ASSERT(kbd_cfg);
    const MCU_pin_t pin = get_MCU_pin(kbd_cfg->column_pins[column_index]);
    const uint8_t line = gpio_pin_to_exti_line(pin.pin);
    exti_disable_request(1 << line);
}

static inline void keyboard_scanning_start(void) {
    for (uint8_t i = 0; i < KEYBOARD_COLUMNS; ++i) {
        set_keyboard_EXTI_disable(i);
    }

    scanning_active = true;
    timer_enable_counter(TIM2);
}

static inline void keyboard_scanning_stop(void) {
    timer_disable_counter(TIM2);
    scanning_active = false;

    for (int i = 0; i < KEYBOARD_ROWS; ++i) {
        MCU_pin_t pin = get_MCU_pin(kbd_cfg->row_pins[i]);
        gpio_clear(pin.port, pin.pin);
    }
    
    for (uint8_t i = 0; i < KEYBOARD_COLUMNS; ++i) {
        set_keyboard_EXTI_enable(i);
    }
}

static inline void keyboard_execute_matrix_scan(void) {
    DEBUG_ASSERT(kbd_cfg);
    for (uint8_t i = 0; i < KEYBOARD_ROWS; ++i) {
        const MCU_pin_t active_row = get_MCU_pin(kbd_cfg->row_pins[i]);
        gpio_clear(active_row.port, active_row.pin);

        //for (volatile int delay = 0; delay < RC_STABILITY_DELAY_TICK; ++delay) { MACRO_ASM_DO_NOTHING; }
        // Для конденсаторов в 100 нФ должно быть значение 2000-3000

        for (uint8_t j = 0; j < KEYBOARD_COLUMNS; ++j) {
            MCU_pin_t column = get_MCU_pin(kbd_cfg->column_pins[j]);
            key_state_t *key = &keys[i][j];
            key->raw_state = !gpio_get(column.port, column.pin);
        }

        gpio_set(active_row.port, active_row.pin);
    }
}

static inline bool keyboard_execute_debounce_tick(void) {
    bool any_pressed = false;
    const uint32_t evt_time = get_current_time_ms();

    for (uint8_t i = 0; i < KEYBOARD_ROWS; ++i) {
        for (uint8_t j = 0; j < KEYBOARD_COLUMNS; ++j) {
            key_state_t *key = &keys[i][j];

            if (key->debounce_counter > 0 && key->debounce_counter < DEBOUNCE_TICKS) {
                if (key->raw_state) { ++(key->debounce_counter); }
                else { --(key->debounce_counter); }
            }
            else if (key->debounce_counter == 0) {
                if (key->raw_state) {
                    ++(key->debounce_counter);
                    last_activity_time = evt_time;
                }
                else if (key->raw_state != key->stable_state) {
                    event_bus_t *bus = event_dispatcher_get_bus();

                    event_t evt = {0};
                    evt.priority = EVENT_PRIORITY_NORMAL;
                    evt.payload.type = EVENT_DATA_UNSIGNED;

                    const uint8_t keycode = i * KEYBOARD_COLUMNS + j;
                    const uint32_t duration = evt_time - key->last_change_time;
                    const uint32_t duration_max_value = ((1 << 24) - 1);
                    const uint32_t duration_calc = duration <= duration_max_value ? duration : duration_max_value;

                    evt.payload.data.unsigned_value = (duration_calc << 8) | keycode;
                    evt.id = EVENT_KEY_RELEASE;

                    evt.timestamp = evt_time;
                    key->last_change_time = evt_time;
                    key->stable_state = key->raw_state;

                    event_bus_post_from_isr(bus, &evt);
                    last_activity_time = evt_time;
                }
            }
            else if (key->debounce_counter == DEBOUNCE_TICKS) {
                if (!(key->raw_state)) {
                    --(key->debounce_counter);
                    last_activity_time = evt_time;
                }
                else if (key->raw_state != key->stable_state) {
                    event_bus_t *bus = event_dispatcher_get_bus();

                    event_t evt = {0};
                    evt.priority = EVENT_PRIORITY_NORMAL;
                    evt.payload.type = EVENT_DATA_UNSIGNED;
                    evt.payload.data.unsigned_value = i * KEYBOARD_COLUMNS + j;
                    evt.id = EVENT_KEY_PRESS;

                    evt.timestamp = evt_time;
                    key->last_change_time = evt_time;
                    key->last_repeat_time = evt_time;
                    key->stable_state = key->raw_state;

                    event_bus_post_from_isr(bus, &evt);
                    last_activity_time = evt_time;
                }
                else if ((evt_time - key->last_change_time) >= REPEAT_DELAY_MS && (evt_time - key->last_repeat_time) >= REPEAT_PERIOD_MS) {
                    event_bus_t *bus = event_dispatcher_get_bus();

                    event_t evt = {0};
                    evt.priority = EVENT_PRIORITY_NORMAL;
                    evt.payload.type = EVENT_DATA_UNSIGNED;
                    evt.payload.data.unsigned_value = i * KEYBOARD_COLUMNS + j;
                    evt.id = EVENT_KEY_REPEAT;

                    evt.timestamp = evt_time;
                    key->last_repeat_time = evt_time;

                    event_bus_post_from_isr(bus, &evt);
                    last_activity_time = evt_time;
                }
            }

            if (key->stable_state) { any_pressed = true; }
        }
    }

    return any_pressed;
}

static void keyboard_periodic_task(void) {
    if (!scanning_active) { return; }

    keyboard_execute_matrix_scan();
    const bool any_pressed = keyboard_execute_debounce_tick();
    const uint32_t now = get_current_time_ms();

    if (!any_pressed) {

        if ((now - last_activity_time) >= SCAN_IDLE_TIMEOUT_MS) {
            keyboard_scanning_stop();
        }
    }
    static volatile uint32_t tick = 0;
    tick++;
}

static void keyboard_trigger_task(uint8_t line) {
    if (!scanning_active) {
        keyboard_scanning_start();
    }
    exti_reset_request(1 << line);
    //nvic_clear_pending_irq(NVIC_EXTI0_IRQ + line);
    //exti_disable_request(1 << line);
}

void matrix_keyboard_init(const matrix_keyboard_config_t *config) {
    DEBUG_ASSERT(config);

    kbd_cfg = config;

    for (int i = 0; i < KEYBOARD_ROWS; ++i) {
        MCU_pin_t pin = get_MCU_pin(kbd_cfg->row_pins[i]);
        rcc_periph_clock_enable(pin.port);
        gpio_set_mode(pin.port, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, pin.pin);
        gpio_clear(pin.port, pin.pin);
    }

    for (int i = 0; i < KEYBOARD_COLUMNS; ++i) {
        MCU_pin_t pin = get_MCU_pin(kbd_cfg->column_pins[i]);
        rcc_periph_clock_enable(pin.port);
        gpio_set_mode(pin.port, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, pin.pin);
        gpio_set(pin.port, pin.pin);
    }

    delay_ms(GPIO_STABILITY_DELAY_MS); // Страховочная задержка при инициализации

    scanning_active = false;
    last_activity_time = 0;

    debounce_timer_disable();
    debounce_timer_init(SCAN_PERIOD_MS, KEYBOARD_TIMER_PRIORITY, keyboard_periodic_task);

    keyboard_connect_EXTI();
    for (int i = 0; i < KEYBOARD_COLUMNS; ++i) {
        MCU_pin_t pin = get_MCU_pin(kbd_cfg->column_pins[i]);
        const uint32_t line = gpio_pin_to_exti_line(pin.pin);
        const uint32_t line_mask = (1 << line);

        exti_select_source(line_mask, pin.port);
        exti_set_trigger(line_mask, EXTI_TRIGGER_BOTH); // По идее, должно работать независимо от типа подтяжки
        exti_enable_request(line_mask);

        nvic_set_priority(NVIC_EXTI0_IRQ + line, KEYBOARD_TRIGGER_PRIORITY);
        nvic_enable_irq(NVIC_EXTI0_IRQ + line);
    }
}

void EXTI0_Handler(void) { if (EXTI_activation_mask & (1 << 0)) { keyboard_trigger_task(0); } }
void EXTI1_Handler(void) { if (EXTI_activation_mask & (1 << 1)) { keyboard_trigger_task(1); } }
void EXTI2_Handler(void) { if (EXTI_activation_mask & (1 << 2)) { keyboard_trigger_task(2); } }
void EXTI3_Handler(void) { if (EXTI_activation_mask & (1 << 3)) { keyboard_trigger_task(3); } }
void EXTI4_Handler(void) { if (EXTI_activation_mask & (1 << 4)) { keyboard_trigger_task(4); } }