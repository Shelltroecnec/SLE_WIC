/**
 * @file max31865.c
 * @author Fakhruddin Khan
 * @brief 
 * @version 0.1
 * @date 2021-04-09
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "max31865.h"
#include "headers.h"
#include "spi.h"
#include <linux/ioctl.h>
#include <linux/spi/spidev.h>
#include <math.h>
#include <stdbool.h>
#include <sys/ioctl.h>

/*---------------------------Variable Declaration-----------------------------*/
// s

/*--------------------------------Subroutine----------------------------------*/

/**
 * @brief           initialize the SPI driver for accessing the RTD
 * 
 * @param spi_dev   string, SPI device on which RTD is connected
 * @param spi_mode  SPI mode of operation(1/3 supported by MAX31865)
 * @param freq      SPI clock frequency
 * @return spi_t*   returns a pointer to SPI handler
 */
spi_t *max31865_init(char *spi_dev, char spi_mode, int freq) {
    uint8_t  ucrx_buff[6], uctx_buff[6];
    spi_t *spi = NULL;
    spi = spi_new();
    spi->dev = spi_dev;
    ILOG("max31865_init SPI Dev: %s, mode: %d, freq: %d\n", spi_dev, spi_mode, freq);
    if (spi_open(spi, spi_dev, spi_mode, freq))
        ILOG("Print error Code Here\r\n");

    rtd_write_byte(spi, WRITE_CONFIG_ADDR, 0xD0); //configure RTD config register for 3-WIRE SETUP
    usleep(100000);                               //100ms delay  //sampling rate 60Hz Filter, 16.67ms(5 time constant) + 1ms(additional) = 17.67ms(toatal delay)

    uctx_buff[0] = 0xff; //WRITE_HFT_MSB_ADDR
    uctx_buff[1] = 0xff; //WRITE_HFT_LSB_ADDR
    uctx_buff[2] = 0x00;    //WRITE_LFT_MSB_ADDR
    uctx_buff[3] = 0x00;    //WRITE_LFT_LSB_ADDR
    rtd_write_bytes(spi, WRITE_HFT_MSB_ADDR, uctx_buff, 4);

    rtd_read_bytes(spi, READ_HFT_MSB_ADDR, ucrx_buff, 4); //commented while debugging
    DLOG("read: 0x%02X\n", ucrx_buff[1]);
    DLOG("read: 0x%02X\n", ucrx_buff[2]);
    DLOG("read: 0x%02X\n", ucrx_buff[3]);
    DLOG("read: 0x%02X\n", ucrx_buff[4]);

    if (ucrx_buff[1] == 0xff)
        ILOG("RTD SPI COMM working\r\n");
    else
        ILOG("RTD SPI COMM NOT working\r\n");

    return spi;
}

/**
 * @brief           Close the SPI driver for RTD
 * 
 * @param spi       SPI handler to be close   
 * @return int8_t   return success(0) or fail(-1)
 */
int8_t max31865_deinit(spi_t *spi) {
    DLOG("Closing SPI %s\n", spi->dev);
    if (spi_close(spi)) {
        ILOG("Fail to close the RTD SPI device\r\n");
        return -1;
    }
    return 0;
}

/**
 * @brief                   read the RTD sensor for the selected channel and given number of samples
 * 
 * @param spi               SPI handler
 * @param pst_rtd_read      pointer to rtd read structure
 * @return uint8_t          return success or fail, 0 - on success non 0 on failure
 */
uint8_t rtd_sensor_scan(spi_t *spi, st_rtd_sample_t *pst_rtd_read, int channel) {
    uint8_t sample_no = 0, error_status = 0;

    rtd_write_byte(spi, WRITE_CONFIG_ADDR, 0xD2); //writing a 1 to the Fault Status Clear bit in the Configuration Register returns all fault status bits to 0.
    pst_rtd_read->counter = 0;
    for (sample_no = 0; sample_no < pst_rtd_read->rtd_sample_cnt; sample_no++) {
        usleep(100000); //100ms delay     //Sampling Time delay to get a new conversion in RTD data register
        rtd_temp_read(spi, pst_rtd_read, channel);
        pst_rtd_read->counter += 1;     //increment by 1
        if (pst_rtd_read->rtd_fault) {
            ILOG("Fault Bit Set\r\n");
            return 1;
        }
    }
    rtd_write_byte(spi, WRITE_CONFIG_ADDR, 0x00);

    pst_rtd_read->avg_adc_count[channel - 1] = avg_samples_integer((uint16_t*)&pst_rtd_read->adc_count[channel-1][0], pst_rtd_read->rtd_sample_cnt, &pst_rtd_read->max_adc_count[channel - 1], &pst_rtd_read->min_adc_count[channel - 1]);
    pst_rtd_read->rtd_avg_temp[channel - 1] = rtd_calculate_temp(pst_rtd_read->avg_adc_count[channel - 1]);
    pst_rtd_read->rtd_max_temp[channel - 1] = rtd_calculate_temp(pst_rtd_read->max_adc_count[channel - 1]);
    pst_rtd_read->rtd_min_temp[channel - 1] = rtd_calculate_temp(pst_rtd_read->min_adc_count[channel - 1]);

    return error_status;
}

