#include "headers.h"
#include "mcp48fvb22.h"
//TODO: Need to see if it is implemented as per CLI
typedef struct
{
    spi_t *spi;
    char aout_dac_bc_inc;
    char aout_channel;
    int16_t aout_dac_curr_bc;
    char flag_ramp_state;
} st_analog_out_cb_t;

/**
 * @brief           Analog out helper function
 * 
 */
void analog_out_help(void) {
    CLOG("AO command Usage:\n");
    CLOG("   %s -c AO [DAC_CH] [OUT_STATE] [UNIT/UDELAY] [VALUE]\n", exec_filename);
    CLOG("      DAC_CH - DAC output channel selection:\n");
    CLOG("           1 - ouput channel 1\n");
    CLOG("           2 - ouput channel 2\n");
    CLOG("      OUT_STATE - DAC output selection:\n");
    CLOG("           0 - Ramp\n");
    CLOG("           1 - Static\n");
    //TODO: update below logic in the below processing
    // CLOG("      [UNIT/UDELAY] - if Unit of output or update delay for ramping\n");
    // CLOG("           DAC output update period [%d-%d] msec (if OUT_STATE as Ramp)\n", MIN_DAC_WR_PRD, MAX_DAC_WR_PRD);
    // CLOG("           DAC output unit Options (if OUT_STATE as Static)\n");
    // CLOG("              %d - Voltage output\n", OUTPUT_VOLTAGE);
    // CLOG("              %d - Current output\n", OUTPUT_CURRENT);
    // CLOG("      [VALUE] - Output current or voltage (if OUT_STATE as Static) or \n");
    // CLOG("                DAC count incremented/decremented on each time (if OUT_STATE as Static)\n");

}

/**
 * @brief           Analalog out callback function
 * 
 * @param ptr       Character pointer to the function, needs to be NULL to callback else it will exit
 * @param args      Argument to be passed
 * @return int8_t   return 0 on success, non 0 on failure 
 */
int aout_callback(char *ptr, void *args) {

    if (ptr != NULL) {
        CLOG("exiting from analog output test %s\n", ptr);
        return 1;
    }

    st_analog_out_cb_t *pst_analog_out_cb = (st_analog_out_cb_t *)args;

    if (pst_analog_out_cb->flag_ramp_state == RAMP_UP) {
        pst_analog_out_cb->aout_dac_curr_bc += pst_analog_out_cb->aout_dac_bc_inc;
        if (pst_analog_out_cb->aout_dac_curr_bc >= DAC_MAX_VALID_BIT_CNT) {
            pst_analog_out_cb->aout_dac_curr_bc = DAC_MAX_VALID_BIT_CNT;
            pst_analog_out_cb->flag_ramp_state = RAMP_DOWN;
        }
    } else if (pst_analog_out_cb->flag_ramp_state == RAMP_DOWN) {
        pst_analog_out_cb->aout_dac_curr_bc -= pst_analog_out_cb->aout_dac_bc_inc;
        if (pst_analog_out_cb->aout_dac_curr_bc <= 0) {
            pst_analog_out_cb->aout_dac_curr_bc = 0;
            pst_analog_out_cb->flag_ramp_state = RAMP_UP;
        }
    }

    if (mcp48f_dac_set_output_cnt(pst_analog_out_cb->spi, pst_analog_out_cb->aout_channel, pst_analog_out_cb->aout_dac_curr_bc) > -1) {
        CLOG("DAC Output BC = %d   Volt = %0.3f\n", pst_analog_out_cb->aout_dac_curr_bc,
             ((float)(MCP48F_REF_VOLT * pst_analog_out_cb->aout_dac_curr_bc) / MCP48F_BITS_RES));
    } else {
        WLOG("Fail to update the DAC Output for BC %d\n", pst_analog_out_cb->aout_dac_curr_bc);
    }
    return 0;
}

/**
 * @brief                   Analog out CLI command
 * 
 * @param argc              (ARGument Count) is int and stores number of command-line arguments passed by the user including the name of the program. 
 *                          So if we pass a value to a program, value of argc would be 2 (one for argument and one for program name) 
 *                          The value of argc should be non negative.
 * @param argv              (ARGument Vector) is array of character pointers listing all the arguments.
                            If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
                            Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.
 * @param additional_arg    Any additional arguments required
 */
