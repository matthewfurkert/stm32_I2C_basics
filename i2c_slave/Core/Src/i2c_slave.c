#include "main.h"
#include "i2c_slave.h"

uint8_t I2C_REGISTERS[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t datatosend[6] = {2, 5, 8, 9, 12, 34};

extern I2C_HandleTypeDef hi2c2;

#define RxSIZE 11
uint8_t RxData[RxSIZE];
uint8_t rxcount = 0;
uint8_t txcount = 0;
uint8_t start_position = 0;

int is_first_recvd = 0;
int countAddr = 0;
int countrxcplt = 0;
int counterror = 0;

void process_data (void)
{
//	HAL_I2C_Mem_Write(&hi2c2, countrxcplt, 6, 1, txdata, 4, 1000);
	int startREG = RxData[0];
	int numREG = rxcount - 1;
	int endREG = startREG + numREG-1;
	if (endREG > 9) Error_Handler();

	int indx = 1;
	for (int i = 0; i < numREG; i++)
	{
		I2C_REGISTERS[startREG++] = RxData[indx++];
	}
}


void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
	HAL_I2C_EnableListen_IT(hi2c);
}


void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
	if (TransferDirection == I2C_DIRECTION_TRANSMIT)
	{
		if (is_first_recvd == 0)
		{
			rxcount = 0;
			countAddr++;
			HAL_I2C_Slave_Seq_Receive_IT(hi2c, RxData+rxcount, 1, I2C_FIRST_FRAME);
		}

	}
	else
	{
		txcount = 0;
		start_position = RxData[1];
		HAL_I2C_Slave_Seq_Transmit_IT(hi2c, I2C_REGISTERS+start_position+txcount, 1, I2C_FIRST_FRAME);
	}
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	txcount++;
	HAL_I2C_Slave_Seq_Transmit_IT(hi2c, I2C_REGISTERS+start_position+txcount, 1, I2C_NEXT_FRAME);
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if (is_first_recvd == 0)
	{
		rxcount++;
		is_first_recvd = 1;
		HAL_I2C_Slave_Seq_Receive_IT(hi2c, RxData+rxcount, RxData[0], I2C_LAST_FRAME);
	}
	else
	{
		rxcount += RxData[0];
		is_first_recvd = 0;
		process_data();
	}
}


void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	counterror++;
	uint32_t errorcode = HAL_I2C_GetError(hi2c);
	if (errorcode == 4)		// AF error
	{
		__HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);	//Clear AF flag
		if (txcount == 0)	//Error whilst slave is receiving
		{
			process_data();
		}
		else				//Error whilst slave is transmitting
		{
			txcount--;
		}

	}
	HAL_I2C_EnableListen_IT(hi2c);
}


