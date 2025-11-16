#include "as5600.h"

//SCL = PB6
//SDA = PB7

uint8_t as5600_data[2];
uint8_t as5600_status = 0;

void as5600_retrieve_angle(void)
{

    // Read raw angle (12-bit value)
    HAL_I2C_Mem_Read(&hi2c2, AS5600L_ADDR, AS5600L_RAW_ANGLE_H, 1, as5600_data, 2, 100);

    // Add angle to data register
    update_sensor_data(as5600_data[0], as5600_data[1]);

}

void as5600_retrieve_status(void)
{

    HAL_I2C_Mem_Read(&hi2c2, AS5600L_ADDR, AS5600L_STATUS, 1, &as5600_status, 1, 100);

    as5600_status &= 0x38;
    update_sensor_status(as5600_status);

}


