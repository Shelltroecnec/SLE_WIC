#ifndef LTC2634_H
#define LTC2634_H

/* Write and Update Command, Power Down and Internal/External Reference voltage */
#define DAC_WRITE_CMD                                0x0    
#define DAC_UPDATE_WRITE_CMD                         0x1
#define DAC_WRITE_AND_UPDATE_ALL                     0x2
#define DAC_WRITE_AND_UPDATE_CMD                     0x3
#define POWER_DOWN_DAC                               0x4
#define POWER_DOWN_ALL_DAC                           0x5
#define SELECT_INTERNAL_REFERENCE                    0x6
#define SELECT_EXTERNAL_REFERENCE                    0x7
#define NO_OPERATION                                 0xF

/* Address for DAC Selection, with that you also select respective Register */
#define ADDRESS_DAC_A                                0x0
#define ADDRESS_DAC_B                                0x1
#define ADDRESS_DAC_C                                0x2
#define ADDRESS_DAC_D                                0x3
#define ADDRESS_DAC_ALL                              0xF

//#define DAC_READ_CMD                                 0x06    
/*
#define DAC_CMDERROR_BIT_MASK                        0x01    

//DAC Volatile Register ADDR  
#define DAC_DATA0_REG_ADDR                            0x00    //00000 
#define DAC_DATA1_REG_ADDR                            0x01    //00001
#define DAC_VREF_REG_ADDR                             0x08    //01000
#define DAC_PWR_DOWN_REG_ADDR                         0x09    //01001
#define DAC_GAIN_REG_ADDR                             0x0A    //01010

//DAC Non-volatile Register ADDR  --- not used
#define DAC_NV_DATA0_REG_ADDR                         0x10    //00000 
#define DAC_NV_DATA1_REG_ADDR                         0x11    //00001
#define DAC_NV_VREF_REG_ADDR                          0x18    //01000
#define DAC_NV_PWR_DOWN_REG_ADDR                      0x19    //01010000
#define DAC_NV_GAIN_REG_ADDR                          0x1A    //01010000
*/

#define NUM_BYTES_IN_ONE_CYCLE                   3
#define LTC2634_BITS_RES                         4095
#define LTC2634_REF_VOLT                         5


#define ST_RESULT_IGNORE                        0
#define ST_RESULT_PASS                          1
#define ST_RESULT_FAIL                          2


spi_t *ltc2634_dac_init(char *spi_dev, char spi_mode, int freq);
int ltc2634_dac_deinit(spi_t *spi);
int ltc2634_dac_write(spi_t *spi, uint8_t address, uint8_t operation, uint16_t *data);
int ltc2634_dac_set_output(spi_t *spi, uint8_t channel, float voltage);
int ltc2634_dac_set_output_cnt(spi_t *spi, uint8_t channel, uint16_t data);

#endif