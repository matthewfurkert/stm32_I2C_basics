/*
 * data_transfer.c
 *
 *  Created on: Oct 30, 2025
 *      Author: matthew
 */

#include "data_transfer.h"

#define SENSOR_STATUS		1
#define SENSOR_RAW_ANGLE	3
#define MOTOR_DIRECTION		5
#define MOTOR_SPEED			6

uint8_t slave_rx_buffer[6] = {0};
uint8_t slave_tx_buffer[6] = {0};
uint8_t SENSOR_REGISTERS[10] = {0};
uint8_t MOTOR_REGISTERS[10] = {0};

/**
 * @brief Prepare data to be transmitted based on register address
 * @retval None
 */

void process_received_data(uint8_t address)
{
    switch (address) {
        case MOTOR_DIRECTION: // Motor direction register
            // Transfer the received data into the motor register
            MOTOR_REGISTERS[0] = slave_rx_buffer[0];
            break;

        case MOTOR_SPEED: // Motor speed register
            MOTOR_REGISTERS[5] = slave_rx_buffer[0];
            break;

        default:
            // Unknown data
        MOTOR_REGISTERS[8] = slave_rx_buffer[0];
        MOTOR_REGISTERS[9] = slave_rx_buffer[1];
            break;
    }
}

/**
 * @brief Prepare data to be transmitted based on register address
 * @retval None
 */
void prepare_data_to_send(uint8_t address)
{
    switch (address) {
        case SENSOR_RAW_ANGLE: // Raw angle register
            // Data is stored in positions 5 and 6 of SENSOR_REGISTERS
            slave_tx_buffer[0] = SENSOR_REGISTERS[5]; // MSB
            slave_tx_buffer[1] = SENSOR_REGISTERS[6]; // LSB
            break;

        case SENSOR_STATUS: // Magnitude register
            slave_tx_buffer[0] = SENSOR_REGISTERS[0];
            break;

        default:
            // Unknown register, return zeros
            slave_tx_buffer[0] = 0x00;
            slave_tx_buffer[1] = 0x00;
            break;
    }
}


/**
 * @brief Update sensor data in the data registers
 * @param angle_msb Most significant byte of angle data
 * @param angle_lsb Least significant byte of angle data
 * @retval None
 */
void update_sensor_data(uint8_t angle_msb, uint8_t angle_lsb)
{
    SENSOR_REGISTERS[5] = angle_msb;
    SENSOR_REGISTERS[6] = angle_lsb;
}

void update_sensor_status(uint8_t status)
{
    SENSOR_REGISTERS[0] = status;
}

