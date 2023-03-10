#include "headers.h"
#include "sched.h"


st_mod_sample_t st_mod_sample;
/**
 * @brief           Modbus helper function
 * 
 */
void mod_help(void) {
    CLOG("MOD command Usage:\n");
    CLOG("   %s -c MOD [CH] [BR] [FRAME]\n", exec_filename);
    CLOG("      CH - Select multiplexer channel\n");
    CLOG("           1 - Serial channel 1\n");
    CLOG("           2 - Serial channel 2\n");
    CLOG("           3 - Serial channel 3\n");
    CLOG("      BR - Baudrate configuration[bps]\n");
    CLOG("      FRAME - Modbus Frame in Hex format (No need of 0x before)\n");
}

/**
 * @brief               This function selects the mode and dir or speed for serial communication
 * 
 * @param mode          Serial mode SER_RS485_MODE/SER_RS232_MODE
 * @param channel       Modbus channel 
 * @param dir_speed     Baudrate
 */
void mod_enable_channel(char channel, char dir_speed) {

    //Setting RS485 or RS232 mode
    // if (mode == SER_RS485_MODE) {
    digital_pin_out_write(config.ser_rs485_232_select_pin, SET);   // in RS485 mode
    // } else if (mode == SER_RS232_MODE) {
    //     digital_pin_out_write(config.ser_rs485_232_select_pin, RESET); // in RS232 mode
    // } else {
    //     ILOG("[MOD] invalid RS485/RS232 mode: %d", mode);
    // }

    //Speed and RS485 Tx/Rx mode setting -- this should be used by the serial driver and toggling these pins when tx starts and stops
    // if (dir_speed == RS485_TRANSMIT_DIR) {
    //     digital_pin_out_write(config.ser_rs485_d_fast_en_pin, SET); // Tx enable pin
    //     digital_pin_out_write(config.ser_rs485_r_en_pin, SET);      // Rx disable  pin
    // } else if (dir_speed == RS485_RECEIVE_DIR) {
    //     digital_pin_out_write(config.ser_rs485_d_fast_en_pin, RESET); // Tx disable pin
    //     digital_pin_out_write(config.ser_rs485_r_en_pin, RESET);      // Rx enable pin
    // } else if (mode == SER_RS232_MODE && dir_speed == RS232_SPEED_1MBPS) {
    //     digital_pin_out_write(config.ser_rs485_d_fast_en_pin, SET); // 1MBPS
    //     digital_pin_out_write(config.ser_rs485_r_en_pin, RESET);    // Rx enable dont care here
    // } else if (mode == SER_RS232_MODE && dir_speed == RS232_SPEED_250KBPS) {
    //     digital_pin_out_write(config.ser_rs485_d_fast_en_pin, RESET); // 250KBPS
    //     digital_pin_out_write(config.ser_rs485_r_en_pin, RESET);      // Rx enable dont care here
    // }

    //channel selection for connecting with UART3
    if (channel == SER_CHANNEL_1) {
        digital_pin_out_write(config.ser_mux_S0_pin, RESET); // channel select bit0 - 0
        digital_pin_out_write(config.ser_mux_S1_pin, RESET); // channel select bit1 - 0
    } else if (channel == SER_CHANNEL_2) {
        digital_pin_out_write(config.ser_mux_S0_pin, SET);   // channel select bit0 - 1
        digital_pin_out_write(config.ser_mux_S1_pin, RESET); // channel select bit1 - 0
    } else if (channel == SER_CHANNEL_3) {
        digital_pin_out_write(config.ser_mux_S0_pin, RESET); // channel select bit0 - 0
        digital_pin_out_write(config.ser_mux_S1_pin, SET);   // channel select bit1 - 1
    } else {
        CLOG("Invalid Serial Mux Channel\n");
    }
    
    // Enable the Serial Mux
    // digital_pin_in_read(config.ser_rs485_232_mux_en_pin); //Mux enable is pulled up and hence keeping it input so that it wont affect the enable pin
}


// void ser_loopback_test(serial_t *serial) {
//     uint8_t transmit[] = TX_data;
//     uint8_t receive[128];
//     int ret;

//     CLOG("Write Bytes: %d String: _%s_\n", (int)sizeof(transmit), transmit);
//     /* Write to the serial port */
//     if (serial_write(serial, transmit, sizeof(transmit)) < 0) {
//         fprintf(stderr, "serial_write(): %s\n", serial_errmsg(serial));
//         exit(1);
//     }

