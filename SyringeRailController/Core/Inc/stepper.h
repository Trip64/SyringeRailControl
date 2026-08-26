#ifndef __STEPPER_H
#define __STEPPER_H

#include "stm32f4xx_hal.h"

extern const uint8_t stepper_sequence[8][4];

typedef struct {
    GPIO_TypeDef *ports[4];
    uint16_t pins[4];
    uint32_t step_delay;
    uint32_t last_step_time;
    int8_t step_index;
    int8_t direction;
    uint8_t running;
} Stepper_t;

void Stepper_Init(Stepper_t *stepper, 
                  GPIO_TypeDef *p0, uint16_t pin0,
                  GPIO_TypeDef *p1, uint16_t pin1,
                  GPIO_TypeDef *p2, uint16_t pin2,
                  GPIO_TypeDef *p3, uint16_t pin3);

void Stepper_SetSpeed(Stepper_t *stepper, uint32_t delay_ms);
void Stepper_SetDirection(Stepper_t *stepper, int8_t dir);
void Stepper_Start(Stepper_t *stepper);
void Stepper_Stop(Stepper_t *stepper);
void Stepper_Update(Stepper_t *stepper);

#endif // __STEPPER_H
