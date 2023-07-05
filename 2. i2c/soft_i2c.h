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

/*I2C Flags for Read/Write*/
#define READ_BIT    1
#define WRITE_BIT   0

/*I2C состояния*/
enum SoftI2C_States
{
    SOFT_I2C_IDLE, /*Линия находится в режиме ожидания*/
    SOFT_I2C_GENERATE_START, /*Линия находится в режиме генерации стартового пульса*/
    SOFT_I2C_ADDRESS, /* Линия находится в режиме передачи адреса*/
    SOFT_I2C_WRITING, /* Линия находится в режиме передачи данных*/
    SOFT_I2C_READING, /* Линия находится в режиме чтения данных*/
    SOFT_I2C_GENERATE_STOP, /*Линия находится в режиме генерации остановки*/
    SOFT_I2C_WAIT_ACK /* Линия находится в режиме ожидания подтверждения от ведомого устройства (ACK)*/
};

/*I2C флаги (и состояния)*/
typedef struct
{
    enum SoftI2C_States state:3; /*Состояние линии*/
    bool clock_pulse_high:1; /*Состояние SCL линии. Если true, то SCL=1*/
    bool reading:1; /*Текущая задача - чтение*/
    bool writing:1; /*Текущая задача - запись*/
    bool ack:1; /*ПОявилось ли подтверждение от ведомого устройства*/
}SoftI2C_Flags;

/*Основная структура для  I2C*/
typedef struct 
{
    void (*set_high_function)(void*); /*Указатель на функцию установки 1 на ножку*/
    void (*set_low_function)(void*); /*Указатель на функцию установки 0 на ножку*/
    uint8_t (*read_pin_function)(void*); /*Указатель на функцию Чтения пина*/
    void* scl_pin_ptr; /*Указатель на ножку scl, которая передаётся в функции (может быть чем угодно)*/
    void* sda_pin_ptr;/*Указатель на ножку sda, которая передаётся в функции (может быть чем угодно)*/

    SoftI2C_Flags flags; /*Флаги*/

    uint8_t to_transmit[10]; /*байты на передачу*/
    uint8_t to_receive[10]; /*Байты на приём*/
    uint8_t bytes_to_read; /*Количество байт на чтение*/
    uint8_t bytes_to_write; /*Количество байт на запись*/
    uint8_t current_byte; /*Текущий записанный/считанный байт*/
    uint8_t addr; /*Байт адреса (без смещения)*/
    uint8_t current_bit; /*Текущий бит для приёма-передачи*/
}SoftI2C_handler;

/**
 * @brief Инициализация структуры
 * 
 * @param s_i2c указатель на структуру SoftI2C_handler
 */
void SoftI2C_Init(SoftI2C_handler* s_i2c);

/**
 * @brief  передача данных в динию I2C. 
 * Предварительно следует записать в handler байты to_transmit и addr
 * 
 * @param s_i2c указатель на структуру SoftI2C_handler
 */
void SoftI2C_Write(SoftI2C_handler* s_i2c);

/**
 * @brief  приём данных с линии I2C. 
 * Предварительно следует записать в handler байт addr
 * 
 * @param s_i2c указатель на структуру SoftI2C_handler
 */
void SoftI2C_Read(SoftI2C_handler* s_i2c);

/**
 * @brief Основной обработчик, который следует вызывать с частотой 2*I2C_FREQUENCY
 * 
 * @param s_i2c указатель на структуру SoftI2C_handler
 */
void SoftI2C_Tick(SoftI2C_handler* s_i2c);


#endif