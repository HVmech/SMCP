#include <globals/motor_telemetry_globals.h>

volatile bool g_generate_motor_telemetry_updates = false;
volatile uint8_t g_current_phase = 0;
volatile uint32_t g_phase_progress_percentage = 0;