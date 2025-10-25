#include "i2c_slave.h"


#define SENSOR_STATUS		1
#define SENSOR_RAW_ANGLE	3
#define MOTOR_DIRECTION		5
#define MOTOR_SPEED			6

#define MAX_DATA_LENGTH		6

// Global variables for I2C slave operation
uint8_t slave_rx_buffer[6] = {0, 0, 0, 0, 0, 0};
uint8_t slave_tx_buffer[6] = {0, 0, 0, 0, 0, 0};

uint8_t SENSOR_REGISTERS[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t MOTOR_REGISTERS[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

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

    // Master is sending to slave
    if (TransferDirection == I2C_DIRECTION_TRANSMIT) {
    	log_info("Receiving data direction\n\r");
    	data_size = 0;
    	data_direction = 0;
    	data_address = 0;
    	rx_flag = 0;
    	HAL_I2C_Slave_Seq_Receive_IT(hi2c, &data_size, 1, I2C_FIRST_FRAME);
    }
    // Master is reading from slave
    else if (TransferDirection == I2C_DIRECTION_RECEIVE) {
    	if (data_size && data_direction && data_address) {
    		log_info("Preparing data to send\n\r");
    		memset(slave_tx_buffer, 0, 6);
    		// Prepare data based on the previously received register address
    		prepare_data_to_send();
    		log_info("Sending data to master\n\r");
    		HAL_I2C_Slave_Seq_Transmit_IT(hi2c, slave_tx_buffer, data_size, I2C_LAST_FRAME);
    	}
    	else {
    		log_error("Master attempting to read data without setting data properties\n\r");
    		Error_Handler();
    	}
    }
    else {
    	log_error("Invalid data transfer direction\n\r");
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
	log_info("Data size : %d (bytes)\n\r"
			"Data direction: %d (write=1, read=2)\n\r"
			"Data address: %d\n\r"
			"slave_rx_buffer[0]: %d\n\r"
			"slave_rx_buffer[1]: %d\n\r"
			"slave_rx_buffer[2]: %d\n\r"
			"rx_flag: %d\n\r",
			data_size, data_direction, data_address, slave_rx_buffer[0],
			slave_rx_buffer[1], slave_rx_buffer[2], rx_flag);

	if (!data_direction) {
		log_info("Receiving data direction\n\r");
		HAL_I2C_Slave_Seq_Receive_IT(hi2c, &data_direction, 1, I2C_NEXT_FRAME);
	}

	else if (data_direction == 1){
		if (!data_address) {
			log_info("Receiving data address\n\r");
			//Go to next frame as data will be in the same write command
			HAL_I2C_Slave_Seq_Receive_IT(hi2c, &data_address, 1, I2C_NEXT_FRAME);
		}
		else if (data_size && data_address) {
			if (!rx_flag) {
				log_info("Receiving data from master\n\r");
				//To prevent buffer overflow, should never occur
				if (data_size > sizeof(slave_rx_buffer)) data_size = sizeof(slave_rx_buffer);
				memset(slave_rx_buffer, 0, 6);
				rx_flag=1;
				HAL_I2C_Slave_Seq_Receive_IT(hi2c, slave_rx_buffer, data_size, I2C_LAST_FRAME);
			}
			else {
				log_info("Data received, processing...\n\r");
				process_received_data();
				HAL_I2C_EnableListen_IT(hi2c);
			}
		}
	}

	else if (data_direction == 2) {
		if (!data_address) {
			log_info("Receiving data address\n\r");
			//Go to last frame as restart condition will occur due to change in direction
			HAL_I2C_Slave_Seq_Receive_IT(hi2c, &data_address, 1, I2C_LAST_FRAME);
		}
		else if (data_size && data_address) {
			log_info("Directives received (setup to send data to master)\n\r");
			HAL_I2C_EnableListen_IT(hi2c);
		}
	}
	else {
		// Instead of Error_Handler()
		log_error("Invalid data direction: Exiting slave receive callback\n\r");
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
	if (data_size || data_direction || data_address) {
		log_info("Sent requested data\n\r");
	}

	else {
		// Instead of Error_Handler()
		log_error("Received data without any data properties\n\r");
		HAL_I2C_EnableListen_IT(hi2c);
	}
}

/**
 * @brief Prepare data to be transmitted based on register address
 * @retval None
 */
void process_received_data(void)
{
    switch (data_address) {
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
void prepare_data_to_send(void)
{
    switch (data_address) {
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
	uint32_t errorcode = HAL_I2C_GetError(hi2c);
	log_error("Error code: %d \n\r", errorcode);
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
    SENSOR_REGISTERS[5] = angle_msb;
    SENSOR_REGISTERS[6] = angle_lsb;
}

void update_sensor_status(uint8_t status)
{
    SENSOR_REGISTERS[0] = status;
}






