#ifndef __ADXL_H
#define __ADXL_H

#include "main.h"
#include "i2c.h"

#define DEVICE_ADDR		0x53<<1
#define DEVID_R			0x00
#define POWER_CTL_R		0x2D
#define DATA_FORMAT_R	0x31
#define FOUR_G			0x01
#define RESET			0x00
#define SET_MEASURE_B	0x08
#define DATA_START_ADDR	0x32
#define FOUR_G_SCALE_FACTOR 0.0078

extern uint8_t data_rec[6];
extern uint8_t device_id;

#endif
