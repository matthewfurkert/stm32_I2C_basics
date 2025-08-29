
#ifndef __AS5600_H
#define __AS5600_H

#include "main.h"
#include "i2c.h"

// AS5600 I2C Address
#define AS5600L_ADDR        0x40<<1

// AS5600 Register Addresses
#define AS5600L_ZMCO        0x00    // ZMCO register
#define AS5600L_ZPOS_H      0x01    // Zero position high byte
#define AS5600L_ZPOS_L      0x02    // Zero position low byte
#define AS5600L_MPOS_H      0x03    // Maximum position high byte
#define AS5600L_MPOS_L      0x04    // Maximum position low byte
#define AS5600L_MANG_H      0x05    // Maximum angle high byte
#define AS5600L_MANG_L      0x06    // Maximum angle low byte
#define AS5600L_CONF_H      0x07    // Configuration high byte
#define AS5600L_CONF_L      0x08    // Configuration low byte
#define AS5600L_I2CADDR		0x20
#define AS5600L_I2CUPDT		0x21
#define AS5600L_RAW_ANGLE_H 0x0C    // Raw angle high byte
#define AS5600L_RAW_ANGLE_L 0x0D    // Raw angle low byte
#define AS5600L_ANGLE_H     0x0E    // Angle high byte
#define AS5600L_ANGLE_L     0x0F    // Angle low byte
#define AS5600L_STATUS      0x0B    // Status register
#define AS5600L_AGC         0x1A    // Automatic gain control
#define AS5600L_MAGNITUDE_H 0x1B    // Magnitude high byte
#define AS5600L_MAGNITUDE_L 0x1C    // Magnitude low byte
#define AS5600L_BURN        0xFF    // Burn command

// Status register bit definitions
#define AS5600L_STATUS_MH	0x08    // Magnet too strong
#define AS5600L_STATUS_ML	0x10    // Magnet too weak
#define AS5600L_STATUS_MD	0x20    // Magnet detected

// Configuration values
#define AS5600L_CONF_PWMF_115HZ		0x00
#define AS5600L_CONF_PWMF_230HZ		0x01
#define AS5600L_CONF_PWMF_460HZ		0x02
#define AS5600L_CONF_PWMF_920HZ		0x03

#define AS5600L_CONF_OUTS_ANALOG	0x00
#define AS5600L_CONF_OUTS_PWM		0x20

#define AS5600L_CONF_PWMF_SHIFT		0
#define AS5600L_CONF_OUTS_SHIFT		5

extern uint8_t as5600_data[2];

void as5600_retrieve_angle(void);
void as5600_retrieve_status(void);

#endif
