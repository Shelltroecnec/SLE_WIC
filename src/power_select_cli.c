#include "headers.h"

int pwr_select_cb(int pwr_sel){
    int retval;
    retval = gpio_write_read(PWR_GPIO, GPIO_DIR_OUT, pwr_sel);
    return retval;
}

void pwrsel_help(void){
    CLOG("Power select command Usage:\n");
    CLOG("   %s -c PWR [Option] \n", exec_filename);
    CLOG("      option:\n");
    CLOG("      0: Switch off Smartio\n");
    CLOG("      1: Switch on Smartio\n");
}

void pwrsel_cmd(int argc, char **argv, void *additional_arg) {
    if (argc < PWRSEL_MIN_ARG_REQ) {
        // CLOG("[MOT] Less number of argument/s provided for MOT Cmd!!\n");
        pwrsel_help();
        return;
    }
    int state;
    int pwr_sel = atoi(argv[1]);
    if((pwr_sel < 0) || (pwr_sel > 2)){
        CLOG("Invalid power option selected\n");
    }
    state = pwr_select_cb(pwr_sel);
    if (state == 0)
    {
        ILOG("GPIO low\n\r");
    }
    else
    {
        ILOG("GPIO High\n\r");
    }

}
