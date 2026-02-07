#include <globals/USART_globals.h>

volatile bool g_usart_rx_pending[USART_CNT] = {0, 0, 0};