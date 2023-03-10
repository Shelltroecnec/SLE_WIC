
#include "headers.h"

#define TEMP_REG 0x0
#define CONFIG_REG 0x1

#define CONFIG_SHUTDOWN 0b00000001
#define CONFIG_ONESHOT 0b10000000
sti2c_t *stI2c = NULL;

/**
 * @brief 	        Initialize the IO expander and sets the slave address to the I2C device
 * 
 * @param filename 	I2C device name/location
 * @param dev_addr 	I2C Slave address of IO expanders, it could be 0/1 based on the ADDR pin of expander
 * @return int 		0 - success, -1 - Fail
 */
int IOExp_Init(char *filename, uint16_t dev_addr) {

    int rc;
    /*
	 * Set the I2C bus filename and slave address,
	 */
    stI2c = i2c_new();
    /*
	 * Open the I2C device.
	 */
    if ((rc = i2c_open(stI2c, filename)) < 0) {
        ELOG("i2c_open(): %s\n", i2c_errmsg(stI2c));
        exit(1);
    }
    //Save the device and slave address for future used
    stI2c->filename = filename;
    stI2c->addr = dev_addr;

    if (rc < 0) {
        WLOG("[IOEX] Failed to start stI2c device %s\r\n", stI2c->filename);
        return rc;
    }
    DLOG("[IOEX] openned %s, set dev addr %02X, rc: %d\n", stI2c->filename, stI2c->addr, rc);

    return 0;
}

/**
 * @brief Delinitialize IoExpander i2c
 * 
 */
void IOExp_deinit(void) {
    i2c_close(stI2c);
    i2c_free(stI2c);
}

/**
 * @brief IO Expander helper function
 * 
 */
void ioexpander_help(void) {
    CLOG("IOExp Command Usage:\n");
    CLOG("    %s -c IOEX [EXP_Num] [PORT] [PIN] [DIR] [VALUE]\n", exec_filename);
    CLOG("       available EXP_Num: 0-1 - IO Expander selection\n");
    CLOG("       available PORT: 0-1\n");
    CLOG("       available PIN: 0-7\n");
    CLOG("                      0xFF - allow to access complete PORT, VALUE will be written in out DIR\n");
    CLOG("       available DIR:\n");
    CLOG("           %s   - GPIO as input mode\n", GPIO_DIR_IN);
    CLOG("           %s  - GPIO as output mode\n", GPIO_DIR_OUT);
    CLOG("           %s - Just Read the GPIO without setting/changing its direction\n", GPIO_DIR_READ);
    CLOG("       available VALUE: This is a Hex string, Only required in case of Accessing full PORT\n");
    CLOG("           0 - Low\n");
    CLOG("           1 - High\n");
    CLOG("       Note: value parameter mandatory for CLI but will be ignored 'DIR as in'\n");
}


/**
 * @brief               Write and read funtion for io expander
 * 
 * @param dev           I2C Slave address of IO expanders, it could be 0/1 based on the ADDR pin of expander
 * @param IOExp_number  IO expander number, could be 0/1.
 * @param dir           Direction to be set
 * @param port          Port number of selected gpio
 * @param pin           Pin number
 * @param value         Value to be set
 * @return              value set after function call. 
 */
