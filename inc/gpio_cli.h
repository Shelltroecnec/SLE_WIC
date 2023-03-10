#ifndef GPIO_CLI_H
#define GPIO_CLI_H

/**
 * @file gpio_control.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-03-06
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#define GPIO_MIN_ARG_REQ                4
#define GPIO_MAX_ARG_REQ                4

#define FROM_GPIOS                      1
#define FROM_IOEXP_1                    2
#define FROM_IOEXP_2                    4

#define GPIO_DIR_IN                     "in"
#define GPIO_DIR_OUT                    "out"
#define GPIO_DIR_READ                   "read"


#define IMX_GPIO_NR(port, index) ((((port)-1) * 32) + ((index)&31))
#define IMX_GET_PORT_GPIO_NR(x) ((x / 32) + 1)
#define IMX_GET_PIN_GPIO_NR(x) (x % 32)
#define MAX_BUF 100

void led_help(void);
void led_cmd(int argc, char **argv, void *additional_arg);
void gpio_cmd(int argc,char **argv,void *additional_arg);
void gpio_help(void);
int gpio_string_parser(char *str, char *io_array);
char digital_pin_in_read(char *str);
char digital_pin_out_write(char *str, char value);
char digital_pin_read(char *str);
char gpio_write_read(int gpio, char *gpio_dir, char value);

#endif