//     /* Read up to receive size or 2000ms timeout */
//     if ((ret = serial_read(serial, receive, sizeof(transmit), 2000)) < 0) {
//         fprintf(stderr, "serial_read(): %s\n", serial_errmsg(serial));
//         exit(1);
//     }

//     CLOG("Read  Bytes: %d String: _%s_\n", ret, receive);

//     if (strcmp((const char *)transmit, (const char *)receive) == 0) {
//         CLOG("Result: Loopback Tess Pass\n");
//     } else {
//         CLOG("Result: Loopback Tess Fail\n");
//     }
// }


/**
 * @description: Calculates CRC16 for modbus query
 * @param {uint8_t} *a : String of bytes of which CRC is to be calculated
 * @param {uint8_t} len : Length of string
 * @return {uint16_t} : 16 bit MODBUS CRC
 */
uint16_t fnCalcCRC(uint8_t *str, uint8_t len)
{
    int i, j;
    uint16_t crc = 0xFFFF;
    for (j = 0; j < len; j++)
    {
        crc ^= (uint16_t)str[j];
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

int read_modbus(st_mod_sample_t *st_mod_sample){
    
    uint16_t crc = 0;
    CLOG("ch: %d\n", st_mod_sample->Mod_channel);
    CLOG("baudrate: %d\n",  st_mod_sample->baudrate);
    CLOG("TX_data: %s\n", st_mod_sample->TX_data);

    shared_mem_mutex_lock(&stMy_data->stModbus_lock);
    st_mod_sample->tx_len = convert_byte_array(st_mod_sample->TX_data, st_mod_sample->TX_data, 256);
    crc = fnCalcCRC(st_mod_sample->TX_data, st_mod_sample->tx_len);
    // CLOG("CRC= %x\n",crc);
    st_mod_sample->TX_data[st_mod_sample->tx_len++] = crc & 0xFF;
    st_mod_sample->TX_data[st_mod_sample->tx_len++] = crc >> 8 & 0xFF;
    
    CLOG("MB Request: "); 
    print_byte_array(st_mod_sample->TX_data, st_mod_sample->tx_len);
    struct sched_param param;
    sched_getparam(0, &param);
    // DLOG("default priority: %d\n", param.sched_priority);

    param.sched_priority = 99;
     sched_setparam(0, &param);
    // DLOG("New priority: %d\n", param.sched_priority);


    sched_setscheduler(0, SCHED_FIFO, &param);
    if (st_mod_sample->Mod_channel > 3 || st_mod_sample->Mod_channel < 1) {
        CLOG("[MOD] Invalid mode(RS485/RS232)....\n");
        mod_help();
        // return;
    }

    int time_to_tx = ((int)(1000000.0f/(float)st_mod_sample->baudrate) * st_mod_sample->tx_len * 11);
    serial_t *serial;
    
    int ret;

    //DLOG("float : %f, int %d\n", (float)(1000000.0f/(float)baudrate), (int)(1000000.0f/(float)baudrate));

    CLOG("Selecting channel %d for RS485 serial test, BR: %d, time to sleep: %d\n", st_mod_sample->Mod_channel, st_mod_sample->baudrate, time_to_tx);

    if (st_mod_sample->Mod_channel == SER_CHANNEL_1) {
        digital_pin_out_write(config.ser_mux_S0_pin, RESET); // channel select bit0 - 0
        digital_pin_out_write(config.ser_mux_S1_pin, RESET); // channel select bit1 - 0
    } else if (st_mod_sample->Mod_channel == SER_CHANNEL_2) {
        digital_pin_out_write(config.ser_mux_S0_pin, SET);   // channel select bit0 - 1
        digital_pin_out_write(config.ser_mux_S1_pin, RESET); // channel select bit1 - 0
    } else if (st_mod_sample->Mod_channel == SER_CHANNEL_3) {
        digital_pin_out_write(config.ser_mux_S0_pin, RESET); // channel select bit0 - 0
        digital_pin_out_write(config.ser_mux_S1_pin, SET);   // channel select bit1 - 1
    } else {
        CLOG("Invalid Serial Mux Channel\n");
    }
    if(config.modbus_workaround_flag == 1){
        CLOG("Select pin and Mux reset enabled\n");
        digital_pin_out_write(config.ser_rs485_232_select_pin, RESET);
        digital_pin_out_write(config.ser_rs485_232_mux_en_pin, RESET);
        usleep(config.modbus_delay_ms*1000);
    }
    
    // mod_enable_channel(channel, RS485_TRANSMIT_DIR);
    digital_pin_out_write(config.ser_rs485_232_select_pin, SET);   // in RS485 mode
    digital_pin_out_write(config.ser_rs485_d_fast_en_pin, SET); // Tx disable pin
    digital_pin_out_write(config.ser_rs485_r_en_pin, SET);      // Rx enable pin

    // CLOG("Mux enable: %d\n", digital_pin_in_read(config.ser_rs485_232_mux_en_pin)); //Mux enable is pulled up and hence keeping it input so that it wont affect the enable pin

    // DLOG("2\r\n");

    //RS485 in Tx mode
    serial = serial_new();
    // digital_pin_out_write(config.ser_rs485_d_fast_en_pin, SET); // Tx enable pin
    // digital_pin_out_write(config.ser_rs485_r_en_pin, SET); 
    /* Open config.ser_rs485_232_dev with baudrate 115200, and defaults of 8N1, no flow control */
    if (serial_open(serial, config.ser_rs485_232_dev, st_mod_sample->baudrate) < 0) {
        fprintf(stderr, "serial_open(): %s\n", serial_errmsg(serial));
        exit(1);
    }
    
    // DLOG("3\r\n");

    /* Write to the serial port */
    if (serial_write(serial, st_mod_sample->TX_data, st_mod_sample->tx_len) < 0) {
        fprintf(stderr, "serial_write(): %s\n", serial_errmsg(serial));
        exit(1);
    }

    usleep(time_to_tx+1000); //wait for the write to complete

    //RS485 in Rx mode
    digital_pin_out_write(config.ser_rs485_d_fast_en_pin, RESET); // Tx disable pin
    digital_pin_out_write(config.ser_rs485_r_en_pin, RESET);      // Rx enable pin
    //mod_enable_channel(channel, RS485_RECEIVE_DIR);
    memset(st_mod_sample->RX_data, '\0', 256);
    /* Read up to receive size or 10 seconds timeout */
    if ((ret = serial_read(serial, st_mod_sample->RX_data, 256, RX_TIMEOUT)) < 0) {
        fprintf(stderr, "serial_read(): %s\n", serial_errmsg(serial));
        exit(1);
    }
    
    CLOG("\nModbus received[%d]: ", ret);
    // st_mod_sample->RX_data[ret] = '\0';
    // CLOG("\nRX_Data: %s\n", st_mod_sample->RX_data);
    print_byte_array(st_mod_sample->RX_data,ret); 
    st_mod_sample->rx_len = ret;
    serial_close(serial);
    
    serial_free(serial);
    shared_mem_mutex_unlock(&stMy_data->stModbus_lock);
    CLOG("UNLOCKED\n");
    return ret;

}                       
/**                     
 * @brief                   Modbus CLI command
 *                      
 * @param argc              (ARGument Count) is int and stores number of command-line arguments passed by the user including the name of the program. 
 *                          So if we pass a value to a program, value of argc would be 2 (one for argument and one for program name) 
 *                          The value of argc should be non negative.
 * @param argv              (ARGument Vector) is array of character pointers listing all the arguments.
                            If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
                            Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.
 * @param additional_arg    Any additional arguments required
 */                     
void mod_cmd(int argc, char **argv, void *additional_arg) {
    // char user_input[128];

    if (argc < MOD_MIN_ARG_REQ) {
        CLOG("[MOD] Less number of argument/s provided for SER Cmd!!\n");
        mod_help();     
        return;         
    }                   
    st_mod_sample.Mod_channel = atoi(argv[1]);
    st_mod_sample.baudrate = atoi(argv[2]);
    memcpy(st_mod_sample.TX_data,argv[3],strlen(argv[3]));
    // st_mod_sample->TX_data[256] = argv[3];


    read_modbus(&st_mod_sample);
    // int st_mod_sample->tx_len = 0;  
    // uint16_t crc = 0;
    // shared_mem_mutex_lock(&stMy_data->stModbus_lock);
    // CLOG("LOCKED\n");
    // st_mod_sample->tx_len = convert_byte_array(argv[3], TX_data, 256);
    // // print_byte_array(TX_data, st_mod_sample->tx_len);
    // // DLOG("len = %d, Str: %s\r\n", st_mod_sample->tx_len, argv[3]);
    // // DLOG("len = %d\r\n", st_mod_sample->tx_len);
    // crc = fnCalcCRC(TX_data, st_mod_sample->tx_len);
    // // DLOG("CRC = %04x\r\n", crc);
                        
    // TX_data[st_mod_sample->tx_len++] = crc & 0xFF;
    // TX_data[st_mod_sample->tx_len++] = crc >> 8 & 0xFF;

    // CLOG("MB Request: "); 
    // print_byte_array(TX_data, st_mod_sample->tx_len);
    // struct sched_param param;

    // sched_getparam(0, &param);
    // // DLOG("default priority: %d\n", param.sched_priority);

    // param.sched_priority = 99;
    //  sched_setparam(0, &param);
    // // DLOG("New priority: %d\n", param.sched_priority);

                        
    // sched_setscheduler(0, SCHED_FIFO, &param);
    // if (channel > 3 || channel < 1) {
    //     CLOG("[MOD] Invalid mode(RS485/RS232)....\n");
    //     mod_help();
    //     return;
    // }

    // int time_to_tx = ((int)(1000000.0f/(float)baudrate) * st_mod_sample->tx_len * 11);
    // serial_t *serial;
    
    // int ret;

    // //DLOG("float : %f, int %d\n", (float)(1000000.0f/(float)baudrate), (int)(1000000.0f/(float)baudrate));

    // CLOG("Selecting channel %d for RS485 serial test, BR: %d, time to sleep: %d\n", channel, baudrate, time_to_tx);

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
    // if(config.modbus_select_pin == 1){
    //     CLOG("Select pin and Mux reset enabled\n");
    //     digital_pin_out_write(config.ser_rs485_232_select_pin, RESET);
    //     digital_pin_out_write(config.ser_rs485_232_mux_en_pin, RESET);
    //     usleep(config.modbus_delay_ms*1000);
    // }
    
    // // mod_enable_channel(channel, RS485_TRANSMIT_DIR);
    // digital_pin_out_write(config.ser_rs485_232_select_pin, SET);   // in RS485 mode
    // digital_pin_out_write(config.ser_rs485_d_fast_en_pin, SET); // Tx disable pin
    // digital_pin_out_write(config.ser_rs485_r_en_pin, SET);      // Rx enable pin

    // // CLOG("Mux enable: %d\n", digital_pin_in_read(config.ser_rs485_232_mux_en_pin)); //Mux enable is pulled up and hence keeping it input so that it wont affect the enable pin

    // // DLOG("2\r\n");

    // //RS485 in Tx mode
    // serial = serial_new();
    // // digital_pin_out_write(config.ser_rs485_d_fast_en_pin, SET); // Tx enable pin
    // // digital_pin_out_write(config.ser_rs485_r_en_pin, SET); 
    // /* Open config.ser_rs485_232_dev with baudrate 115200, and defaults of 8N1, no flow control */
    // if (serial_open(serial, config.ser_rs485_232_dev, baudrate) < 0) {
    //     fprintf(stderr, "serial_open(): %s\n", serial_errmsg(serial));
    //     exit(1);
    // }
    
    // // DLOG("3\r\n");

    // /* Write to the serial port */
    // if (serial_write(serial, TX_data, st_mod_sample->tx_len) < 0) {
    //     fprintf(stderr, "serial_write(): %s\n", serial_errmsg(serial));
    //     exit(1);
    // }

    // usleep(time_to_tx+1000); //wait for the write to complete

    // //RS485 in Rx mode
    // digital_pin_out_write(config.ser_rs485_d_fast_en_pin, RESET); // Tx disable pin
    // digital_pin_out_write(config.ser_rs485_r_en_pin, RESET);      // Rx enable pin
    // //mod_enable_channel(channel, RS485_RECEIVE_DIR);

    // /* Read up to receive size or 10 seconds timeout */
    // if ((ret = serial_read(serial, TX_data, 256, RX_TIMEOUT)) < 0) {
    //     fprintf(stderr, "serial_read(): %s\n", serial_errmsg(serial));
    //     exit(1);
    // }
    
    // CLOG("Modbus received[%d]: ", ret);
    // TX_data[ret] = '\0';
    // print_byte_array(TX_data,ret); 

    // serial_close(serial);
    
    // serial_free(serial);
    // shared_mem_mutex_unlock(&stMy_data->stModbus_lock);
    // CLOG("UNLOCKED\n");
}