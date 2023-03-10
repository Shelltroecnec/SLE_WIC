/**
 * @file rtc.h
 * @author Shahid Hashmi (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
/* Libaray includes */
#include <stdint.h>


/* Genenral Specificarions */
#define MIN_OPERATING_TEMP  -40
#define MAX_OPERATING_TEMP   85
#define EXTERNAL_CLK    32768

#define RTC_MIN_ARG_REQ 2
#define RTC_MAX_ARG_REQ 2

/* Registers */
#define RTC_SECOND  0x00
#define RTC_MINUTE  0x01
#define RTC_HOUR    0x02
#define RTC_DAY     0x03
#define RTC_DATE    0x04
#define RTC_MONTH   0x05
#define RTC_YEAR    0x06
#define RTC_CONTROL_REG     0x07 //Configuration Register
#define RTC_TRICKLE_CHARGER_REG     0x08
#define RTC_FLAG_REG    0x09

/* Functions */
void rtc_help(void);
void rtc_cmd(int argc,char **argv,void *additional_arg);
int rtc_init(char *filename, uint16_t dev_addr);
void rtc_deinit(void);
int rtc_display();
int rtc_write();
char rtc_read();
char rtc_write_read();
//char ioexpander_write_read(char *dev, char IOExp_number, char *dir, char port, char pin, char value);