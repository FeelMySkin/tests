#ifndef SOFT_I2C_H
#define SOFT_I2C_H


/*
    SCL and SDA - OPEN DRAIN OUTPUT
    Start Condition - SCL High, SDA High-to-Low
    Bit reads on fronts, bit sets on backs
    Stop Condition - SCL High, SDA Low-to-High
    ACK - SDA low on 9th impulse
*/

#include "defines.h"

enum SoftI2C_States
{
    SOFT_I2C_IDLE,
    SOFT_I2C_WRITING,
    SOFT_I2C_READING
};

typedef struct
{
    enum SoftI2C_States state;
    bool clock_pulse_high:1;
}SoftI2C_state_machine;

typedef struct 
{
    void (*set_high_function)(void*);
    void (*set_low_function)(void*);
    void (*read_pin_function)(void*);
    void* scl_pin_ptr;
    void* sda_pin_ptr;

    SoftI2C_state_machine flags;

    uint8_t a;
}SoftI2C_handler;

void SoftI2C_Init(SoftI2C_handler* s_i2c);
void SoftI2C_Write(SoftI2C_handler* s_i2c);
void SoftI2C_Read(SoftI2C_handler* s_i2c);
void SoftI2C_Start(SoftI2C_handler* s_i2c);
void SoftI2C_Stop(SoftI2C_handler* s_i2c);
void SoftI2C_Tick(SoftI2C_handler* s_i2c);


#endif