/**
 * @file ad7988.h
 * @author Fakhruddin Khan (f.khan@acevin.com)
 * @brief 
 * @version 0.1
 * @date 2021-08-11
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef AD7988_H
#define AD7988_H

#include <stdint.h>         /* C99 types */
#include "spi.h"

#define ADC_BIT_RESOLUTION          65535 // 2^16 - 1 ---> for 16 bit ADC
#define ADC_VREF                    5.0f // 


extern spi_t *ad7988_init(char *spi_dev, char spi_mode, int freq);
extern int8_t ad7988_deinit(spi_t *spi);
extern int8_t AD7988_ReadData(spi_t *spi, uint16_t *data, int samp_cnt);
extern float ad7988_voltage_conv(uint16_t *data, int samp_cnt);

#endif