char ioexpander_write_read(char *dev, char IOExp_number, char *dir, char port, char pin, char value) {
    uint8_t ucWrBuff[10] = {0};
    uint8_t ucRdBuff[10] = {0};

    //Initialise the peripheral
    CLOG("[IOEX] IO Exp number: %d, add: 0x%x\n", IOExp_number, SLAVE_ADDR_EXP(IOExp_number));
    IOExp_Init(dev, SLAVE_ADDR_EXP(IOExp_number));

    if (strcmp(dir, GPIO_DIR_OUT) == 0) //Out mode
    {
        //Set direction of the port

        if ((unsigned char)pin != 0xff) {
            i2c_read_bytes(stI2c, GET_IO_CFG_PORT_REG_ADDR(port), ucRdBuff, 1); //Read all 8 register of the io expander
            ucWrBuff[0] = ucRdBuff[0] & ~(1 << pin);// set the pin number in the output mode
            DLOG("[IOEX] Reading Port %c, pin %c \r\n",port,pin);
            DLOG("[IOEX] Pin %c Set as Output pin, Data - %x \r\n",pin, ucWrBuff[0]);                            
        } 
        else {
            ucWrBuff[0] = 0; //set all the pins of given port as output
            //DLOG("[IOEX] Configuring the Whole Port %c as Output Port \r\n",port);
        }
        DLOG("-------------Writing-------------\r\n");
        DLOG("[IOEX] Writing to CFG Reg %d= 0x%02X \n", GET_IO_CFG_PORT_REG_ADDR(port), ucWrBuff[0]);
        i2c_write_bytes(stI2c, GET_IO_CFG_PORT_REG_ADDR(port), ucWrBuff, 1); //configure the mode of given port
        
        char oldRead; //?????
        if ((unsigned char)pin != 0xff) {
            i2c_read_bytes(stI2c, GET_IO_OUT_PORT_REG_ADDR(port), ucRdBuff, 1); //Read all 8 register of the io expander
            DLOG("[IOEX] Reading the Output Port Registers(02h,03h) \r\n");
            oldRead = ucRdBuff[0];
            if (value == 0) {
                ucWrBuff[0] = ucRdBuff[0] & ~(1 << pin);  //set the pin number in the output mode
                DLOG("[IOEX] Value = 0 and Data - %x \r\n",ucWrBuff[0]);
            } else if (value == 1) {
                ucWrBuff[0] = ucRdBuff[0] | (1 << pin);  //set the pin number in the output mode
                DLOG("[IOEX] Value = 1 and Data - %x \r\n",ucWrBuff[0]);
            } else {
                WLOG("[IOEX] Invalid value [%d] set for pin[%d]\n", value, pin);
            }
        } else {
            ucWrBuff[0] = value; //set all the pins of given port as output
            //DLOG("[IOEX] Setting Whole Port %c af Value %x \r\n",port,value);
        }
        DLOG("[IOEX] %s IOEX%d_%d_%x, set to: %x out port: 0x%02X\n", dir, IOEXP_NUMBER(IOExp_number), port, pin, value, ucWrBuff[0]);
        i2c_write_bytes(stI2c, GET_IO_OUT_PORT_REG_ADDR(port), ucWrBuff, 1); //configure the mode of given port
    }

    //Direction Input 
    else if (strcmp(dir, GPIO_DIR_IN) == 0) {
        
        if ((unsigned char)pin != 0xff) {
            i2c_read_bytes(stI2c, GET_IO_CFG_PORT_REG_ADDR(port), ucRdBuff, 1);
            DLOG("[IOEX] Input Read Reg %x \r\n",ucRdBuff[0]);

            ucWrBuff[0] = ucRdBuff[0] | (1<<pin); // set the pin number in the Input mode
            DLOG("[IOEX] Reading Port %d, pin %d \r\n",port,pin);
            DLOG("[IOEX] Pin %d Set as Input pin, Data - %x \r\n",pin,ucWrBuff[0]);
        } else {
            ucWrBuff[0] = 0xFF; //set all the pins of given port as input
            DLOG("[IOEX] Configurating whole port %d as Input Port \r\n",port);
        }

        DLOG("-----------Writing---------\r\n");
        DLOG("[IOEX] Writing to CFG Reg %d = 0x%02X\n", GET_IO_CFG_PORT_REG_ADDR(port), ucWrBuff[0]);
        i2c_write_bytes(stI2c, GET_IO_CFG_PORT_REG_ADDR(port), ucWrBuff, 1); //configure the mode of given port

        i2c_read_bytes(stI2c, GET_IO_IN_PORT_REG_ADDR(port), ucRdBuff, 1); //Read
        DLOG("[IOEX] Read input Reg  address: 0x%X value: 0x%02X\n", GET_IO_IN_PORT_REG_ADDR(port), ucRdBuff[0]);
        // ILOG("[IOEX] IN, IO Exp Num: %d, port: %d, Read value: 0x%02X\n", IOExp_number, port, ucRdBuff[0]);
        //CLOG("[IOEX] %s IOEX%d_%d_%x, in port: 0x%02X\n", dir, IOEXP_NUMBER(IOExp_number), port, pin, PIN_VALUE(ucRdBuff[0], pin));

        value = (((unsigned char)pin == 0xff) ? (ucRdBuff[0] & pin) : (ucRdBuff[0] >> pin));
        DLOG("[IOEX] Input Operation Reading value = %d \r\n",value);

    }

    //Reading the Pin
    else if (strcmp(dir, GPIO_DIR_READ) == 0) {
        //Just read the GPIO port // just read the IO dont change the state
        i2c_read_bytes(stI2c, GET_IO_IN_PORT_REG_ADDR(port), ucRdBuff, 1); //Read
        char in_port = ucRdBuff[0];
        CLOG("[IOEX] Reading Operation Input_Port = %c \r\n",in_port);
        i2c_read_bytes(stI2c, GET_IO_OUT_PORT_REG_ADDR(port), ucRdBuff, 1); //Read
        char out_port = ucRdBuff[0];
        CLOG("[IOEX] Reading Operation Output_Port = %c \r\n",out_port);
        // ILOG("[IOEX] Read input Reg address: 0x%X value: 0x%02X\n", GET_IO_IN_PORT_REG_ADDR(port), ucRdBuff[0]);
        // ILOG("[IOEX] Read, IO Exp Num: %d, port: %d, Read value: 0x%02X\n", IOExp_number, port, ucRdBuff[0]);
        out_port = PIN_VALUE(out_port, pin);
        char out_port_arr[40] = {0};
        byte_to_bits(&out_port, 1, out_port_arr);

        in_port = PIN_VALUE(in_port, pin);
        char in_port_arr[40] = {0};
        byte_to_bits(&in_port, 1, in_port_arr);

        CLOG("[IOEX] %s IOEX%d_%d_%x, out port = 0x%02X - %s, in port = 0x%02X - %s\n", dir, IOEXP_NUMBER(IOExp_number), port, pin, PIN_VALUE(out_port, pin), out_port_arr, PIN_VALUE(in_port, pin), in_port_arr);
        value = (((unsigned char)pin == 0xff) ? (ucRdBuff[0] & pin) : (ucRdBuff[0] > pin)); //REVIEW: test this logic
        CLOG("[IOEX] Reading Operation, Reading value = %c \r\n",value);
    } 
    else {
        WLOG("[IOEX] Invalid dir %s selected!!\n", dir);
    }

    //If No Direction is Given
    ucRdBuff[0] = 0;
    i2c_read_bytes(stI2c, GET_IO_CFG_PORT_REG_ADDR(port), ucRdBuff, 1); //Read all 8 register of the io expander
    //CLOG("[IOEX] Read value cfg[%d]: 0x%02X\n", GET_IO_CFG_PORT_REG_ADDR(port), ucRdBuff[0]);
    ucRdBuff[0] = 0;
    i2c_read_bytes(stI2c, GET_IO_OUT_PORT_REG_ADDR(port), ucRdBuff, 1); //Read all 8 register of the io expander
    //CLOG("[IOEX] Read value Out[%d]: 0x%02X\n", GET_IO_OUT_PORT_REG_ADDR(port), ucRdBuff[0]);

    //Deinitialize the IOExpander
    IOExp_deinit();
    // (void) oldRead;
    return value;
}

