#include "app.h"

Stepper_t rail_stepper;
Stepper_t syringe_stepper;

static volatile uint16_t adc_pot_rail = 2048;
static volatile uint16_t adc_pot_syringe = 2048;

#define BTN_PRESSED GPIO_PIN_RESET  

static inline uint32_t map_adc_to_delay(uint16_t adc_val) {
    return 1 + ((adc_val * 19) / 4095);
}

void App_Init(void) {
    Stepper_Init(&rail_stepper, 
                 MOTOR_RAIL_P0_PORT, MOTOR_RAIL_P0_PIN,
                 MOTOR_RAIL_P1_PORT, MOTOR_RAIL_P1_PIN,
                 MOTOR_RAIL_P2_PORT, MOTOR_RAIL_P2_PIN,
                 MOTOR_RAIL_P3_PORT, MOTOR_RAIL_P3_PIN);
                 
    Stepper_Init(&syringe_stepper, 
                 MOTOR_SYR_P0_PORT, MOTOR_SYR_P0_PIN,
                 MOTOR_SYR_P1_PORT, MOTOR_SYR_P1_PIN,
                 MOTOR_SYR_P2_PORT, MOTOR_SYR_P2_PIN,
                 MOTOR_SYR_P3_PORT, MOTOR_SYR_P3_PIN);
}

void App_UpdateADC(uint16_t rail_pot_val, uint16_t syringe_pot_val) {
    adc_pot_rail = rail_pot_val;
    adc_pot_syringe = syringe_pot_val;
}

void App_Run(void) {
    Stepper_SetSpeed(&rail_stepper, map_adc_to_delay(adc_pot_rail));
    Stepper_SetSpeed(&syringe_stepper, map_adc_to_delay(adc_pot_syringe));

    int8_t rail_dir = (HAL_GPIO_ReadPin(SW_RAIL_DIR_PORT, SW_RAIL_DIR_PIN) == BTN_PRESSED) ? 1 : -1;
    int8_t syr_dir  = (HAL_GPIO_ReadPin(SW_SYR_DIR_PORT, SW_SYR_DIR_PIN)   == BTN_PRESSED) ? 1 : -1;

    Stepper_SetDirection(&rail_stepper, rail_dir);
    Stepper_SetDirection(&syringe_stepper, syr_dir);

    int rail_ffw_l = (HAL_GPIO_ReadPin(BTN_RAIL_FFW_L_PORT, BTN_RAIL_FFW_L_PIN) == BTN_PRESSED);
    int rail_ffw_r = (HAL_GPIO_ReadPin(BTN_RAIL_FFW_R_PORT, BTN_RAIL_FFW_R_PIN) == BTN_PRESSED);
    int lim_rail_l = (HAL_GPIO_ReadPin(SW_RAIL_LIM_L_PORT, SW_RAIL_LIM_L_PIN) == BTN_PRESSED);
    int lim_rail_r = (HAL_GPIO_ReadPin(SW_RAIL_LIM_R_PORT, SW_RAIL_LIM_R_PIN) == BTN_PRESSED);
    
    if ((rail_dir == 1 && lim_rail_r) || (rail_dir == -1 && lim_rail_l) || 
        (rail_ffw_r && lim_rail_r) || (rail_ffw_l && lim_rail_l)) {
        Stepper_Stop(&rail_stepper);
    } else {
        if (rail_ffw_l) {
            Stepper_SetSpeed(&rail_stepper, 1);
            Stepper_SetDirection(&rail_stepper, -1);
            Stepper_Start(&rail_stepper);
        } else if (rail_ffw_r) {
            Stepper_SetSpeed(&rail_stepper, 1);
            Stepper_SetDirection(&rail_stepper, 1);
            Stepper_Start(&rail_stepper);
        } else {
            if (HAL_GPIO_ReadPin(BTN_RAIL_START_PORT, BTN_RAIL_START_PIN) == BTN_PRESSED) {
                Stepper_Start(&rail_stepper);
            }
            if (HAL_GPIO_ReadPin(BTN_RAIL_STOP_PORT, BTN_RAIL_STOP_PIN) == BTN_PRESSED) {
                Stepper_Stop(&rail_stepper);
            }
        }
    }

    int lim_syr_t = (HAL_GPIO_ReadPin(SW_SYR_LIM_T_PORT, SW_SYR_LIM_T_PIN) == BTN_PRESSED);
    int lim_syr_b = (HAL_GPIO_ReadPin(SW_SYR_LIM_B_PORT, SW_SYR_LIM_B_PIN) == BTN_PRESSED);

    if ((syr_dir == 1 && lim_syr_t) || (syr_dir == -1 && lim_syr_b)) {
        Stepper_Stop(&syringe_stepper);
    } else {
        if (HAL_GPIO_ReadPin(BTN_SYR_START_PORT, BTN_SYR_START_PIN) == BTN_PRESSED) {
            Stepper_Start(&syringe_stepper);
        }
        if (HAL_GPIO_ReadPin(BTN_SYR_STOP_PORT, BTN_SYR_STOP_PIN) == BTN_PRESSED) {
            Stepper_Stop(&syringe_stepper);
        }
    }

    Stepper_Update(&rail_stepper);
    Stepper_Update(&syringe_stepper);
}
