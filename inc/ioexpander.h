#ifndef IOEXPANDER_H
#define IOEXPANDER_H

#define PCA6416

/* I2C Device*/
#define I2C0_DEV_NAME                      "/dev/i2c-0"
#define I2C1_DEV_NAME                      "/dev/i2c-1"
#define I2C2_DEV_NAME                      "/dev/i2c-2"
#define I2C3_DEV_NAME                      "/dev/i2c-3"
#define I2C_RETRIES                        0x0701
#define I2C_TIMEOUT                        0x0702
#define I2C_SLAVE                          0x0703
#define I2C_RDWR                           0x0707
#define I2C_BUS_MODE                       0x0780

#define IO_EXP_MIN_ARG_REQ                 6
#define IO_EXP_MAX_ARG_REQ                 6

#define FROM_GPIOS                       1
#define FROM_IOEXP_1                     2
#define FROM_IOEXP_2                     4

#define IOEXP_INDEX_NUMBER(x)           ((x == 1)? FROM_IOEXP_2: FROM_IOEXP_1) 
#define IOEXP_NUMBER(x)                 ((x == FROM_IOEXP_2)? 1 : 0) 
#define PIN_VALUE(val,pin)              (((unsigned char)pin == 0xFF)? (val & pin) : ((val >> pin) & 0x01)) 

//Macro defination based on the PCA6416 IO Expander
#if defined PCA6416
    #define SLAVE_ADDR0                 0x20
    #define SLAVE_ADDR1                 0x21
    #define SLAVE_ADDR_EXP(x)           ((FROM_IOEXP_2 == x)? SLAVE_ADDR1: SLAVE_ADDR0)    

    //Register address
    #define GPIO_IN_REG                0x00
    #define GPIO_OUT_REG               0x02
    #define GPIO_PORARITY_REG          0x04
    #define GPIO_CFG_REG               0x06 
    #endif
    #define GET_IO_IN_PORT_REG_ADDR(x)  ((0 == x)? GPIO_IN_REG: ((1 == x)? GPIO_IN_REG + 1: GPIO_IN_REG))
    #define GET_IO_OUT_PORT_REG_ADDR(x) ((0 == x)? GPIO_OUT_REG: ((1 == x)? GPIO_OUT_REG + 1: GPIO_OUT_REG))
    #define GET_IO_POLARITY_PORT_REG_ADDR(x) ((0 == x)? GPIO_PORARITY_REG: ((1 == x)? GPIO_PORARITY_REG + 1: GPIO_PORARITY_REG))
    #define GET_IO_CFG_PORT_REG_ADDR(x)  ((0 == x)? GPIO_CFG_REG: ((1 == x)? GPIO_CFG_REG + 1: GPIO_CFG_REG))

//GPIO MASK
#define GPIO3_MASK                         0x10
#define GPIO4_MASK                         0x20
#define GPIO5_MASK                         0x40
#define GPIO6_MASK                         0x80
#define GPIO7_MASK                         0x08

void ioexpander_help(void);
void ioexpander_cmd(int argc,char **argv,void *additional_arg);
char ioexpander_write_read(char *dev, char IOExp_number, char *dir, char port, char pin, char value);
#endif