#include "i2c_slave.h"

volatile uint8_t i2c_log_flags = 0;

uint8_t data_size;
uint8_t data_direction;
uint8_t data_address;
uint8_t rx_flag;


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

    // Master is sending to slave (slave is receiving (RX))
    if (TransferDirection == I2C_DIRECTION_TRANSMIT) {
    	i2c_log_flags |= FLAG_ADDR_MATCH_RX;					// For debugging
    	data_size = 0;
    	data_direction = 0;
    	data_address = 0;
    	rx_flag = 0;
    	HAL_I2C_Slave_Seq_Receive_IT(hi2c, &data_size, 1, I2C_FIRST_FRAME);
    }
    // Master is reading from slave (slave is transmitting (TX))
    else if (TransferDirection == I2C_DIRECTION_RECEIVE) {
    	i2c_log_flags |= FLAG_ADDR_MATCH_TX;					// For debugging
    	if (data_size && data_direction && data_address) {
    		memset(slave_tx_buffer, 0, 6);
    		// Prepare data based on the previously received register address
    		prepare_data_to_send(data_address);
    		HAL_I2C_Slave_Seq_Transmit_IT(hi2c, slave_tx_buffer, data_size, I2C_LAST_FRAME);
    	}
    	else {
    		log_error("Master attempting to read data without setting data properties\r\n");
    		Error_Handler();
    	}
    }
    else {
    	log_error("Invalid data transfer direction\r\n");
    	Error_Handler();
    }
}

/**
 * @brief I2C Slave Rx Transfer completed callback.
 * @param hi2c Pointer to a I2C_HandleTypeDef structure that contains
 *        the configuration information for the specified I2C.
 * @retval None
 */
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	i2c_log_flags |= FLAG_RX_CPLT; // For debugging

	if (!data_direction) {
		HAL_I2C_Slave_Seq_Receive_IT(hi2c, &data_direction, 1, I2C_NEXT_FRAME);
	}

	else if (data_direction == 1){
		if (!data_address) {
			//Go to next frame as data will be in the same write command
			HAL_I2C_Slave_Seq_Receive_IT(hi2c, &data_address, 1, I2C_NEXT_FRAME);
		}
		else if (data_size && data_address) {
			if (!rx_flag) {
				//To prevent buffer overflow, should never occur
				if (data_size > sizeof(slave_rx_buffer)) data_size = sizeof(slave_rx_buffer);
				memset(slave_rx_buffer, 0, 6);
				rx_flag=1;
				HAL_I2C_Slave_Seq_Receive_IT(hi2c, slave_rx_buffer, data_size, I2C_LAST_FRAME);
			}
			else {
				process_received_data(data_address);
				HAL_I2C_EnableListen_IT(hi2c);
			}
		}
	}

	else if (data_direction == 2) {
		if (!data_address) {
			//Go to last frame as restart condition will occur due to change in direction
			HAL_I2C_Slave_Seq_Receive_IT(hi2c, &data_address, 1, I2C_LAST_FRAME);
		}
		else if (data_size && data_address) {
			HAL_I2C_EnableListen_IT(hi2c);
		}
	}
	else {
		HAL_I2C_EnableListen_IT(hi2c);
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
	i2c_log_flags |= FLAG_TX_CPLT; // For debugging

	if (data_size || data_direction || data_address) {
	}

	else {
		HAL_I2C_EnableListen_IT(hi2c);
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
    i2c_log_flags |= FLAG_LISTEN;
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
//	uint32_t errorcode = HAL_I2C_GetError(hi2c);
//	log_error("Error code: %d \n\r", errorcode);
	i2c_log_flags |= FLAG_ERROR;
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