void analog_out_cmd(int argc, char **argv, void *additional_arg) {
    char ch = 0, ch1 = 0;
    if (argc < ANO_MIN_ARG_REQ) {
        // CLOG("[AO] Less number of argument/s provided for AO Cmd!!\n");
        analog_out_help();
        return;
    }

    //parsing the inputs
    int channel = atoi(argv[1]);
    int dac_output_state = atoi(argv[2]);

    if ((channel > 2 || channel < 1)) {
        CLOG("Invalid channel!!!\n");
        analog_out_help();
        return;
    }
    if (dac_output_state > OUTPUT_STATIC || dac_output_state < OUTPUT_RAMP) {
        CLOG("Invalid Output State!!!\n");
        analog_out_help();
        return;
    }

    CLOG("selected AO channel: %d, Output State: %s\n", channel, DAC_OUT_STATE(dac_output_state));

    // uint16_t dac_bc_inc = 0;
    // uint8_t dac_write_prd = 0;

    spi_t *spi_dac = NULL;
    struct timeval tv;

    if ((spi_dac = mcp48f_dac_init(config.mcp48f_dac_dev, SPI_MODE_3, DAC_SPI_FREQ)) == NULL) {
        WLOG("AO Init failed!!! ");
    }
    if (dac_output_state == OUTPUT_RAMP) {
        CLOG("Default DAC output update period is %d msec. ", config.cli_aout_dac_wr_prd);
        CLOG("Would you like to modify it? [Y/n] ");

        ch = getchar();
        while ((ch != '\n') && ((ch1 = getchar()) != '\n') && ch1 != (char)EOF);

        if ((ch == 'Y') || (ch == 'y')) {
            CLOG("Enter the valid DAC output update period [%d-%d] msec: ", MIN_DAC_WR_PRD, MAX_DAC_WR_PRD);
            
            int delay = 0;
            scanf("%d", &delay);
            while ((ch = getchar()) != '\n' && ch != (char)EOF);
            if ((delay <= MAX_DAC_WR_PRD) && (delay >= MIN_DAC_WR_PRD)) {
                config.cli_aout_dac_wr_prd = delay; //overwriting default period
                CLOG("DAC write period is %d msec\r\n", config.cli_aout_dac_wr_prd);
            } else {
                CLOG("invalid period entered, valid range is [%d-%d] msec\r\n", MIN_DAC_WR_PRD, MAX_DAC_WR_PRD);
                exit(0);
            }
        } else if ((ch == 'N') || (ch == 'n')) {
            CLOG("DAC write period is %d msec\r\n", config.cli_aout_dac_wr_prd);
        } else {
            CLOG("invalid period, using the default write period %d\r\n", config.cli_aout_dac_wr_prd);
        }

        CLOG("Default DAC bit count increment is %d", config.cli_aout_dac_bc_inc);
        CLOG("Would you like to modify it? [Y/n] ");

        ch = getchar();
        while ((ch != '\n') && ((ch1 = getchar()) != '\n') && ch1 != (char)EOF);

        if ((ch == 'Y') || (ch == 'y')) {
            CLOG("Enter the DAC bit counts to be increment in each write period [%d-%d] sec: ", MIN_DAC_BC_INC_PRD, MAX_DAC_BC_INC_PRD);
        
            int dac_bc_inc_period = 0;
            scanf("%d", &dac_bc_inc_period);
            if ((dac_bc_inc_period <= MAX_DAC_BC_INC_PRD) && (dac_bc_inc_period >= MIN_DAC_BC_INC_PRD)) {
                config.cli_aout_dac_bc_inc = dac_bc_inc_period; //overwriting default period
                CLOG("DAC bit count increment is %d sec\r\n", config.cli_aout_dac_bc_inc);
            } else {
                CLOG("invalid value entered, valid range is [%d-%d] sec\r\n", MIN_DAC_BC_INC_PRD, MAX_DAC_BC_INC_PRD);
                exit(0);
            }
        } else if ((ch == 'N') || (ch == 'n')) {
            CLOG("DAC bit count increment is %d\r\n", config.cli_aout_dac_bc_inc);
        } else {
            CLOG("invalid option, using the default bit count %d\r\n", config.cli_aout_dac_bc_inc);
        }

        st_analog_out_cb_t st_analog_out_cb;
        st_analog_out_cb.spi = spi_dac;
        st_analog_out_cb.aout_dac_bc_inc = config.cli_aout_dac_bc_inc;
        st_analog_out_cb.aout_channel = channel;
        st_analog_out_cb.flag_ramp_state = RAMP_UP;

        //populating period
        tv.tv_sec = config.cli_aout_dac_wr_prd / 1000;
        tv.tv_usec = (config.cli_aout_dac_wr_prd % 1000) * 1000;
        
        check_stdin_cb(&tv, &aout_callback, &st_analog_out_cb);
    } else {
        int unit = 0;
        float out_value = 0;
        // uint8_t ch = 0;

        CLOG("Enter output unit [%d - Voltage, %d - Current]: ", OUTPUT_VOLTAGE, OUTPUT_CURRENT);
        scanf("%d", &unit);
        if ((unit != OUTPUT_VOLTAGE) && (unit != OUTPUT_CURRENT)) {
            CLOG("Invalid unit!!!\n");
            return;
        }

        if (unit == OUTPUT_VOLTAGE) {
            CLOG("Enter output voltage value [0-10V]: ");
        } else {
            CLOG("Enter output current value [4-20mA]: ");
        }
        scanf("%f", &out_value);

        if (((unit == OUTPUT_VOLTAGE) && (out_value > 10.0f || out_value < 0.0f)) ||
            ((unit == OUTPUT_CURRENT) && (out_value > 20.0f || out_value < 4.0f))) {
            CLOG("Invalid range for Analog ouptut!!!\n");
            return;
        }

        if (unit == OUTPUT_VOLTAGE) {
            out_value = map(out_value, IN_MIN_RANGE_V, IN_MAX_RANGE_V, OUT_MIN_RANGE, OUT_MAX_RANGE);
        } else {
            out_value = map(out_value, IN_MIN_RANGE_I, IN_MAX_RANGE_I, OUT_MIN_RANGE_I, OUT_MAX_RANGE_I);
        }

        CLOG("DAC output value: %0.3fV\n", out_value);

        // Set the output out_value to the DAC channel
        if (mcp48f_dac_set_output(spi_dac, channel, out_value) > -1) {
            CLOG("Successfully Updated the DAC Output\n");
        } else {
            WLOG("Fail to update the DAC Output!!!\n");
        }
    }
    mcp48f_dac_deinit(spi_dac);
}