//#include "main.h"
//#include "i2c_slave.h"
//#include <string.h>
//
//uint8_t DATA_REGISTERS[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//
//extern I2C_HandleTypeDef hi2c2;
//
//#define RxSIZE 11
//uint8_t RxData[RxSIZE];
//uint8_t rxcount = 0;
//uint8_t txcount = 0;
//uint8_t start_position = 0;
//
//int is_first_recvd = 0;
//int countAddr = 0;
//int countrxcplt = 0;
//int counterror = 0;
//int errornum = 0;
//int bus_error =0;
//
//// Global variables for tracking
//uint8_t last_txcount = 0;
//int read_stall_count = 0;
//
//void check_i2c_stall(I2C_HandleTypeDef *hi2c) {
//    HAL_I2C_StateTypeDef hal_state = HAL_I2C_GetState(hi2c);
//    uint32_t sr1 = hi2c->Instance->SR1;
//    uint32_t sr2 = hi2c->Instance->SR2;
//    uint32_t cr1 = hi2c->Instance->CR1;
//
//    // Always log state for debugging
//    char msg[100];
//    snprintf(msg, sizeof(msg), "I2C1 State: HAL=0x%X, SR1=0x%04X (AF=%d, BERR=%d, OVR=%d), SR2=0x%04X (BUSY=%d, TRA=%d), CR1=0x%04X, TxCount=%d",
//             hal_state, sr1, (sr1 & I2C_SR1_AF) ? 1 : 0, (sr1 & I2C_SR1_BERR) ? 1 : 0, (sr1 & I2C_SR1_OVR) ? 1 : 0,
//             sr2, (sr2 & I2C_SR2_BUSY) ? 1 : 0, (sr2 & I2C_SR2_TRA) ? 1 : 0, cr1, txcount);
//    log_info(msg);
//
//    // Stall detection (unchanged, but now with more context)
//    if (hal_state == HAL_I2C_STATE_BUSY && last_txcount == txcount && txcount > 0) {
//        read_stall_count++;
//        snprintf(msg, sizeof(msg), "Stall detected on I2C1: TxCount=%d", txcount);
//        log_error(msg);
//
//        // Recovery...
//        HAL_I2C_DeInit(hi2c);
//        HAL_I2C_Init(hi2c);
//        // Reset variables...
//        HAL_I2C_EnableListen_IT(hi2c);
//        log_info("I2C1 reset due to stall");
//    }
//    last_txcount = txcount;
//}
//
//void process_data(void)
//{
//    if (rxcount == 1) {
//        // Single byte received - this is a read setup commandSSS
//        start_position = RxData[0];
//
//        // Validate register address
//        if (start_position > 9) {
//            start_position = 0; // Default to register 0 if invalid
//        }
//        return;
//    }
//
//    // Multiple bytes received - this is a write command
//    // Format: [start_register, data1, data2, ...]
//    int startREG = RxData[0];
//    int numREG = rxcount - 1;  // First byte is register address, rest is data
//    int endREG = startREG + numREG - 1;
//
//    if (endREG > 9 || startREG > 9) {
//        Error_Handler();
//        return;
//    }
//
//    int indx = 1;
//    for (int i = 0; i < numREG; i++) {
//        DATA_REGISTERS[startREG + i] = RxData[indx + i];
//    }
//}
//
//void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
//{
//    HAL_I2C_EnableListen_IT(hi2c);
//}
//
//void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
//{
//    if (TransferDirection == I2C_DIRECTION_TRANSMIT) {
//        // Master wants to write to slave
//        if (is_first_recvd == 0) {
//            rxcount = 0;
//            countAddr++;
//            HAL_I2C_Slave_Seq_Receive_IT(hi2c, RxData + rxcount, 1, I2C_FIRST_FRAME);
//        }
//    } else {
//        // Master wants to read from slave (I2C_DIRECTION_RECEIVE)
//        txcount = 0;
//
//        // Send data starting from the previously set start_position
//        // The master will determine how many bytes to read
//        if (start_position <= 9) {
//            HAL_I2C_Slave_Seq_Transmit_IT(hi2c, DATA_REGISTERS + start_position + txcount, 1, I2C_FIRST_FRAME);
//        } else {
//            // Invalid start position - send from register 0
//            start_position = 0;
//            HAL_I2C_Slave_Seq_Transmit_IT(hi2c, DATA_REGISTERS, 1, I2C_FIRST_FRAME);
//        }
//    }
//}
//
//void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
//{
//    txcount++;
//
//    // Continue sending bytes as long as master requests them
//    // The master controls how many bytes to read by sending NACK
//    if ((start_position + txcount) < 10) {
//        HAL_I2C_Slave_Seq_Transmit_IT(hi2c, DATA_REGISTERS + start_position + txcount, 1, I2C_NEXT_FRAME);
//    } else {
//        // Reached end of register array - stop transmission
//        // The HAL will handle the NACK from master
//    }
//}
//
//void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
//{
//    if (is_first_recvd == 0) {
//        // First byte received
//        rxcount = 1;  // We have 1 byte so far
//        is_first_recvd = 1;
//
//        // Check if this might be a read setup (single byte transaction)
//        // We can't know for sure until the transaction ends, so we need to
//        // prepare to receive more data but handle the case where transaction ends
//
//        // Try to receive more data - if master sends STOP after first byte,
//        // we'll get an error callback and process as single byte
//        HAL_I2C_Slave_Seq_Receive_IT(hi2c, RxData + rxcount, 1, I2C_NEXT_FRAME);
//
//    } else {
//        // Additional bytes received - this is definitely a write operation
//        rxcount++;
//
//        // Continue receiving until master sends STOP (which will trigger error callback)
//        if (rxcount < RxSIZE) {
//            HAL_I2C_Slave_Seq_Receive_IT(hi2c, RxData + rxcount, 1, I2C_NEXT_FRAME);
//        }
//    }
//}
//
//void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
//{
//    counterror++;
//    uint32_t errorcode = HAL_I2C_GetError(hi2c);
//    errornum = errorcode;
//
//    if (errorcode == 4) { // AF error (Acknowledge Failure)
//        __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF); // Clear AF flag
//
//        // Process whatever data we received
//        if (txcount == 0)	//Error whilst slave is receiving
//        {
//        	process_data();
//        }
//        else				//Error whilst slave is transmitting
//        {
//        	txcount--;
//        }
//    }
//    else if (errorcode != 1) { // BERR (Bus Error)
//    	bus_error++;
//    	HAL_I2C_DeInit(hi2c);
//    	HAL_I2C_Init(hi2c); // Reinitialize with original parameters
//    	memset(RxData, 0, RxSIZE); // Clear receive buffer
//    	rxcount = 0;
//    	txcount = 0;
//    	is_first_recvd = 0;
//    	start_position = 0;
//    	HAL_I2C_EnableListen_IT(hi2c);
//    }
//
//    // Reset state for next transaction
//    is_first_recvd = 0;
//    rxcount = 0;
//
//    HAL_I2C_EnableListen_IT(hi2c);
//}
