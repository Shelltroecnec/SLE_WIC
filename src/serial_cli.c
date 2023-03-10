#include "headers.h"

/**
 * @brief Serial cli helper function
 * 
 */
void ser_help(void) {
    CLOG("SER command Usage:\n");
    CLOG("   %s -c SER [MODE] \n", exec_filename);
    CLOG("      MODE - SER Mode selection\n");
    CLOG("           0 - RS232 test mode\n");
    CLOG("           1 - RS485 test mode\n");
    // CLOG("      CH - Select multiplexxer channel\n");
    // CLOG("           1 - Serial channel 1\n");
    // CLOG("           2 - Serial channel 2\n");
    // CLOG("           3 - Serial channel 3\n");
}

/**
 * @brief               This function selects the mode and dir or speed for serial communication
 * 
 * @param mode          Serial mode SER_RS485_MODE/SER_RS232_MODE
 * @param channel       Modbus channel
 * @param dir_speed     Baudrate for modbus
 */
void ser_enable_channel(char mode, char dir_speed) {

    //Setting RS485 or RS232 mode
    // if (mode == SER_RS485_MODE) {
    //     digital_pin_out_write(config.ser_rs485_232_select_pin, SET);   // in RS232 mode
    // } else if (mode == SER_RS232_MODE) {
    //     digital_pin_out_write(config.ser_rs485_232_select_pin, RESET); // in RS232 mode
    // } else {
    //     ILOG("[SER] invalid RS485/RS232 mode: %d", mode);
    // }

    // Speed and RS485 Tx/Rx mode setting -- this should be used by the serial driver and toggling these pins when tx starts and stops
    if (mode == SER_RS485_MODE && dir_speed == RS485_TRANSMIT_DIR) {
        digital_pin_out_write(config.ser_rs485_d_fast_en_pin, SET); // Tx enable pin
        digital_pin_out_write(config.ser_rs485_r_en_pin, SET);      // Rx disable  pin
    } else if (mode == SER_RS485_MODE && dir_speed == RS485_RECEIVE_DIR) {
        digital_pin_out_write(config.ser_rs485_d_fast_en_pin, RESET); // Tx disable pin
        digital_pin_out_write(config.ser_rs485_r_en_pin, RESET);      // Rx enable pin
    } else if (mode == SER_RS232_MODE && dir_speed == RS232_SPEED_1MBPS) {
        digital_pin_out_write(config.ser_rs485_d_fast_en_pin, SET); // 1MBPS
        digital_pin_out_write(config.ser_rs485_r_en_pin, RESET);    // Rx enable dont care here
    } else if (mode == SER_RS232_MODE && dir_speed == RS232_SPEED_250KBPS) {
        digital_pin_out_write(config.ser_rs485_d_fast_en_pin, RESET); // 250KBPS
        digital_pin_out_write(config.ser_rs485_r_en_pin, RESET);      // Rx enable dont care here
    }

    //channel selection for connecting with UART3
    // if (channel == SER_CHANNEL_1) {
    //     digital_pin_out_write(config.ser_mux_S0_pin, RESET); // channel select bit0 - 0
    //     digital_pin_out_write(config.ser_mux_S1_pin, RESET); // channel select bit1 - 0
    // } else if (channel == SER_CHANNEL_2) {
    //     digital_pin_out_write(config.ser_mux_S0_pin, SET);   // channel select bit0 - 1
    //     digital_pin_out_write(config.ser_mux_S1_pin, RESET); // channel select bit1 - 0
    // } else if (channel == SER_CHANNEL_3) {
    //     digital_pin_out_write(config.ser_mux_S0_pin, RESET); // channel select bit0 - 0
    //     digital_pin_out_write(config.ser_mux_S1_pin, SET);   // channel select bit1 - 1
    // } else {
    //     CLOG("Invalid Serial Mux Channel\n");
    // }   

    // Enable the Serial Mux
    digital_pin_in_read(config.ser_rs485_232_mux_en_pin); //Mux enable is pulled up and hence keeping it input so that it wont affect the enable pin
}


void ser_loopback_test(serial_t *serial) {
    uint8_t transmit[] = "Hello SmartIO, RS232";
    uint8_t receive[128];
    int ret;

    CLOG("Write Bytes: %d String: _%s_\n", (int)sizeof(transmit), transmit);
    /* Write to the serial port */
    if (serial_write(serial, transmit, sizeof(transmit)) < 0) {
        fprintf(stderr, "serial_write(): %s\n", serial_errmsg(serial));
        exit(1);
    }

    /* Read up to receive size or 2000ms timeout */
    if ((ret = serial_read(serial, receive, sizeof(transmit), 2000)) < 0) {
        fprintf(stderr, "serial_read(): %s\n", serial_errmsg(serial));
        exit(1);
    }

    CLOG("Read  Bytes: %d String: _%s_\n", ret, receive);

    if (strcmp((const char *)transmit, (const char *)receive) == 0) {
        CLOG("Result: Loopback Tess Pass\n");
    } else {
        CLOG("Result: Loopback Tess Fail\n");
    }
}

