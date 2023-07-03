#include "main.h"

void I2C_TIM_Handler()
{
    LL_TIM_ClearFlag_UPDATE(I2C_TIM);
    SoftI2C_Tick(&s_i2c);
}

int main()
{
    InitPeriph();
    InitRCC();
    InitGPIO();
    InitSoftI2C();
    InitTIM();

    while(1)
    {
        //__wfi();
    }
}





void InitPeriph()
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA | LL_AHB1_GRP1_PERIPH_GPIOC);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2 | LL_APB1_GRP1_PERIPH_PWR);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM9);
}

void InitRCC()
{
    LL_RCC_HSI_Enable();
    while(!LL_RCC_HSI_IsReady()) __nop();
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI,LL_RCC_PLLM_DIV_8,84,LL_RCC_PLLP_DIV_2);
    LL_RCC_PLL_Enable();
    while(!LL_RCC_PLL_IsReady()) __nop();

    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) __nop();

    SystemCoreClockUpdate();
}

void InitTIM()
{
    //200kHz Timer (2 * 100kHz)
    LL_TIM_InitTypeDef tim;
    tim.Autoreload = SystemCoreClock/1600000;
    tim.Prescaler = 0;
    tim.CounterMode = LL_TIM_COUNTERMODE_UP;
    tim.RepetitionCounter = 0;
    tim.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(I2C_TIM, &tim);

    LL_TIM_ClearFlag_UPDATE(I2C_TIM);
    LL_TIM_EnableIT_UPDATE(I2C_TIM);

    NVIC_EnableIRQ(I2C_TIM_IRQn);
    LL_TIM_EnableCounter(I2C_TIM);

}

void InitGPIO()
{
    LL_GPIO_InitTypeDef gpio;
    gpio.Mode = LL_GPIO_MODE_OUTPUT;
    gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio.Pin = sda.pin;
    gpio.Pull = LL_GPIO_PULL_UP;
    gpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    LL_GPIO_Init(sda.port,&gpio);

    
    gpio.Pin = scl.pin;
    LL_GPIO_Init(scl.port,&gpio);
}

void InitSoftI2C()
{
    s_i2c.scl_pin_ptr = (void*)(&scl);
    s_i2c.sda_pin_ptr = (void*)(&sda);
    s_i2c.set_high_function = &SetPin;
    s_i2c.set_low_function = &ResetPin;
    s_i2c.read_pin_function = &ReadPin;
    
    SoftI2C_Init(&s_i2c);
}


uint8_t ReadPin(const GpioPin *pin)
{
    if(LL_GPIO_ReadInputPort(pin->port)& pin->pin) return 1;
    else return 0;
}

void SetPin(const GpioPin *pin)
{
    LL_GPIO_SetOutputPin(pin->port, pin->pin);
}

void ResetPin(const GpioPin *pin)
{
    LL_GPIO_ResetOutputPin(pin->port, pin->pin);
}