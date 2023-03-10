/**
 * @file ADS7038.c
 * @author Shahid Hashmi
 * @brief 
 * @version 0.1
 * @date 2023-02-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */
/* Library */
#include "headers.h"
#include "ADS7038.h"

/* This Function is used to initialize ADS SPI */
spi_t *ads7038_init(char *ADSspi_dev, char spi_mode, int freq) {
    spi_t *spi;
    spi = spi_new();
    if (spi == NULL) {
        CLOG("Fail to create SPI device!!!\n");
        return NULL;
    }
    CLOG("ads7038: SPI Dev: %s, mode: %d, freq: %d, %p\n", ADSspi_dev, spi_mode, freq, spi);
    if (spi_open(spi, ADSspi_dev, spi_mode, freq)) {
        ILOG("Print error Code Here\r\n");
        return NULL;
    }
    return spi;
}

/* This function is used to deinit */
int8_t ads7038_deinit(spi_t *spi) {
    if (spi_close(spi)) {
        ILOG("Fail to close the ADS7038 SPI device\r\n");
        return -1;
    }
    return 0;
}

/* This is used to read data from ADS7038*/
// int8_t AD7988_ReadData(spi_t *spi, uint16_t *data, int samp_cnt) {
//     uint8_t txBuff[10];
//     uint8_t rxBuff[samp_cnt * 2];

//     memset(txBuff, 0, 10);
//     memset(rxBuff, 0, samp_cnt * 2);
//     // int offset = 0;
//     DLOG("---------Txbuff size: %d-------\r\n", (int)sizeof(rxBuff));
//     for (int samp_index = 0; samp_index < samp_cnt; (samp_index++)) {
//         if (spi_transfer(spi, txBuff, rxBuff + (samp_index * 2), 2)) {
//             WLOG("SPI Read error: %s", spi->error.errmsg);
//             return spi->error.c_errno;
//         }
//         //usleep(3);
//     }

//     //switching MSB-LSB
//     for (int samp_index = 0; samp_index < samp_cnt; samp_index++) {
//         data[samp_index] = (uint16_t)(rxBuff[samp_index * 2 + 1] | (uint16_t)(rxBuff[samp_index * 2] << 8));
//     }

//     return 0;
// }

/* This is used to Write data from ADS7038 */
int8_t AD7988_WriteData(spi_t *spi, uint16_t *data, int samp_cnt) {
    uint8_t txBuff[24];
    uint8_t rxBuff[samp_cnt * 2];

    memset(txBuff, 0, 24);
    memset(rxBuff, 0, samp_cnt * 2);
    // int offset = 0;
    DLOG("---------Txbuff size: %d-------\r\n", (int)sizeof(rxBuff));
    for (int samp_index = 0; samp_index < samp_cnt; (samp_index++)) {
        if (spi_transfer(spi, txBuff, rxBuff + (samp_index * 2), 2)) {
            WLOG("SPI Read error: %s", spi->error.errmsg);
            return spi->error.c_errno;
        }
        //usleep(3);
    }

    //switching MSB-LSB
    for (int samp_index = 0; samp_index < samp_cnt; samp_index++) {
        data[samp_index] = (uint16_t)(rxBuff[samp_index * 2 + 1] | (uint16_t)(rxBuff[samp_index * 2] << 8));
    }

    return 0;
}

/* This function is used to Configure ADS7038 in AIN/GPIO */
//int8_t ADS7038pin_configuration()
