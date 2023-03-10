/**
 * @file gpio_cli.c
 * @author Fakhruddin Khan (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2021-03-06
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "headers.h"
#include <fcntl.h>

// Error Handling.....and if the pin is not default as gpio...what?

/**
 * @brief Write and Read GPIO pins
 *
 * @param gpio GPIO number
 * @param gpio_dir GPIO direction
 * @param value GPIO value
 * @return char value of GPIO after operation
 */
char gpio_write_read(int gpio, char *gpio_dir, char value)
{
    int retval, retTemp;
    int fd;

    char buf[MAX_BUF];
    char bufVal[10] = {0};
    // Export GPIO pin//
    fd = open("/sys/class/gpio/export", O_WRONLY);

    sprintf(buf, "%d", gpio);
    retval = retTemp = write(fd, buf, strlen(buf));
    if (retval < 0)
        DLOG("Export failed! or already exported!\n\r");
    else
        DLOG("GPIO%02d export success\n\r", gpio);
    close(fd);

    if (strstr(gpio_dir, GPIO_DIR_OUT) != NULL)
    { // Pin direction --OUTPUT//
        // SET GPIO pin out Direction//
        sprintf(buf, "/sys/class/gpio/gpio%d/direction", gpio);
        fd = open(buf, O_WRONLY);
        retTemp = write(fd, "out", 3);
        close(fd);

        sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
        fd = open(buf, O_WRONLY);
        sprintf(bufVal, "%d", value);
        retTemp = write(fd, bufVal, 1); /* Set GPIO state */

        DLOG("[GPIO] GPIO_%02d_%02d - out, set to %d\n\r", IMX_GET_PORT_GPIO_NR(gpio), IMX_GET_PIN_GPIO_NR(gpio), value);
        close(fd);
    }
    else if (strstr(gpio_dir, GPIO_DIR_IN) != NULL)
    { // Pin direction --INPUT//

        sprintf(buf, "/sys/class/gpio/gpio%d/direction", gpio);
        fd = open(buf, O_WRONLY);
        retTemp = write(fd, "in", 2);
        close(fd);

        sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
        fd = open(buf, O_RDONLY);

        /* Read GPIO status */
        // lseek(fd, 0, SEEK_SET);
        retTemp = read(fd, bufVal, 1);
        value = bufVal[0] - 48; // convert ASCI to int
        DLOG("[GPIO] GPIO_%02d_%02d - in, current level: %d\n\r", IMX_GET_PORT_GPIO_NR(gpio), IMX_GET_PIN_GPIO_NR(gpio), value);
        close(fd);
    }
    else if (strstr(gpio_dir, GPIO_DIR_READ) != NULL)
    { // Pin direction --INPUT//
        sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);
        fd = open(buf, O_RDONLY);
        if (fd != -1)
        {
            // lseek(fd, 0, SEEK_SET);
            retTemp = read(fd, bufVal, 1);
            value = bufVal[0] - 48; // convert ASCI to int
            DLOG("[GPIO] GPIO_%02d_%02d - read, current level: %d\n\r", IMX_GET_PORT_GPIO_NR(gpio), IMX_GET_PIN_GPIO_NR(gpio), value);
        }
        else
        {
            DLOG("[GPIO] Fail to read the GPIO[%d]\n\r", gpio);
        }
        close(fd);
    }
    else
    { // Pin direction --NOT DEFINED//
        DLOG("[GPIO] Invalid Direction set!!!!\n");
        // gpio_help();
    }
    return value;
}

void led_help(void)
{
    CLOG("   Not implemented yet, In progress:\n");
}

/**
 * @brief GPIO Helper function
 *
 */
void gpio_help(void)
{
    CLOG("GPIO command Usage:\n");
    CLOG("   %s -c GPIO [PIN] [DIR] [VALUE]\n", exec_filename);
    // CLOG("      available PORT: 1-5\n");
    CLOG("      available PIN: 0-255\n");
    CLOG("      available DIR:\n");
    CLOG("          in   - GPIO as input mode\n");
    CLOG("          out  - GPIO as output mode\n");
    CLOG("          %s - Just Read the GPIO without setting/changing its direction\n", GPIO_DIR_READ);
    CLOG("      available VALUE:\n");
    CLOG("          0 - Low\n");
    CLOG("          1 - High\n");
    CLOG("      Note: value parameter mandatory for CLI but will be ignored 'DIR as in'\n");
}

