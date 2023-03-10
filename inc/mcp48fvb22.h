#ifndef MCP48fvb22_H
#define MCP48fvb22_H

//#define mcp48fdev  "/dev/spidev6.1"

#define DAC_WRITE_CMD                                0x00    
#define DAC_READ_CMD                                 0x06    

/* DAC BIT MASK */
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


#define NUM_BYTES_IN_ONE_CYCLE                  3
#define MCP48F_BITS_RES                         4095
#define MCP48F_REF_VOLT                         5


#define ST_RESULT_IGNORE                        0
#define ST_RESULT_PASS                          1
#define ST_RESULT_FAIL                          2


spi_t *mcp48f_dac_init(char *spi_dev, char spi_mode, int freq);
int mcp48f_dac_deinit(spi_t *spi);
int mcp48f_dac_read_write(spi_t *spi, uint8_t address, uint8_t operation, uint16_t *data);
int mcp48f_dac_set_output(spi_t *spi, uint8_t channel, float voltage);
int mcp48f_dac_set_output_cnt(spi_t *spi, uint8_t channel, uint16_t data);

#endif