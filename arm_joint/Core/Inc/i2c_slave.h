#ifndef __I2C_SLAVE_H__
#define __I2C_SLAVE_H__

#include <string.h>
#include "main.h"
#include "uart_comms.h"

void update_sensor_data(uint8_t angle_msb, uint8_t angle_lsb);
void update_sensor_status(uint8_t status);
void process_received_data(void);
void prepare_data_to_send(void);
//void prepare_read_data(void);
void I2C_Slave_Init(I2C_HandleTypeDef *hi2c);


#endif
