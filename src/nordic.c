#include "headers.h"
#include "spi.h"
#include <linux/ioctl.h>
#include <linux/spi/spidev.h>
#include <stdbool.h>
#include <sys/ioctl.h>

/**
 * @brief           Read data from nordic nrf52840 
 * 
 * @param spi       SPI handler
 * @param data      pointer to read coun/data 
 * @return int8_t   return 0 on success, non 0 on failure
 */
int8_t nordic_send_rcv_data(spi_t *spi, uint8_t *tx_data, uint8_t *rx_data, uint16_t len) {
    if (spi_transfer(spi, tx_data, rx_data, len)) {
        CLOG("SPI Read error: %s", spi->error.errmsg);
        return spi->error.c_errno;
    }
    ILOG("Tx: %s\r\n", tx_data);
    ILOG("Rx: %s\r\n", rx_data);
    return 0;
}

/**
 * @brief           Close the SPI driver for RTD
 * 
 * @param spi       SPI handler to be close   
 * @return int8_t   return success(0) or fail(-1)
 */
int8_t nordic_deinit(spi_t *spi) {
    DLOG("Closing SPI %s\n", spi->dev);
    if (spi_close(spi)) {
        ILOG("Fail to close the RTD SPI device\r\n");
        return -1;
    }
    return 0;
}

/**
 * @brief           initialize the SPI driver for accessing the RTD
 * 
 * @param spi_dev   string, SPI device on which RTD is connected
 * @param spi_mode  SPI mode of operation(1/3 supported by MAX31865)
 * @param freq      SPI clock frequency
 * @return spi_t*   returns a pointer to SPI handler
 */
spi_t *nordic_init(char *spi_dev, char spi_mode, int freq) {
    spi_t *spi = NULL;

    spi = spi_new();
    spi->dev = spi_dev;

    ILOG("nordic_init SPI Dev: %s, mode: %d, freq: %d\n", spi_dev, spi_mode, freq);
    if (spi_open(spi, spi_dev, spi_mode, freq)) {
        ILOG("Print error Code Here\r\n");
        return NULL;
    }
    return spi;
}

/**
 * @brief Nordic helper function
 * 
 */
void nordic_help(void) {
    CLOG("   NORD command Usage:\n");
    CLOG("       %s -c NORD \n", exec_filename);
    CLOG("          Test the NORD SPIS\n");
}
#define TEST_STRING "To Nordic"

/**
 * @brief           initialize the SPI driver for accessing the RTD
 * 
 * @param spi_dev   string, SPI device on which RTD is connected
 * @param spi_mode  SPI mode of operation(1/3 supported by MAX31865)
 * @param freq      SPI clock frequency
 * @return spi_t*   returns a pointer to SPI handler
 */
void nordic_cmd(char *spi_dev, char spi_mode, int freq) {
    spi_t *spi_nrf52840 = NULL;
    uint8_t tx_data[100] = TEST_STRING;
    uint8_t rx_data[100] = {0}; //"AQSA";
    // int err_no;

    spi_nrf52840 = nordic_init(config.nordic_spi_device, SPI_MODE_0, NORIDC_SPI_CLOCK_FREQ);

    /*err_no = */nordic_send_rcv_data(spi_nrf52840, tx_data, rx_data, 11);

    if (strcmp((const char *)rx_data, "From Nordic") == 0) {
        CLOG("Result: Nordic communication Pass\r\n");
    } else {
        CLOG("Result: Nordic communication Fail\r\n");
    }
    // *(void *)&err_no;
    nordic_deinit(spi_nrf52840);
}


