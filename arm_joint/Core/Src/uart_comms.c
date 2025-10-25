#include "uart_comms.h"

int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
	return ch;
}

void log_error(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	printf("log error: ");
	vprintf(format, args);
	printf("\r\n");
	va_end(args);
}
void log_info(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    printf("log info: ");
    vprintf(format, args);
    printf("\r\n");
    va_end(args);
}

void log_debug(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	printf("log debug: ");
	vprintf(format, args);
	printf("\r\n");
	va_end(args);
}

void log_warn(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	printf("log warn: ");
	vprintf(format, args);
	printf("\r\n");
	va_end(args);
}
