#ifndef NORDIC_H
#define NORDIC_H

void nordic_cmd(char *spi_dev, char spi_mode, int freq);
void nordic_help(void);

#define NORDIC_MIN_ARG_REQ          1
#define NORIDC_SPI_CLOCK_FREQ       2000000

#define NRD_PING_REQ_STR "To Nordic"     //the string to be sent from IMX to Nordic
#define NRD_PING_RESP_STR "From Nordic"  //the string response to be expected from Nordic to IMX

#define NORDIC_PING_MESS_CNT        10   //no of times to ping inorder to declare pass/fail

#endif