void led_cmd(int argc, char **argv, void *additional_arg)
{
    // int iLoop = 0;

    CLOG("Command %s is in progress\r\n", argv[0]);
}

/**
 * @brief                   GPIO CLI command
 *
 * @param argc              (ARGument Count) is int and stores number of command-line arguments passed by the user including the name of the program.
 *                          So if we pass a value to a program, value of argc would be 2 (one for argument and one for program name)
 *                          The value of argc should be non negative.
 * @param argv              (ARGument Vector) is array of character pointers listing all the arguments.
                            If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
                            Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.
 * @param additional_arg    Any additional arguments required
 */
void gpio_cmd(int argc, char **argv, void *additional_arg)
{
    if (argc < GPIO_MIN_ARG_REQ)
    {
        gpio_help();
        return;
    }
    int log_level_bak = 0;
    // int fd;
    int retval = 0;
    // int port = atoi(argv[1]);
    int pin = atoi(argv[1]);
    char *dir = argv[2];

    char value = atoi(argv[3]); // REVIEW: Add validity o this as well

    log_level_bak = config.log_level;

    config.log_level = 3;
    // if (port > 5 || port < 1) {
    //     CLOG("Invalid port!!!\n");
    //     gpio_help();
    //     return;
    // }

    if (pin > 255 || pin < 0)
    {
        CLOG("Invalid Pin!!!\n");
        gpio_help();
        return;
    }

    /* Check here if the pin is configured as GPIO by default*/

    // int gpio = IMX_GPIO_NR(port, pin);
    int gpio = pin;
    CLOG("Selected GPIO Pin: %d\n\r", gpio);

    retval = gpio_write_read(gpio, dir, value);
    /*Error Handling*/
    if (retval == 0)
    {
        ILOG("GPIO low\n\r");
    }
    else
    {
        ILOG("GPIO High\n\r");
    }
    config.log_level = log_level_bak;
}

/**
 * @brief               This function parse a IO string to io_array[3] which indicates which GPIO is configured
 *
 * @param str           pointer to char array for GPIO configuration
 *                      Here are the sample of valid keys for GPIO configuration
 *                      GPIO-2-32 -- GPIO, Port 2, Pin 32
 *                      IOE0-1-4  -- IOExpander 1, port 1, Pin 4
 *                      IOE1-20-7  -- IOExpander 2, port 0, Pin 7
 * @param io_array      char array pointer of 3 bytes size which holds type of IO, port number and pin number
 * @return int          parsing success or failure - 0: success, -1: failed (invalid strings parsing will return -1)
 */
int gpio_string_parser(char *str, char *io_array)
{
    char *token;
    char strcopy[32] = {0};
    strcpy(strcopy, str);

    token = strtok(strcopy, "-");
    if (token != NULL)
    {
        if (strcmp(token, "GPIO") == 0)
        {
            io_array[0] = FROM_GPIOS;
        }
        else if (strcmp(token, "IOE1") == 0)
        {
            io_array[0] = FROM_IOEXP_1;
        }
        else if (strcmp(token, "IOE2") == 0)
        {
            io_array[0] = FROM_IOEXP_2;
        }
        else
        {
            ILOG("Not a valid string....\n");
            return -1;
        }
        // ILOG("Token1: %s, %d\n", token, io_array[0]);
    }
    else
    {
        WLOG("Failed to parse the string\n");
        return -1;
    }
    if (strcmp(token, "GPIO") == 0)
    {
        token = strtok(NULL, "-");
        if (token != NULL)
        {
            io_array[1] = atoi(token);
            // ILOG("Token2: %s, port: %d\n", token, io_array[1]);
        }
        else
        {
            WLOG("Failed to parse the string at pin number\n");
            return -1;
        }
    }

    else
    {
        token = strtok(NULL, "-");
        if (token != NULL)
        {
            io_array[1] = atoi(token);
            // ILOG("Token2: %s, port: %d\n", token, io_array[1]);
        }
        else
        {
            WLOG("Failed to parse the string at pin number\n");
            return -1;
        }

        token = strtok(NULL, "-");
        if (token != NULL)
        {
            io_array[2] = atoi(token);
            // ILOG("Token3: %s, pin: %d\n", token,io_array[2]);
        }
        else
        {
            WLOG("Failed to parse the string at pin number\n");
            return -1;
        }
    }

    return -1;
}