/**
 * @brief                   Serial CLI command
 * 
 * @param argc              (ARGument Count) is int and stores number of command-line arguments passed by the user including the name of the program. 
 *                          So if we pass a value to a program, value of argc would be 2 (one for argument and one for program name) 
 *                          The value of argc should be non negative.
 * @param argv              (ARGument Vector) is array of character pointers listing all the arguments.
                            If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
                            Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.
 * @param additional_arg    Any additional arguments required
 */
void ser_cmd(int argc, char **argv, void *additional_arg) {
    // char user_input[128];
    if (argc < SER_MIN_ARG_REQ) {
        // CLOG("[SER] Less number of argument/s provided for SER Cmd!!\n");
        ser_help();
        return;
    }
    char ser_mode = atoi(argv[1]);
    // char channel = atoi(argv[2]);
    if (ser_mode > 1) {
        CLOG("[SER] Invalid mode(RS485/RS232)....\n");
        ser_help();
        return;
    }
    
    // if (channel > 3 || channel < 1) {
    //     CLOG("[SER] Invalid number of channels, select between 1-3....\n");
    //     ser_help();
    //     return;
    // }

    serial_t *serial;
    char ch = 0, ch1 = 0;
    int ret;

    serial = serial_new();

    /* Open config.ser_rs485_232_dev with baudrate 115200, and defaults of 8N1, no flow control */
    if (serial_open(serial, config.ser_rs485_232_dev, 115200) < 0) {
        fprintf(stderr, "serial_open(): %s\n", serial_errmsg(serial));
        exit(1);
    }

    if (ser_mode == SER_RS232_MODE) {
        CLOG("[SER] Please make sure switch SW5 is towards RS232...\n");
        // CLOG("Selecting channel %d as RS232\n", channel);
        // CLOG("\rInstall Loopback test setup on channel %d and hit entered:", channel);
        fflush(stdin);
        // while ((ch = getchar()) != '\n' && ch != (char)EOF);
        ch = getchar();
        // ser_enable_channel(ser_mode, RS232_SPEED_250KBPS);
        // usleep(100000);
        ser_loopback_test(serial);
    } else { // RS485 mode
        CLOG("[SER] Please make sure switch SW5 is towards RS485...\n");
        CLOG("[SER] Make sure SW7 termination is enabled\n");

        // CLOG("Selecting channel %d for RS485 serial test\n", channel);

        // ser_enable_channel(ser_mode, channel, RS485_TRANSMIT_DIR);
        uint8_t rs485_test[100] = "Hello SmartIO, RS485";

        /* Write to the serial port */
        if (serial_write(serial, (const uint8_t *)rs485_test, strlen((const char *)rs485_test) + 1) < 0) {
            fprintf(stderr, "serial_write(): %s\n", serial_errmsg(serial));
            exit(1);
        }

        usleep(100000); //wait for the write to complete

        CLOG("Sent \"%s\" over RS485, did you received it? [Y/n] ", rs485_test);
        ch = getchar();
        while ((ch != '\n') && ((ch1 = getchar()) != '\n') && ch1 != (char)EOF);

        if ((ch == 'Y') || (ch == 'y')) {
            CLOG("RS485 Tx test PASS!!\r\n");
            memset(rs485_test, 0, sizeof(rs485_test));
        } else if ((ch == 'N') || (ch == 'n')) {
            CLOG("RS485 Tx test FAIL\r\n");
        } 

        // CLOG("Switching RS485 in Rx mode, Send some data from channel %d\n", channel);
        // ser_enable_channel(ser_mode, RS485_RECEIVE_DIR);
        /* Read up to receive size or 15 seconds timeout */
        if ((ret = serial_read(serial, rs485_test, 10, 15000)) < 0) {
            fprintf(stderr, "serial_read(): %s\n", serial_errmsg(serial));
            exit(1);
        }
        
        DLOG("read %d bytes: %s\n", ret, rs485_test);

        if(ret){
            CLOG("Did you send \"%s\"? [Y/n] ",rs485_test);
            ch = getchar();
            while ((ch != '\n') && ((ch1 = getchar()) != '\n') && ch1 != (char)EOF);

            if ((ch == 'Y') || (ch == 'y')) {
                CLOG("RS485 Rx test PASS!!\r\n");
                memset(rs485_test, 0, sizeof(rs485_test));
            } else if ((ch == 'N') || (ch == 'n')) {
                CLOG("RS485 rx test FAIL\r\n");
            } 

        } else {
            CLOG("Serial read timeout. RS485 Rx test FAIL\r\n");
        }
        
    }
    serial_close(serial);
    serial_free(serial);
}