/**
 * @file rtc.c
 * @author Shahid Hashmi (you@domain.com)
 * @brief This file is about CLI specified functions for RTC (DS1340)
 * interfaced with the i2c to SC606 CPU via I2C Bus
 * @version 0.1
 * @date 2023-02-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "headers.h"

/* Gobal */
sti2c_t *strtc_i2c = NULL;

void rtc_help(void)
{
    CLOG("RTC Command Usage:\n");
    CLOG("    %s -c RTC [VALUE]\n", exec_filename);
    CLOG("       available VALUE: 0-1 - \n");
}

void rtc_cmd(int argc, char **argv, void *additional_arg)
{
    if (argc < RTC_MIN_ARG_REQ) {
        // WLOG("[IOEX] Less number of argument/s provided for IOExp Cmd!!\n");
        ioexpander_help();
        return;
    }

    int RTCState = 0;
    //Parsing of the arguments
    RTCState = atoi(argv[1]);
    int idate = atoi(argv[2]);
    int imonth = atoi(argv[3]);
    int iyear = atoi(argv[4]);
    int iHour = atoi(argv[5]);
    int iminute = atoi(argv[6]);
    int isecond = atoi(argv[7]);

    rtc_display(char *dev, char IOExp_number, char *dir, char port, char pin, char value); //
}

/* RTC DS1340 I2C initialization Function */
int rtc_init(char *filename, uint16_t dev_addr) {

    int rc;
    /* Set the I2C bus filename and slave address*/
    strtc_i2c = i2c_new();
    /* Open the I2C device */
    if ((rc = i2c_open(strtc_i2c, filename)) < 0) {
        ELOG("i2c_open(): %s\n", i2c_errmsg(strtc_i2c));
        exit(1);
    }
    //Save the device and slave address for future used
    strtc_i2c->filename = filename;
    strtc_i2c->addr = dev_addr;

    if (rc < 0) {
        WLOG("[RTC] Failed to start RTC device %s\r\n", strtc_i2c->filename);
        return rc;
    }
    DLOG("[RTC] openned %s, set dev addr %02X, rc: %d\n", stI2c->filename, stI2c->addr, rc);

    return 0;
}

/* RTC DS1340 Dinitialization from I2C */
void rtc_deinit(void) {
    i2c_close(strtc_i2c);
    i2c_free(strtc_i2c);
}



