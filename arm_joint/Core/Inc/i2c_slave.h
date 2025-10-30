#ifndef __I2C_SLAVE_H__
#define __I2C_SLAVE_H__

#include <string.h>
#include "main.h"
#include "uart_comms.h"
#include "data_transfer.h"

extern volatile uint8_t i2c_log_flags;

#define FLAG_LISTEN			1
#define FLAG_ADDR_MATCH_RX	2
#define FLAG_ADDR_MATCH_TX	4
#define FLAG_RX_CPLT		8
#define FLAG_TX_CPLT		16
#define FLAG_ERROR			128

extern uint8_t last_size, last_dir, last_addr, last_rx_flag;

void I2C_Slave_Init(I2C_HandleTypeDef *hi2c);


#endif