/**
 * @brief                   ioexpander CLI command
 * 
 * @param argc              (ARGument Count) is int and stores number of command-line arguments passed by the user including the name of the program. 
 *                          So if we pass a value to a program, value of argc would be 2 (one for argument and one for program name) 
 *                          The value of argc should be non negative.
 * @param argv              (ARGument Vector) is array of character pointers listing all the arguments.
                            If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
                            Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.
 * @param additional_arg    Any additional arguments required
 */
void ioexpander_cmd(int argc, char **argv, void *additional_arg) {
    if (argc < IO_EXP_MIN_ARG_REQ) {
        // WLOG("[IOEX] Less number of argument/s provided for IOExp Cmd!!\n");
        ioexpander_help();
        return;
    }

    int IOExpSelection = 0;
    //Parsing of the arguments
    IOExpSelection = atoi(argv[1]);
    int port = atoi(argv[2]);
    int pin = htoi(argv[3]);
    char *dir = argv[4];
    int value = htoi(argv[5]);
    // char ucWrBuff[10] = {0};
    // char ucRdBuff[10] = {0};

    DLOG("[IOEX] Expander selected = %d\n", IOExpSelection);
    DLOG("[IOEX] Port = %d\n", port);
    DLOG("[IOEX] Pin = %x\n", pin);
    DLOG("[IOEX] dir = %s\n", dir);
    DLOG("[IOEX] Value = %x\n", value);

    if (port > 1 || port < 0) {
        WLOG("[IOEX] Invalid port!!!\n");
        ioexpander_help();
        return;
    }

    if (((pin > 7) && (pin != 0xff)) || pin < 0) {
        WLOG("[IOEX] Invalid Pin!!!\n");
        ioexpander_help();
        return;
    }

    //Apply it on IO expander registers
    ioexpander_write_read(config.IOExpDev, IOEXP_INDEX_NUMBER(IOExpSelection), dir, port, pin, (char)value);
}
