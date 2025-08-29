

#include "as5600.h"

//SCL = PB6
//SDA = PB7

uint8_t as5600_data[2];

void as5600_write(uint8_t reg, uint8_t value)
{
    uint8_t data[2];
    data[0] = reg;
    data[1] = value;

    HAL_I2C_Master_Transmit(&hi2c1, AS5600L_ADDR, data, 2, 100);
}

uint16_t as5600_read_angle(void)
{
    uint16_t angle = 0;

    // Read raw angle (12-bit value)
    HAL_I2C_Mem_Read(&hi2c1, AS5600L_ADDR, AS5600L_RAW_ANGLE_H, 1, as5600_data, 2, 100);

    // Combine high and low bytes
    angle = ((uint16_t)as5600_data[0] << 8) | as5600_data[1];

    // Mask to 12 bits (0-4095)
    angle &= 0x0FFF;

    return angle;
}

uint8_t as5600_read_status(void)
{
    uint8_t status = 0;

    HAL_I2C_Mem_Read(&hi2c1, AS5600L_ADDR, AS5600L_STATUS, 1, &status, 1, 100);

    status &= 0x38;

    return status;
}

uint16_t as5600_read_magnitude(void)
{
    uint16_t magnitude = 0;

    HAL_I2C_Mem_Read(&hi2c1, AS5600L_ADDR, AS5600L_MAGNITUDE_H, 1, as5600_data, 2, 100);

    magnitude = ((uint16_t)as5600_data[0] << 8) | as5600_data[1];
    magnitude &= 0x0FFF;  // 12-bit value

    return magnitude;
}

uint8_t as5600_read_agc(void)
{
    uint8_t agc = 0;

    HAL_I2C_Mem_Read(&hi2c1, AS5600L_ADDR, AS5600L_AGC, 1, &agc, 1, 100);

    return agc;
}

void as5600_init(void)
{
    // Optional: Configure the sensor
    // The AS5600 works out of the box with default settings
    // You can configure PWM frequency, output mode, etc. if needed

    // Example: Set PWM frequency to 920Hz and output mode to PWM
    // uint16_t config = AS5600_CONF_PWMF_920HZ | AS5600_CONF_OUTS_PWM;
    // as5600_write(AS5600_CONF_L, (uint8_t)(config & 0xFF));
    // as5600_write(AS5600_CONF_H, (uint8_t)(config >> 8));

    HAL_Delay(1); // Small delay after initialization
}
