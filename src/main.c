/**
 * @file main.c
 * @author Fakhruddin Khan (f.khan@acevin.com)
 * @brief 
 * @version 0.1
 * @date 2021-08-11
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#include "headers.h"


pthread_t tcp_server_tid;
pthread_t digital_tid;
pthread_t fifo_server_tid;
int fn_test_callback(char *ptr, void *args) {
    if (ptr != NULL) {
        ILOG("exiting from callback %s\n", ptr);
        return 1;
    }
    return 0;
}

void test_eeprom(int argc, char **argv, void *additional_arg);
void eeprom_help(void);

/**
 * @brief 
 * 
 * @param argc is the number of arguments being passed into your program from the command line 
 * @param argv is the array of arguments.
 * @return int 
 */
int main(int argc, char **argv) {
    exec_filename = argv[0]; // name of executeable for help print....

    // CLOG("argc: %d\r\n",argc);
    // Initialize the JSON based configuration
    config_startup();


    //Create the shared memory object if it is not already created, if already present open it as O_RDWR mode
    shared_mem_init(SHARED_MEM_FILE_NAME); 
  
    // Register CLI functions.....
    cli_register_cmd("GPIO", "Test GPIO", GPIO_MIN_ARG_REQ, &gpio_help, &gpio_cmd, NULL);
    cli_register_cmd("AI", "Test Analog sensor", AIN_MIN_ARG_REQ, &ain_help, &ain_cmd, NULL);
    cli_register_cmd("RTD", "Test RTD sensor", RTD_MIN_ARG_REQ, &rtd_help, &rtd_cmd, NULL);
    cli_register_cmd("DI", "Test Digital Inputs", DIN_MIN_ARG_REQ, &din_help, &din_cmd, NULL);
    cli_register_cmd("DIGOUT", "Test Digital Outputs", DIGOUT_MIN_ARG_REQ, &digital_out_help, &digital_out_cmd, NULL);
    cli_register_cmd("DAC", "Test DAC output", DAC_MIN_ARG_REQ, &dac_help, &dac_cmd, NULL);
    cli_register_cmd("FSH", "Test Flash output", FLASH_MIN_ARG_REQ, &flash_help, &flash_cmd, NULL);
    cli_register_cmd("IOEX", "Test IOExpander", IO_EXP_MIN_ARG_REQ, &ioexpander_help, &ioexpander_cmd, NULL);
    cli_register_cmd("RLY", "Test Relays ", RLY_MIN_ARG_REQ, &relay_help, &relay_cmd, NULL);
    cli_register_cmd("SD", "Test SD card ", SD_MIN_ARG_REQ, &sd_card_help, &sd_card_cmd, NULL);
    cli_register_cmd("SER", "Test Serial Interface(RS485/RS232)", SER_MIN_ARG_REQ, &ser_help, &ser_cmd, NULL);
    // cli_register_cmd("MOD", "Test Modbus communication", MOD_MIN_ARG_REQ, &mod_help, &mod_cmd, NULL);
    cli_register_cmd("SIM", "Test Simcard communication", SIM_MIN_ARG_REQ, &simcard_help, &simcard_cmd, NULL);
    cli_register_cmd("PWR", "Switch Power selection", PWRSEL_MIN_ARG_REQ, &pwrsel_help, &pwrsel_cmd, NULL);
    cli_register_cmd("LVDS", "Test LVDS image", LVDS_MIN_ARG_REQ, &lvds_help, &lvds_cmd, NULL);
    //cli_register_cmd("EROM", "Test EEPROM ", 1, &eeprom_help, &test_eeprom, NULL);
    cli_register_cmd("NORD", "Test NORDIC ", NORDIC_MIN_ARG_REQ, &nordic_help, &nordic_cmd, NULL);
    cli_register_cmd("AO", "Test Analog Output", ANO_MIN_ARG_REQ, &analog_out_help, &analog_out_cmd, NULL);
    cli_register_cmd("MOT", "Test Motor Driver", MOT_MIN_ARG_REQ, &motor_help, &motor_cmd, NULL);
    cli_register_cmd("QSPIF", "Test QSPI Flash", QSPIF_MIN_ARG_REQ, &qspi_flash_help, &qspi_flash_cmd, NULL);
    cli_register_cmd("USB", "Test USB  Hub", USB_MIN_ARG_REQ, &usb_help, &usb_cmd, NULL);
    // cli_register_cmd("TDC", "Test TDC", TDC_MIN_ARG_REQ, &tdc_help, &tdc_cmd, NULL);
    cli_register_cmd("RAM", "Test IMX RAM", IMX_RAM_MIN_ARG_REQ, &imx_ram_help, &imx_ram_cmd, NULL);
    // cli_register_cmd("PWM", "Test PWM", PWM_MIN_ARG_REQ, &pwm_help, &pwm_cmd, NULL);
    cli_register_cmd("EMMC", "Test EMMC Flash", EMMC_FLASH_ARG_REQ, &emmc_flash_help, &emmc_flash_cmd, NULL);


    opts_parser(argc, argv);
    // void *pAddr = NULL;
    // int *ptr = NULL;
    // char pst_ai_sensor->last_power_selected[10] = {0};
    // int ret = -1;
    // char *cptr = NULL;
    // shared_mem_mutex_lock();
    // DLOG("In lock: %d\n", stMy_data->st_shm_data.last_power_selected);
    // stMy_data->st_shm_data.last_power_selected += 1;
    // sleep(2);
    // shared_mem_mutex_unlock();

    shared_mem_deinit();
    
}

void eeprom_help(void) {
    CLOG("   EROM command Usage:\n");
    CLOG("       %s -c EROM \n", exec_filename);
    CLOG("          Test the EROM\n");
}

void test_eeprom(int argc, char **argv, void *additional_arg) {
    sti2c_t *i2c = i2c_new();

    i2c->addr = 0x50;
    i2c->filename = config.eeprom_dev;

    ILOG("EROM Test dev: %s\n", i2c->filename);
    uint8_t ucWrBuff[10], ucRdBuff[10];
    /*
	 * Open the I2C device.
	 */
    ILOG("Opening i2C: %s\n", i2c->filename);

    if (i2c_open(i2c, i2c->filename) < 0) {
        ELOG("i2c_open(): %s\n", i2c_errmsg(i2c));
        exit(1);
    }

    ucWrBuff[0] = 0xAA;
    ILOG("Writing i2C: %d @0x05 location\n", ucWrBuff[0]);

    //Write 0xAA on memory address 0x05
    i2c_write_bytes(i2c, 0x05, ucWrBuff, 1);
    ILOG("i2c_write_bytes %d\n", ucWrBuff[0]);

    i2c_read_bytes(i2c, 0x05, ucRdBuff, 1);
    ILOG("i2c_read_bytes: %d\n", ucRdBuff[0]);
}

// struct timeval tv;
// /* Wait up to five seconds. */
// tv.tv_sec = 0;
// tv.tv_usec = 100000;
// check_stdin_cb(&tv, &fn_test_callback, NULL);