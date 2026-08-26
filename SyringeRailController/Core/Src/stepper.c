#include "stepper.h"

const uint8_t stepper_sequence[8][4] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1}
};

void Stepper_Init(Stepper_t *stepper, 
                  GPIO_TypeDef *p0, uint16_t pin0,
                  GPIO_TypeDef *p1, uint16_t pin1,
                  GPIO_TypeDef *p2, uint16_t pin2,
                  GPIO_TypeDef *p3, uint16_t pin3) 
{
    stepper->ports[0] = p0; stepper->pins[0] = pin0;
    stepper->ports[1] = p1; stepper->pins[1] = pin1;
    stepper->ports[2] = p2; stepper->pins[2] = pin2;
    stepper->ports[3] = p3; stepper->pins[3] = pin3;
    
    stepper->step_delay = 2;
    stepper->last_step_time = 0;
    stepper->step_index = 0;
    stepper->direction = 1;
    stepper->running = 0;
    
    for (int i = 0; i < 4; i++) {
        HAL_GPIO_WritePin(stepper->ports[i], stepper->pins[i], GPIO_PIN_RESET);
    }
}

void Stepper_SetSpeed(Stepper_t *stepper, uint32_t delay_ms) {
    if (delay_ms < 1) delay_ms = 1;
    stepper->step_delay = delay_ms;
}

void Stepper_SetDirection(Stepper_t *stepper, int8_t dir) {
    stepper->direction = (dir >= 0) ? 1 : -1;
}

void Stepper_Start(Stepper_t *stepper) {
    stepper->running = 1;
    stepper->last_step_time = HAL_GetTick();
}

void Stepper_Stop(Stepper_t *stepper) {
    stepper->running = 0;
    for (int i = 0; i < 4; i++) {
        HAL_GPIO_WritePin(stepper->ports[i], stepper->pins[i], GPIO_PIN_RESET);
    }
}

void Stepper_Update(Stepper_t *stepper) {
    if (!stepper->running) return;

    uint32_t current_time = HAL_GetTick();
    if ((current_time - stepper->last_step_time) >= stepper->step_delay) {
        stepper->last_step_time = current_time;
        
        for (int i = 0; i < 4; i++) {
            HAL_GPIO_WritePin(stepper->ports[i], stepper->pins[i], 
                              stepper_sequence[stepper->step_index][i] ? GPIO_PIN_SET : GPIO_PIN_RESET);
        }
        
        stepper->step_index += stepper->direction;
        if (stepper->step_index >= 8) {
            stepper->step_index = 0;
        } else if (stepper->step_index < 0) {
            stepper->step_index = 7;
        }
    }
}
