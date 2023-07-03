#include "soft_i2c.h"


void SoftI2C_Init(SoftI2C_handler* s_i2c)
{
    s_i2c->flags.clock_pulse_high = true;
    s_i2c->flags.state = SOFT_I2C_IDLE;
}

void SoftI2C_Write(SoftI2C_handler* s_i2c)
{

}

void SoftI2C_Read(SoftI2C_handler* s_i2c)
{

}

void SoftI2C_Start(SoftI2C_handler* s_i2c)
{

}

void SoftI2C_Stop(SoftI2C_handler* s_i2c)
{

}

void SoftI2C_Tick(SoftI2C_handler* s_i2c)
{
    if(s_i2c->flags.clock_pulse_high)
    {
        s_i2c->set_low_function(s_i2c->scl_pin_ptr);
        s_i2c->flags.clock_pulse_high = false;
    }
    else
    {
        s_i2c->set_high_function(s_i2c->scl_pin_ptr);
        s_i2c->flags.clock_pulse_high = true;
    }
}
