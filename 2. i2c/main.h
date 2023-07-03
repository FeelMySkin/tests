#ifndef MAIN_H
#define MAIN_H

#include "defines.h"
#include "soft_i2c.h"


const GpioPin sda = {SDA_GPIO, SDA_PIN};
const GpioPin scl = {SCL_GPIO, SCL_PIN};

SoftI2C_handler s_i2c;

void InitPeriph();
void InitRCC();
void InitTIM();
void InitSoftI2C();
void InitGPIO();

void SetPin(const GpioPin*);
void ResetPin(const GpioPin*);
uint8_t ReadPin(const GpioPin*);

#endif