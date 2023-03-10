#ifndef MAX31865_H
#define MAX31865_H

#include <stdint.h>
#include "spi.h"
#include "rtd_cli.h"

/*----------------------------------MACRO Starts-------------------------------------*/


//RTD defination
//Registers defined in Table 1 on page 12 of the RTD data sheet
#define WRITE_CONFIG_ADDR               0x80 
#define READ_CONFIG_ADDR                0x00
#define WRITE_HFT_MSB_ADDR              0x83 //High Fault Threshold MSB
#define WRITE_HFT_LSB_ADDR              0x84 //High Fault Threshold LSB
#define READ_HFT_MSB_ADDR               0x03
#define READ_HFT_LSB_ADDR               0x04
#define WRITE_LFT_MSB_ADDR              0x85
#define WRITE_LFT_LSB_ADDR              0x86
#define READ_LFT_MSB_ADDR               0x05   
#define READ_LFT_LSB_ADDR               0x06
#define FAULT_STATUS_ADDR               0X07
            
#define READ_RTD_MSB_ADDR               0x01
#define READ_RTD_LSB_ADDR               0x02

#define MAX_RTD_TEMP_LIMIT              500 
#define MIN_RTD_TEMP_LIMIT              -300
#define CLI_STRING_SIZE                 100

///RTD conversion related MACROS
#define ADC_BIT_CONV_COUNT              (float)32767
#define RTD_REF_RESISTOR                (float)400 // Reference Resistor(400 ohms) installed on the board.
#define RTD_RES_ZERO_DEGREE             100 // RTD Resistance at 0 Degrees.

#define RTD_ALPHA                       0.00390830
#define RTD_BETA                        -0.0000005775
#define RTD_GAMMA                       -0.00000000000418301


#define MAX31865_SPI_CLOCK_FREQ         1000000

/*----------------------------------MACRO Ends-------------------------------------*/


/*----------------------------Function Prototype starts------------------------------*/

extern st_rtd_sample_t st_rtd_read;

int8_t rtd_read_byte (spi_t *, uint8_t ui8Register, uint8_t *pbuf);
int8_t rtd_write_byte(spi_t *, uint8_t ui8Register, uint8_t data);
int8_t rtd_read_bytes(spi_t *spi, uint8_t ucRegAddr, uint8_t *puReadBuff, uint8_t size);
int8_t rtd_write_bytes(spi_t *spi, uint8_t ucRegAddr, uint8_t *write_bytes, uint8_t size);


spi_t *max31865_init(char *spi_dev, char spi_mode, int freq);
int8_t max31865_deinit(spi_t *spi);


void fault_error_spec(uint8_t fault);
uint8_t rtd_read_fault_status_reg(spi_t *);
void rtd_temp_read(spi_t *spi, st_rtd_sample_t *pst_rtd_read, uint8_t);
uint8_t rtd_read(uint8_t rtd_index, float* rtd_value);
extern void init_adc_dac_rtd_spim(uint8_t snsr_type);
uint8_t rtd_sensor_scan(spi_t *, st_rtd_sample_t *, int);
float rtd_calculate_temp(uint16_t rtd_adc_count);
extern uint8_t mote_type;
extern uint16_t RtdSampRate;

/*----------------------------Function Prototype ends------------------------------*/

#endif