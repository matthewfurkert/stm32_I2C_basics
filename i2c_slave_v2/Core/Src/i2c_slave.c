#include "main.h"
#include "i2c_slave.h"
#include <string.h>

// Global variables for I2C slave operation
uint8_t DATA_REGISTERS[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t current_register = 0;
uint8_t slave_tx_buffer[2];
volatile uint8_t register_address_received = 0;

/**
 * @brief Slave Address Match callback.
 * @param hi2c Pointer to a I2C_HandleTypeDef structure that contains
 *        the configuration information for the specified I2C.
 * @param TransferDirection Master request Transfer Direction (Write/Read)
 * @param AddrMatchCode Address Match Code
 * @retval None
 */
void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
    UNUSED(AddrMatchCode);

    if (TransferDirection == I2C_DIRECTION_TRANSMIT) {
        // Master is writing to slave (sending register address)
        register_address_received = 0;
        HAL_I2C_Slave_Seq_Receive_IT(hi2c, &current_register, 1, I2C_FIRST_FRAME);
    }
    else if (TransferDirection == I2C_DIRECTION_RECEIVE) {
        // Master is reading from slave
        // Prepare data based on the previously received register address
        prepare_read_data();
        HAL_I2C_Slave_Seq_Transmit_IT(hi2c, slave_tx_buffer, 2, I2C_LAST_FRAME);
    }
}

/**
 * @brief I2C Slave Tx Transfer completed callback.
 * @param hi2c Pointer to a I2C_HandleTypeDef structure that contains
 *        the configuration information for the specified I2C.
 * @retval None
 */
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    // Re-enable slave mode for next transaction
    HAL_I2C_EnableListen_IT(hi2c);
}

/**
 * @brief I2C Slave Rx Transfer completed callback.
 * @param hi2c Pointer to a I2C_HandleTypeDef structure that contains
 *        the configuration information for the specified I2C.
 * @retval None
 */
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    register_address_received = 1;
    // Continue listening for the read phase of the transaction
    HAL_I2C_EnableListen_IT(hi2c);
}

/**
 * @brief Prepare data to be transmitted based on register address
 * @retval None
 */
void prepare_read_data(void)
{
    switch (current_register) {
        case 0x08: // Raw angle register
            // Data is stored in positions 5 and 6 of DATA_REGISTERS
            slave_tx_buffer[0] = DATA_REGISTERS[5]; // MSB
            slave_tx_buffer[1] = DATA_REGISTERS[6]; // LSB
            break;

        case 0x1B: // Magnitude register
            // You can use different positions or calculate magnitude
            // For now, using the same data as angle
            slave_tx_buffer[0] = DATA_REGISTERS[5]; // MSB
            slave_tx_buffer[1] = DATA_REGISTERS[6]; // LSB
            break;

        default:
            // Unknown register, return zeros
            slave_tx_buffer[0] = 0x00;
            slave_tx_buffer[1] = 0x00;
            break;
    }
}

/**
 * @brief I2C Listen Complete callback.
 * @param hi2c Pointer to a I2C_HandleTypeDef structure that contains
 *        the configuration information for the specified I2C.
 * @retval None
 */
void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
    // Re-enable listening mode
    HAL_I2C_EnableListen_IT(hi2c);
}

/**
 * @brief I2C error callback.
 * @param hi2c Pointer to a I2C_HandleTypeDef structure that contains
 *        the configuration information for the specified I2C.
 * @retval None
 */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    // Handle errors and re-enable listening
    HAL_I2C_EnableListen_IT(hi2c);
}

/**
 * @brief Initialize I2C slave mode
 * @param hi2c Pointer to I2C handle
 * @retval None
 */
void I2C_Slave_Init(I2C_HandleTypeDef *hi2c)
{
    // Enable listening mode
    if (HAL_I2C_EnableListen_IT(hi2c) != HAL_OK)
    	{
    	  Error_Handler();
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
    DATA_REGISTERS[5] = angle_msb;
    DATA_REGISTERS[6] = angle_lsb;
}

void update_sensor_status(uint8_t status)
{
    DATA_REGISTERS[7] = status;
}


