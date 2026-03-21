#include <globals/motor_telemetry_globals.h>

volatile bool g_generate_motor_telemetry_updates = false;
volatile motor_telemetry_t g_motor_telemetry = {0};