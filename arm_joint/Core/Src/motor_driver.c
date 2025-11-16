/*
 * motor_driver.c
 *
 *  Created on: Nov 2, 2025
 *      Author: matthew
 */

#include "motor_driver.h"

uint8_t motor_direction = 0;
uint8_t motor_speed = 0;

void set_motor_values(void) {
	motor_direction = update_motor_direction();
	motor_speed = update_motor_speed();

	if (!motor_direction) {
		TIM2->CCR3 = motor_speed * 10;
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
		TIM2->CCR4 = motor_speed * 10;
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
	}

	else if (motor_direction ==1) {
		TIM2->CCR3 = motor_speed * 10;
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
		TIM2->CCR4 = motor_speed * 10;
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);
	}
	else if (motor_direction ==2) {
		HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
		HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
	}

}