/**
 * @brief               Read single byte from RTD
 * 
 * @param spi           Pointer to SPI handler
 * @param ucRegAddr     Register address
 * @param pbuf          pointer to uint8_t array for storing the read data
 * @return int8_t       read byte status, 0 success, non 0 - failure
 * 
 */
int8_t rtd_read_byte(spi_t *spi, uint8_t ucRegAddr, uint8_t *pbuf) {
    uint8_t txBuff[2] = {ucRegAddr, 0x00};
    if (spi_transfer(spi, txBuff, pbuf, 2)) {
        ILOG("SPI Read error: %s", spi->error.errmsg);
        return spi->error.c_errno;
    }
    return 0;
}

/**
 * @brief               Write single byte to RTD
 * 
 * @param spi           Pointer to SPI handler
 * @param ucRegAddr     Register address
 * @param data          byte data to be written
 * @return int8_t       write byte status, 0 success, non 0 - failure
 */
int8_t rtd_write_byte(spi_t *spi, uint8_t ucRegAddr, uint8_t data) {
    uint8_t spi_tx_buff[2], spi_rx_buff[2];

    spi_tx_buff[0] = ucRegAddr;
    spi_tx_buff[1] = data;

    if (spi_transfer(spi, spi_tx_buff, spi_rx_buff, 2)) {
        WLOG("SPI write error: %s", spi->error.errmsg);
        return spi->error.c_errno;
    }
    return 0;
}

/**
 * @brief               Read multiple bytes from the MAX31865 
 * 
 * @param spi           Pointer to SPI handler
 * @param ucRegAddr     Start register address from where read should be started
 * @param puReadBuff    pointer to read buffer array, size of this buffer should be size+1
 * @param size          number of bytes to be read from the IC
 * @return int8_t       read multi byte status, 0 success, non 0 - failure
 * 
 */
int8_t rtd_read_bytes(spi_t *spi, uint8_t ucRegAddr, uint8_t *puReadBuff, uint8_t size) {
    uint8_t txBuff[255] = {ucRegAddr, 0x00};
    if (spi_transfer(spi, txBuff, puReadBuff, size + 1)) {
        ILOG("SPI Bytes Read error: %s", spi->error.errmsg);
        return spi->error.c_errno;
    }
    return 0;
}

/**
 * @brief               Write multiple bytes to MAX31865 
 * 
 * @param spi           SPI handler
 * @param ucRegAddr     Start register address from where write should start
 * @param write_bytes   array to uint8_t to be written
 * @param size          Number of bytes to write on MAX31865
 * @return int8_t       return status, 0 - success, non 0 - failure/error
 */
int8_t rtd_write_bytes(spi_t *spi, uint8_t ucRegAddr, uint8_t *write_bytes, uint8_t size) {
    uint8_t spi_tx_buff[255], spi_rx_buff[255];

    spi_tx_buff[0] = ucRegAddr;
    memcpy(&spi_tx_buff[1], write_bytes, size);

    if (spi_transfer(spi, spi_tx_buff, spi_rx_buff, size + 1)) {
        WLOG("SPI write error: %s", spi->error.errmsg);
        return spi->error.c_errno;
    }
    return 0;
}

/**
 * @brief            Reas fault status register of RTD
 * 
 * @param spi        SPI handler
 * @return uint8_t   return read fault status byte 
 */
uint8_t rtd_read_fault_status_reg(spi_t *spi) {
    uint8_t ui8RxBuffer[2];
    rtd_read_byte(spi, FAULT_STATUS_ADDR, ui8RxBuffer); //reading the fault status register to get fault specifications
    uint8_t fault_status_byte = ui8RxBuffer[1];

    //fault_error_spec(fault_status_byte);
    rtd_write_byte(spi, WRITE_CONFIG_ADDR, 0xD2); //writing a 1 to the Fault Status Clear bit in the Configuration Register returns all fault status bits to 0.
    rtd_write_byte(spi, WRITE_CONFIG_ADDR, 0xD0); // Setting the device in autoconfiguration again
    usleep(100000);

    //Verify if the fault bit is reset
    rtd_read_byte(spi, READ_RTD_LSB_ADDR, ui8RxBuffer); //reading the fault status register to get fault specifications
    fault_status_byte = ui8RxBuffer[1];
    DLOG("After clearing fault status, RTD LSB: %x\n",fault_status_byte);
    rtd_write_byte(spi, WRITE_CONFIG_ADDR, 0x00);
    return fault_status_byte;
}

