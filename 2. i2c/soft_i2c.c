#include "soft_i2c.h"

/**
 * @brief  Функция для передачи адреса в линию I2C
 * 
 * @param s_i2c указатель на структуру SoftI2C_handler
 */
static void SoftI2C_Address(SoftI2C_handler* s_i2c)
{
    /* Если текущий уровень SCL высокий, то переключаем его вниз.*/
    if(s_i2c->flags.clock_pulse_high)
    {
        s_i2c->set_low_function(s_i2c->scl_pin_ptr);
        s_i2c->flags.clock_pulse_high = false;
        /*если мы передали 8 бит адреса, то переходим в режим ожидания ACK*/
		if(s_i2c->current_bit == 8)
		{
			s_i2c->current_bit = 0;
			s_i2c->flags.state = SOFT_I2C_WAIT_ACK;
			s_i2c->set_high_function(s_i2c->sda_pin_ptr);
			
		} 
    }
    else
    {
        /* Если уровень низкий, то записываем следующий бит адреса в SDA, и переключаем SCL вверх.*/
        if(s_i2c->addr&(1<<s_i2c->current_bit)) s_i2c->set_high_function(s_i2c->sda_pin_ptr);
        else s_i2c->set_low_function(s_i2c->sda_pin_ptr);
        s_i2c->set_high_function(s_i2c->scl_pin_ptr);
        s_i2c->flags.clock_pulse_high = true;
        /*Повышаем счётчик переданных битов*/
        s_i2c->current_bit++;
    }

}

/**
 * @brief Функция для обработки передачи данных
 * 
 * @param s_i2c указатель на структуру SoftI2C_handler
 */
static void SoftI2C_ProcessWrite(SoftI2C_handler* s_i2c)
{
    /* Если SCL=1*/
    if(s_i2c->flags.clock_pulse_high)
    {
        /*То опускаем SCL в 0*/
        s_i2c->set_low_function(s_i2c->scl_pin_ptr);
        s_i2c->flags.clock_pulse_high = false;
        /*если мы передали 8 бит адреса, то переходим в режим ожидания ACK, и, если всё передали, заканчиваем режим записи*/
        if(s_i2c->current_bit == 8)
        {
            s_i2c->current_bit = 0;
            s_i2c->current_byte++;
            if(s_i2c->current_byte == s_i2c->bytes_to_write) s_i2c->flags.writing = false;
            s_i2c->flags.state = SOFT_I2C_WAIT_ACK;
            s_i2c->set_high_function(s_i2c->sda_pin_ptr);
        } 
    }
    else
    {
        /* Если SCL=0, то записываем в SDA следующий бит*/
        if(s_i2c->to_transmit[s_i2c->current_byte]&(1<<s_i2c->current_bit)) s_i2c->set_high_function(s_i2c->sda_pin_ptr);
        else s_i2c->set_low_function(s_i2c->sda_pin_ptr);
        s_i2c->set_high_function(s_i2c->scl_pin_ptr);
        s_i2c->flags.clock_pulse_high = true;
        s_i2c->current_bit++;
    }
    
}

/**
 * @brief Функция для обработки приёма данных
 * 
 * @param s_i2c указатель на структуру SoftI2C_handler
 */
static void SoftI2C_ProcessRead(SoftI2C_handler* s_i2c)
{
    /*Если SCL=1*/
    if(s_i2c->flags.clock_pulse_high)
    {
        /* Считываем байт с линии SDA, и опускаем SCL в 0*/
        s_i2c->to_receive[s_i2c->current_byte]<<=1;
        s_i2c->to_receive[s_i2c->current_byte]|=s_i2c->read_pin_function(s_i2c->sda_pin_ptr);
        s_i2c->set_low_function(s_i2c->scl_pin_ptr);
        s_i2c->flags.clock_pulse_high = false;
        /*Отсчитываем биты*/
        s_i2c->current_bit++;
    }
    else
    {
        /*Если SCL=0, то поднимаем линию наверх*/
        s_i2c->set_high_function(s_i2c->scl_pin_ptr);
        s_i2c->flags.clock_pulse_high = true;
    }

    /*Как только приняли 8 бит, переходим в режим ожидания подтверждения, и, если всё вычитали, заканчиваем приём*/
    if(s_i2c->current_bit == 8)
    {
        s_i2c->current_bit = 0;
        s_i2c->current_byte++;
        if(s_i2c->current_byte == s_i2c->bytes_to_read)s_i2c->flags.reading = false;
        else s_i2c->to_receive[s_i2c->current_byte] = 0;
        s_i2c->flags.state = SOFT_I2C_WAIT_ACK;
    }
}

/**
 * @brief Функция для обработки подтверждения от ведомого устройства
 * 
 * @param s_i2c указатель на структуру SoftI2C_handler
 */
