#ifndef __APP_H
#define __APP_H

#include "stm32f4xx_hal.h"
#include "stepper.h"

#define BTN_RAIL_START_PORT    GPIOE
#define BTN_RAIL_START_PIN     GPIO_PIN_0

#define BTN_RAIL_STOP_PORT     GPIOE
#define BTN_RAIL_STOP_PIN      GPIO_PIN_1

#define SW_RAIL_DIR_PORT       GPIOE
#define SW_RAIL_DIR_PIN        GPIO_PIN_2

#define BTN_RAIL_FFW_L_PORT    GPIOE
#define BTN_RAIL_FFW_L_PIN     GPIO_PIN_3

#define BTN_RAIL_FFW_R_PORT    GPIOE
#define BTN_RAIL_FFW_R_PIN     GPIO_PIN_4

#define SW_RAIL_LIM_L_PORT     GPIOE
#define SW_RAIL_LIM_L_PIN      GPIO_PIN_5

#define SW_RAIL_LIM_R_PORT     GPIOE
#define SW_RAIL_LIM_R_PIN      GPIO_PIN_6

#define BTN_SYR_START_PORT     GPIOE
#define BTN_SYR_START_PIN      GPIO_PIN_7

#define BTN_SYR_STOP_PORT      GPIOE
#define BTN_SYR_STOP_PIN       GPIO_PIN_8

#define SW_SYR_DIR_PORT        GPIOE
#define SW_SYR_DIR_PIN         GPIO_PIN_9

#define SW_SYR_LIM_T_PORT      GPIOE
#define SW_SYR_LIM_T_PIN       GPIO_PIN_10

#define SW_SYR_LIM_B_PORT      GPIOE
#define SW_SYR_LIM_B_PIN       GPIO_PIN_11

#define MOTOR_RAIL_P0_PORT     GPIOD
#define MOTOR_RAIL_P0_PIN      GPIO_PIN_0
#define MOTOR_RAIL_P1_PORT     GPIOD
#define MOTOR_RAIL_P1_PIN      GPIO_PIN_1
#define MOTOR_RAIL_P2_PORT     GPIOD
#define MOTOR_RAIL_P2_PIN      GPIO_PIN_2
#define MOTOR_RAIL_P3_PORT     GPIOD
#define MOTOR_RAIL_P3_PIN      GPIO_PIN_3

#define MOTOR_SYR_P0_PORT      GPIOD
#define MOTOR_SYR_P0_PIN       GPIO_PIN_4
#define MOTOR_SYR_P1_PORT      GPIOD
#define MOTOR_SYR_P1_PIN       GPIO_PIN_5
#define MOTOR_SYR_P2_PORT      GPIOD
#define MOTOR_SYR_P2_PIN       GPIO_PIN_6
#define MOTOR_SYR_P3_PORT      GPIOD
#define MOTOR_SYR_P3_PIN       GPIO_PIN_7

void App_Init(void);
void App_Run(void);
void App_UpdateADC(uint16_t rail_pot_val, uint16_t syringe_pot_val);

#endif // __APP_H
