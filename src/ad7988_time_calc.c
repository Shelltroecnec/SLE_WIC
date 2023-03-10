
/***************************** Include Files **********************************/
#include "ad7988.h"
#include "headers.h"

/************************* Functions Definitions ******************************/

/**
 * @brief           initialize the SPI driver for accessing AD7988
 * 
 * @param spi       SPI handler
 * @param spi_dev   string, SPI device on which AD7988 is connected
 * @param spi_mode  SPI mode of operation(Mode 0 only supported by AD7988)
 * @param freq      SPI clock frequency
 * @return spi_t*   returns a pointer to SPI handler
 */
spi_t *ad7988_init(char *spi_dev, char spi_mode, int freq) {
    spi_t *spi;
    spi = spi_new();
    if (spi == NULL) {
        ILOG("Fail to create SPI device!!!\n");
        return NULL;
    }
    ILOG("ad7988_init SPI Dev: %s, mode: %d, freq: %d, %x\n", spi_dev, spi_mode, freq, spi);
    if (spi_open(spi, spi_dev, spi_mode, freq)) {
        ILOG("Print error Code Here\r\n");
        return NULL;
    }
    return spi;
}

/**
 * @brief           Close the SPI driver for AD7988
 * 
 * @param spi       SPI handler to be close   
 * @return int8_t   return success(0) or fail(-1)
 */
int8_t ad7988_deinit(spi_t *spi) {
    if (spi_close(spi)) {
        ILOG("Fail to close the RTD SPI device\r\n");
        return -1;
    }
    return 0;
}

/**
 * @brief           Read ADC sample from AD7988 
 * 
 * @param spi       SPI handler
 * @param data      pointer to read adc coun/data 
 * @return int8_t   return 0 on success, non 0 on failure
 */
int8_t AD7988_ReadData(spi_t *spi, uint16_t *data, int samp_cnt) {
    uint8_t txBuff[samp_cnt * 2];
    uint8_t rxBuff[samp_cnt * 2];

    memset(txBuff, 0, samp_cnt * 2);
    memset(rxBuff, 0, samp_cnt * 2);
    int offset = 0;

    // CLOG("Sampling Count: %d\n", samp_cnt);
    // int io_out = 0;
    // digital_pin_out_write(SELECT_RELAY_IO(1), 1);
    // uint64_t  start_time = GetTimeStamp();
    for (int samp_index = 0; samp_index < samp_cnt; (samp_index++)) {
        if (spi_transfer(spi, txBuff, rxBuff + (samp_index * 2), 2)) {
            WLOG("SPI Read error: %s", spi->error.errmsg);
            return spi->error.c_errno;
        }
        // digital_pin_out_write(SELECT_RELAY_IO(2), ~io_out);
        //usleep(3);
    }
    // uint64_t  end_time = GetTimeStamp();
    // digital_pin_out_write(SELECT_RELAY_IO(1), 0);

    // CLOG("Start time: %d, End Time: %d, diff: %d\n",start_time, end_time, end_time - start_time);
    //switching MSB-LSB
    for (int samp_index = 0; samp_index < samp_cnt; samp_index++) {
        data[samp_index] = (uint16_t)(rxBuff[samp_index * 2 + 1] | (uint16_t)(rxBuff[samp_index * 2] << 8));
    }

    return 0;
}

/**
 * @brief         Convert ADC count to voltage
 * 
 * @param data    16 bit adc count
 * @return float  converted float value to voltage
 */
float ad7988_voltage_conv(uint16_t *data, int samp_cnt) {
    /*float fAdcVoltage = 0;

  fAdcVoltage = ((float)data /  ADC_BIT_RESOLUTION * ADC_VREF);
  return fAdcVoltage;*/
}