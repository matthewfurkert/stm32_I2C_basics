/*
 * data_transfer.h
 *
 *  Created on: Oct 30, 2025
 *      Author: matthew
 */
#include <stdint.h>
#include "main.h"

#ifndef INC_DATA_TRANSFER_H_
#define INC_DATA_TRANSFER_H_

void process_received_data(uint8_t address);
void prepare_data_to_send(uint8_t address);
void update_sensor_data(uint8_t angle_msb, uint8_t angle_lsb);
void update_sensor_status(uint8_t status);
uint8_t update_motor_direction(void);
uint8_t update_motor_speed(void);

extern uint8_t SENSOR_REGISTERS[10];
extern uint8_t MOTOR_REGISTERS[10];

// Global variables for I2C slave operation
extern uint8_t slave_rx_buffer[6];
extern uint8_t slave_tx_buffer[6];

#endif /* INC_DATA_TRANSFER_H_ */
