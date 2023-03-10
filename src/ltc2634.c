#include "headers.h"

/**
 * @brief           initialize the SPI driver for accessing LTC2634
 * 
 * @param spi       SPI handler
 * @param spi_dev   string, SPI device on which AD7988 is connected
 * @param spi_mode  SPI mode of operation(Mode 0 only supported by AD7988)
 * @param freq      SPI clock frequency
 * @return spi_t*   returns a pointer to SPI handler
 */
spi_t *ltc2634_dac_init(char *spi_dev, char spi_mode, int freq) {
    spi_t *spi = spi_new();
    ILOG("LTC2634_dac_init: SPI Dev: %s, mode: %d, freq: %d\n", config.ltc2634_spi_device, spi_mode, freq);
    if (spi_open(spi,config.ltc2634_spi_device, spi_mode, freq)) {
        ILOG("Print error Code Here\r\n");
        return NULL;
    }
    return spi;
}

/**
 * @brief           Close the SPI driver for LTC2634
 * 
 * @param spi       SPI handler to be close   
 * @return int      return success(0) or fail(-1)
 */
int ltc2634_dac_deinit(spi_t *spi) {
    if (spi_close(spi)) {
        ILOG("Fail to close the LTC2634 SPI device\r\n");
        return -1;
    }
    return 0;
}

/**
 * @brief              This function allow to write or read 2 bytes to and from LTC2634
 * 
 * @param spi           SPI handler
 * @param address       Register addres, address shuold be 5-bit only 
 * @param operation     Write(b'000) or read (b'110) operation
 * @param data          pointer to 2 bytes for reading or writing
 * @return int          read/write status, 0 -> success, -1 -> failure
 */
int ltc2634_dac_read_write(spi_t *spi, uint8_t address, uint8_t operation, uint16_t *data) {
    // uint8_t count = 0;
    uint8_t rx_buff[NUM_BYTES_IN_ONE_CYCLE];
    uint8_t tx_buff[NUM_BYTES_IN_ONE_CYCLE];
    char* p;
    memset(rx_buff, 0, NUM_BYTES_IN_ONE_CYCLE);

    if (!(operation == DAC_WRITE_CMD || operation == DAC_UPDATE_WRITE_CMD || operation == DAC_WRITE_AND_UPDATE_ALL || operation == DAC_WRITE_AND_UPDATE_CMD)) {
        ILOG("[DAC] Invalid DAC Operation %d\n", operation);
        return -1;
    }

    tx_buff[0] = ((operation << 4) | (address & 0x0F));
    p = &data;
    tx_buff[1] = *p;
    tx_buff[2] = *(p+1);
    ILOG("[MCP] data: %x, %x, %x", *data, data[0], data[1]);
    ILOG("SPI OUT[0]: %d\n", tx_buff[0]);
    ILOG("SPI OUT[1]: %d\n", tx_buff[1]);
    ILOG("SPI OUT[2]: %d\n", tx_buff[2]);

    if (spi_transfer(spi, tx_buff, rx_buff, NUM_BYTES_IN_ONE_CYCLE)) {
        ILOG("SPI Read error: %s", spi->error.errmsg);
        return spi->error.c_errno;
    }
    //if (rx_buff[0] & DAC_CMDERROR_BIT_MASK) { //Valid read cycle
    //    data[0] = rx_buff[2];
    //    data[1] = rx_buff[1];
    //    return 0; // success
    //}
    return -1; //failure
}

/**
 * @brief           set the DAC output channel with the given voltage value
 * 
 * @param spi       SPI handler
 * @param channel   DAC Ouput channel, 0 -> channel 0, 1 -> channel 1
 * @param voltage   output voltage
 * @return int      status, 0 -> Success, -1 -> failure
 */

int ltc2634_dac_set_output(spi_t *spi, uint8_t channel, float voltage) {
    uint16_t data = 0;
    uint8_t address = 0;

    data = (uint16_t)((LTC2634_BITS_RES * voltage) / LTC2634_REF_VOLT);
    if (channel == 1)
        address = ADDRESS_DAC_A;
    else if(channel == 2)
        address = ADDRESS_DAC_B;
    else if(channel == 3)
        address = ADDRESS_DAC_C;
    else if(channel == 4)
        address = ADDRESS_DAC_D;
    else
        address = ADDRESS_DAC_ALL;

    ILOG("Setting DAC O/P: %d, addr: %x\n", data, address);

    CLOG("DAC Output: BC = %d   Volt = %0.3f\n", data, voltage);
    if (ltc2634_dac_read_write(spi, address, DAC_WRITE_CMD, &data) == 0) {  
        ILOG("[DAC] Read or write success\n");
        return 0;
    } else {
        ILOG("[DAC] Read or write Failure\n");
    }
    return -1;
}


int ltc2634_dac_set_output_cnt(spi_t *spi, uint8_t channel, uint16_t data) {
    uint8_t address = 0;

    if (channel == 1)
        address = ADDRESS_DAC_A;
    else if(channel == 2)
        address = ADDRESS_DAC_B;
    else if(channel == 3)
        address = ADDRESS_DAC_C;
    else if(channel == 4)
        address = ADDRESS_DAC_D;
    else
        address = ADDRESS_DAC_ALL;

    DLOG("Setting DAC O/P: %d, addr: %x\n", data, address);
    if (ltc2634_dac_read_write(spi, address, DAC_WRITE_CMD, &data) == 0) {
        DLOG("[DAC] Read or write success\n");
        return 0;
    } else {
        DLOG("[DAC] Read or write Failure\n");
    }
    return -1;
}