static void SoftI2C_Ack(SoftI2C_handler* s_i2c)
{
    /*Сбрасываем текущий указатель на бит*/
    s_i2c->current_bit = 0;

    /*Если SCL=1*/
    if(s_i2c->flags.clock_pulse_high)
    {
        /*Смотрим, если SDA=0, то устройство подтвердилось (ACK), если нет - то NACK*/
        if(s_i2c->read_pin_function(s_i2c->sda_pin_ptr) == 1) s_i2c->flags.ack = false; //NACK
        else s_i2c->flags.ack = true;//ACK

        /*Опускаем SCL в 0*/
        s_i2c->set_low_function(s_i2c->scl_pin_ptr);
        s_i2c->flags.clock_pulse_high = false;
		
        /*Если NACK, то заканчиваем обмен, и генерируем СТОП*/
		if(!s_i2c->flags.ack)
		{
			s_i2c->set_low_function(s_i2c->sda_pin_ptr);
            s_i2c->flags.state = SOFT_I2C_GENERATE_STOP;
			s_i2c->flags.reading = false;
			s_i2c->flags.writing = false;
		}
        /*Иначе продолжаем работу с чтением, записью, или заканчиваем обмен*/
        else if(s_i2c->flags.reading) s_i2c->flags.state = SOFT_I2C_READING;
        else if(s_i2c->flags.writing) s_i2c->flags.state = SOFT_I2C_WRITING;
        else
        {
            s_i2c->set_low_function(s_i2c->sda_pin_ptr);
            s_i2c->flags.state = SOFT_I2C_GENERATE_STOP;
        }
    }
    else
    {
        /*Если SCL=0, то поднимаем его в 1*/
        s_i2c->set_high_function(s_i2c->scl_pin_ptr);
        s_i2c->flags.clock_pulse_high = true;
    }
}

/**
 * @brief Функция для генерации Стартового сигнала
 * 
 * @param s_i2c указатель на структуру SoftI2C_handler
 */
static void SoftI2C_Start(SoftI2C_handler* s_i2c)
{
    /*Если SCL=1, то опускаем SDA в 0*/
	if(s_i2c->flags.clock_pulse_high)
	{
		s_i2c->set_low_function(s_i2c->sda_pin_ptr);
		s_i2c->flags.state = SOFT_I2C_ADDRESS;
	}
	else
	{
        /*Иначе, поднимаем SCL в 1*/
		s_i2c->set_high_function(s_i2c->scl_pin_ptr);
		s_i2c->flags.clock_pulse_high = true;
	}
}

/**
 * @brief Функция для генерации стопового сигнала
 * 
 * @param s_i2c указатель на структуру SoftI2C_handler
 */
static void SoftI2C_Stop(SoftI2C_handler* s_i2c)
{
    /*Если SCL в 1, то поднимаем SDA в 1 и переходим в режим ожидания*/
	if(s_i2c->flags.clock_pulse_high)
    {
        s_i2c->set_high_function(s_i2c->sda_pin_ptr);
		s_i2c->flags.state = SOFT_I2C_IDLE;
    }
	else
	{
    /*Если SCL в 0, то поднимаего его в 1,а SDA опускаем в 0*/
        s_i2c->set_high_function(s_i2c->scl_pin_ptr);
        s_i2c->set_low_function(s_i2c->sda_pin_ptr);
		s_i2c->flags.clock_pulse_high = true;
	}
}




void SoftI2C_Init(SoftI2C_handler* s_i2c)
{
    /*Сбрасываем все флаги и переходим в режим ожидания*/
    s_i2c->flags.clock_pulse_high = true;
    s_i2c->flags.state = SOFT_I2C_IDLE;
    s_i2c->flags.reading = false;
    s_i2c->flags.writing = false;
    s_i2c->bytes_to_read = 0;
    s_i2c->bytes_to_write = 0;
    s_i2c->current_byte = 0;
    s_i2c->set_high_function(s_i2c->sda_pin_ptr);
    s_i2c->set_high_function(s_i2c->scl_pin_ptr);
}

void SoftI2C_Write(SoftI2C_handler* s_i2c)
{
    /*Если текущий режим не IDLE, то возвращаемся*/
    if(s_i2c->flags.state != SOFT_I2C_IDLE) return;

    /*Выставляем адрес в пежим I2C (смещаем и ставим флаг записи)*/
    s_i2c->addr = (s_i2c->addr<<1) | WRITE_BIT;

    /*сбрасываем и выставляем нужные флаги, и переходим в режим старта*/
    s_i2c->current_bit = 0;
    s_i2c->current_byte = 0;
    s_i2c->flags.writing = true;
    s_i2c->flags.reading = false;
    s_i2c->flags.state = SOFT_I2C_GENERATE_START;
}

void SoftI2C_Read(SoftI2C_handler* s_i2c)
{
    /*Если текущий режим не IDLE, то возвращаемся*/
    if(s_i2c->flags.state != SOFT_I2C_IDLE) return;
    
    /*Выставляем адрес в пежим I2C (смещаем и ставим флаг чтения)*/
    s_i2c->addr = (s_i2c->addr<<1) | READ_BIT;
    
    /*сбрасываем и выставляем нужные флаги, и переходим в режим старта*/
    s_i2c->to_receive[0] = 0;
    s_i2c->current_bit = 0;
    s_i2c->current_byte = 0;
    s_i2c->flags.writing = false;
    s_i2c->flags.reading = true;
    s_i2c->flags.state = SOFT_I2C_GENERATE_START;
}

void SoftI2C_Tick(SoftI2C_handler* s_i2c)
{
    /*Переключаемся между текущими состояниями, и выполняем нужные действия*/
    switch(s_i2c->flags.state)
    {
        case SOFT_I2C_GENERATE_START:
            SoftI2C_Start(s_i2c);
        break;

        case SOFT_I2C_ADDRESS:
            SoftI2C_Address(s_i2c);
        break;

        case SOFT_I2C_READING:
            SoftI2C_ProcessRead(s_i2c);
        break;

        case SOFT_I2C_WRITING:
            SoftI2C_ProcessWrite(s_i2c);
        break;

        case SOFT_I2C_WAIT_ACK:
           SoftI2C_Ack(s_i2c);
        break;

        case SOFT_I2C_GENERATE_STOP:
            SoftI2C_Stop(s_i2c);
        break;

        case SOFT_I2C_IDLE:
        break;

    }
}
