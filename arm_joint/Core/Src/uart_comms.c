#include "uart_comms.h"

// DMA transmit buffers (double buffering)
#define UART_TX_BUF_SIZE 256
static uint8_t uart_tx_buffer[2][UART_TX_BUF_SIZE];
static volatile uint8_t active_buffer = 0;  // Buffer being filled
static volatile uint16_t tx_write_idx = 0;
static volatile uint8_t tx_busy = 0;

uint8_t  last_size  = 0;
uint8_t  last_dir   = 0;
uint16_t last_addr  = 0;
uint8_t  last_rx_flag = 0;

// Call this once during initialization
void uart_dma_init(void)
{
    tx_write_idx = 0;
    tx_busy = 0;
    active_buffer = 0;
}

// Transmit buffer contents via DMA
static void uart_dma_start_transmit(uint8_t buffer_idx, uint16_t data_length)
{
	uint32_t start = HAL_GetTick();
	while (tx_busy) {
		HAL_Delay(1);
		// Timeout protection
		if ((HAL_GetTick() - start) > 100) {
			return;
		}
    }

    __disable_irq();
    // Don't start DMA if no data
    if (data_length == 0) {
    	__enable_irq();
        return;
    }

    tx_busy = 1;
    __enable_irq();

    // Start DMA transmission with the filled buffer
    HAL_UART_Transmit_DMA(&huart1, uart_tx_buffer[buffer_idx], data_length);
}

// DMA transfer complete callback
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == huart1.Instance) {
        tx_busy = 0;
    }
}

// Modified putchar - buffers data instead of blocking
int __io_putchar(int ch)
{
	// Wait until there's space
	__disable_irq();
	if (tx_write_idx < UART_TX_BUF_SIZE) {
		// Space available - write immediately
	    uart_tx_buffer[active_buffer][tx_write_idx++] = (uint8_t)ch;
	}
	__enable_irq();

    // Auto-flush on newline or when buffer is nearly full
    if (ch == '\n' || tx_write_idx >= (UART_TX_BUF_SIZE - 16)) {
    	__disable_irq();
    	uint8_t buf_to_send = active_buffer;
    	uint16_t len = tx_write_idx;
    	active_buffer = 1 - active_buffer;  // Switch to other buffer
    	tx_write_idx = 0;  // Reset index for new buffer
    	__enable_irq();
        uart_dma_start_transmit(buf_to_send, len);

    }

    return ch;
}

// Manual flush function
void uart_flush(void)
{
    uart_dma_start_transmit(active_buffer, tx_write_idx);
    while (tx_busy) {
        HAL_Delay(1);
    }
}

////Non DMA UART Transmission
//int __io_putchar(int ch)
//{
//	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
//	return ch;
//}

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
