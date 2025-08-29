#include "adxl.h"

uint8_t data_rec[6];
uint8_t device_id;

void adxl_write(uint8_t reg, uint8_t value)
{
	uint8_t data[2];
	data[0] = reg;
	data[1] = value;

	HAL_I2C_Master_Transmit(&hi2c1, DEVICE_ADDR, data, 2, 100);

}

void adxl_read_values(uint8_t reg)
{
	HAL_I2C_Mem_Read(&hi2c1, DEVICE_ADDR, reg, 1, (uint8_t *)data_rec, 6, 100);
}

void adxl_read_address(uint8_t reg)
{
	HAL_I2C_Mem_Read(&hi2c1, DEVICE_ADDR, reg, 1, &device_id, 1, 100);
}

void adxl_init(void)
{
	//Read device id
	adxl_read_address(DEVID_R);
	//Set data format to +/- 4g
	adxl_write(DATA_FORMAT_R, FOUR_G);
	//Reset all bits
	adxl_write(POWER_CTL_R, RESET);
	//Config power control
	adxl_write(POWER_CTL_R, SET_MEASURE_B);

}