/**
 * @brief               Read digital pins inputs
 *
 * @param str           pointer to digial pin
 * @return              read values
 */
 char digital_pin_in_read(char *str)
 {
    char io_detail[3] = {0};
    char temp = 0;
    gpio_string_parser(str, io_detail);

    if (io_detail[0] == FROM_GPIOS)
    {
        //int gpio = IMX_GPIO_NR(io_detail[1], io_detail[2]);
        int gpio = io_detail[1];
        ILOG("GPIO Number selected: %d %s\n\r", gpio, str);
        temp = gpio_write_read(gpio, GPIO_DIR_IN, temp);
    }
    else if (io_detail[0] == FROM_IOEXP_1)
    {
        temp = ioexpander_write_read(config.IOExpDev, FROM_IOEXP_1, GPIO_DIR_IN, io_detail[1], io_detail[2], temp);
    }
    else if (io_detail[0] == FROM_IOEXP_2)
    {
        temp = ioexpander_write_read(config.IOExpDev, FROM_IOEXP_2, GPIO_DIR_IN, io_detail[1], io_detail[2], temp);
        DLOG("In GPIO_IN-IOEX File \n");
        DLOG("Temp = %d \r\n",temp);
    }
    return temp;
}

/**
 * @brief               Write Digital pin
 *
 * @param str           pointer to digital pin channel
 * @param value         value to be written
 * @return              return 0 on success, non 0 on failure
 */
char digital_pin_out_write(char *str, char value)
{
    char io_detail[3] = {0};
    gpio_string_parser(str, io_detail);

    if (io_detail[0] == FROM_GPIOS)
    {
        //int gpio = IMX_GPIO_NR(io_detail[1], io_detail[2]);
        int gpio = io_detail[1];
        ILOG("GPIO Number selected: %d %s\n\r", gpio,str);
        gpio_write_read(gpio, GPIO_DIR_OUT, value);
    }
    else if (io_detail[0] == FROM_IOEXP_1)
    {
        value = ioexpander_write_read(config.IOExpDev, FROM_IOEXP_1, GPIO_DIR_OUT, io_detail[1], io_detail[2], value);
        CLOG("In GPIO_OUT-IOEX File \n");
    }
    else if (io_detail[0] == FROM_IOEXP_2)
    {
        value = ioexpander_write_read(config.IOExpDev, FROM_IOEXP_2, GPIO_DIR_OUT, io_detail[1], io_detail[2], value);
    }
    return 0;
}

/**
 * @brief               Read status of Digital pin
 *
 * @param str           pointer to digital pin channel
 * @return              Value of digital pin
 */
char digital_pin_read(char *str)
{
    char io_detail[3] = {0};
     char readValue = 0;
     gpio_string_parser(str, io_detail);

     if (io_detail[0] == FROM_GPIOS)
     {
         int gpio = IMX_GPIO_NR(io_detail[1], io_detail[2]);
         ILOG("GPIO Number selected: %d %s\n\r", gpio, str);
         readValue = gpio_write_read(gpio, GPIO_DIR_READ, readValue);
     }
     else if (io_detail[0] == FROM_IOEXP_1)
     {
         readValue = ioexpander_write_read(config.IOExpDev, FROM_IOEXP_1, GPIO_DIR_READ, io_detail[1], io_detail[2], readValue);
         CLOG("In GPIO_INEX File \r\n");
     }
     else if (io_detail[0] == FROM_IOEXP_2)
     {
         readValue = ioexpander_write_read(config.IOExpDev, FROM_IOEXP_2, GPIO_DIR_READ, io_detail[1], io_detail[2], readValue);
     }
     return readValue;
}