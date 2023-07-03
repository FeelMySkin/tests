#ifndef DEFINES_H
#define DEFINES_H

#include "stm32f4xx.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_system.h"
#include "stdint.h"
#include "stdbool.h"

#define SCL_GPIO            GPIOC
#define SCL_PIN             LL_GPIO_PIN_14
#define SDA_GPIO            GPIOA
#define SDA_PIN             LL_GPIO_PIN_1

#define I2C_TIM             TIM9
#define I2C_TIM_Handler     TIM1_BRK_TIM9_IRQHandler
#define I2C_TIM_IRQn        TIM1_BRK_TIM9_IRQn

typedef struct
{
    GPIO_TypeDef*   port;
    uint32_t        pin;
}GpioPin;

#endif