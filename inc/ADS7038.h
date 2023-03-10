/**
 * @file ADS7038.h
 * @author Shahid Hashmi
 * @brief 
 * @version 0.1
 * @date 2023-02-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef ADS7038_H
#define ADS7038_H

/* General */
//#define AI_MIN_ARG_REQ  2
//#define AI_MAX_ARG_REQ  2

/* Configuration */
//#define ADC_BIT_RESOLUTION          65535 // 2^16 - 1 ---> for 16 bit ADC
#define ADC_BIT_RESOLUTION          4095 // 2^12 ---> for 12 bit ADC
#define ADC_VREF                    5.0f // 

/* Command Registers */
#define ADS_NO_OPERATION    0x00
#define SINGLE_REG_READ     0x10
#define SINGLE_REG_WRITE    0x08
#define SET_BIT             0x18
#define CLEAR_BIT           0x20

/* Operation Registers */
#define SYSTEM_STATUS       0x0
#define GENERAL_CFG         0x1
#define DATA_CFG            0x2
#define PIN_CFG             0x5
#define SEQUENCE_CFG        0x10
#define CHANNEL_SEL         0x11


/* Functions */
spi_t *ads7038_init(char *spi_dev, char spi_mode, int freq);
int8_t ads7038_deinit(spi_t *spi);
int8_t ADS7038_ReadData(spi_t *spi, uint16_t *data, int samp_cnt);
int8_t ADS7038_WriteData(spi_t *spi, uint16_t *data, int samp_cnt);
//float ad7988_voltage_conv(uint16_t *data, int samp_cnt);

#endif