/**
 * @brief                   Read RTD resistence from SPI ADC, calculate temp and store it in structure
 * 
 * @param spi               SPI handler for RTD
 * @param pst_rtd_read      Pointer to rtd read structure
 */
void rtd_temp_read(spi_t *spi, st_rtd_sample_t *pst_rtd_read, uint8_t channel) {
    uint8_t ui8RxBuffer[4], rtd_data_reg_msb_byte = 0, rtd_data_reg_lsb_byte = 0;
    if(0 == rtd_read_bytes(spi, READ_RTD_MSB_ADDR, ui8RxBuffer, 2)) {
        rtd_data_reg_msb_byte = ui8RxBuffer[1]; //READ_RTD_MSB_ADDR
        rtd_data_reg_lsb_byte = ui8RxBuffer[2]; //READ_RTD_LSB_ADDR
        pst_rtd_read->rtd_fault = rtd_data_reg_lsb_byte & 0x01;

        DLOG("MSB = %x, LSB %x\r\n", rtd_data_reg_msb_byte,rtd_data_reg_lsb_byte);

        if (pst_rtd_read->rtd_fault) { //if fault bit is set then dont read the temperature at all
            CLOG("RTD data read fault bit set. ");
            return;
        }
        pst_rtd_read->adc_count[channel - 1][pst_rtd_read->counter] = (uint16_t)((rtd_data_reg_msb_byte << 7) + (rtd_data_reg_lsb_byte >> 1));
    }
}

/**
 * @brief                   calculate temp from ADC count read from SPI
 * 
 * @param rtd_adc_count     ADC count value read from SPI ADC
 * @return float            Calculated temperature value
 */
float rtd_calculate_temp(uint16_t rtd_adc_count) {
    float temperature = 0.0f, resistance = 0.0f;
    float nr, dr;

    resistance = RTD_COUNT_TO_RESISTANCE(rtd_adc_count); // adc conversion

    // ILOG("RDT: adc cnt: %d resistance = %0.3f\n", rtd_adc_count, resistance);
    if (resistance >= RTD_RES_ZERO_DEGREE) {
        temperature = -RTD_RES_ZERO_DEGREE * RTD_ALPHA + sqrt(RTD_RES_ZERO_DEGREE * RTD_RES_ZERO_DEGREE * RTD_ALPHA * RTD_ALPHA - 4 * RTD_RES_ZERO_DEGREE * RTD_BETA * (RTD_RES_ZERO_DEGREE - resistance));
        temperature = temperature / (2 * RTD_RES_ZERO_DEGREE * RTD_BETA);
    } else {
        temperature = ((resistance / RTD_RES_ZERO_DEGREE) - 1);
        temperature = temperature / (RTD_ALPHA + (100 * RTD_BETA));

        DLOG("RDT: temperature = %0.3f\n", temperature);

        nr = 1 + (RTD_ALPHA * temperature) + (RTD_BETA * temperature * temperature) +
             ((RTD_GAMMA * temperature * temperature * temperature) * (temperature - 100) -
              (resistance / RTD_RES_ZERO_DEGREE));
        dr = RTD_ALPHA + (2 * RTD_BETA * temperature) - (300 * RTD_GAMMA * temperature * temperature) +
             (4 * RTD_GAMMA * temperature * temperature * temperature);
        temperature = temperature - (nr / dr);

        nr = 1 + (RTD_ALPHA * temperature) + (RTD_BETA * temperature * temperature) +
             ((RTD_GAMMA * temperature * temperature * temperature) * (temperature - 100) -
              (resistance / RTD_RES_ZERO_DEGREE));
        dr = RTD_ALPHA + (2 * RTD_BETA * temperature) - (300 * RTD_GAMMA * temperature * temperature) +
             (4 * RTD_GAMMA * temperature * temperature * temperature);
        temperature = temperature - (nr / dr);

        nr = 1 + (RTD_ALPHA * temperature) + (RTD_BETA * temperature * temperature) +
             ((RTD_GAMMA * temperature * temperature * temperature) * (temperature - 100) -
              (resistance / RTD_RES_ZERO_DEGREE));
        dr = RTD_ALPHA + (2 * RTD_BETA * temperature) - (300 * RTD_GAMMA * temperature * temperature) +
             (4 * RTD_GAMMA * temperature * temperature * temperature);
        temperature = temperature - (nr / dr);
    }
    return temperature;
}