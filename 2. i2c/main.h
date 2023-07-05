#ifndef MAIN_H
#define MAIN_H

#include "defines.h"
#include "soft_i2c.h"

/*Определения пинов для sda и scl*/
const GpioPin sda = {SDA_GPIO, SDA_PIN};
const GpioPin scl = {SCL_GPIO, SCL_PIN};

SoftI2C_handler s_i2c;

/**
 * @brief Функция включения перефирии
 * 
 */
void InitPeriph();

/**
 * @brief Функция для инициализации тактирования
 * 
 */
void InitRCC();

/**
 * @brief Функция активации таймера для использования в I2C
 * 
 */
void InitTIM();

/**
 * @brief Функция для инициализации SoftI2C
 * 
 */
void InitSoftI2C();

/**
 * @brief Функция для Инициализации GPIO
 * 
 */
void InitGPIO();

 /**
  * @brief Функция для выставления 1 на GPIO
  * 
  * @param GpioPin* указатель на GpioPin
  */
void SetPin(const GpioPin*);

/**
 * @brief Функция для выставления 0 на GPIO
  * 
  * @param GpioPin* указатель на GpioPin
 */
void ResetPin(const GpioPin*);

/**
 * @brief Функция для считывания состояния ножки
 * 
 * @param GpioPin* указатель на GpioPin
 * @return uint8_t 1 или 0, в зависимоти, что стои на пине
 */
uint8_t ReadPin(const GpioPin*);

#endif