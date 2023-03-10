#include "headers.h"

/**
 * @brief        Dac helper function
 *
 */
void dac_help(void)
{
    CLOG("DAC command Usage:\n");
    CLOG("   %s -c DAC [DAC_CH] [VAL]\n", exec_filename);
    CLOG("      DAC_CH - DAC output channel selection:\n");
    CLOG("           1 - Ouput channel 1\n");
    CLOG("           2 - Ouput channel 2\n");
    CLOG("      VAL - Actual Circuit Output voltage\n");
}

/**
 * @brief                   din CLI command
 *
 * @param argc              (ARGument Count) is int and stores number of command-line arguments passed by the user including the name of the program.
 *                          So if we pass a value to a program, value of argc would be 2 (one for argument and one for program name)
 *                          The value of argc should be non negative.
 * @param argv              (ARGument Vector) is array of character pointers listing all the arguments.
                            If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
                            Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.
 * @param additional_arg    Any additional arguments required
 */
void dac_cmd(int argc, char **argv, void *additional_arg)
{
    if (argc < DAC_MIN_ARG_REQ)
    {
        dac_help();
        return;
    }

    // parsing the inputs
    int channel = atoi(argv[1]);
    float out_value = atof(argv[2]);

    CLOG("DAC: channel num: %d, DAC output voltage: %0.3f\n", channel, out_value);

    if (channel > 2 || channel < 1)
    {
        CLOG("Invalid channel!!!\n");
        dac_help();
        return;
    }

    // if (channel == 2){
    //     CLOG("Invalid channel!!!\n");
    //     dac_help();
    //     return;
    // }
    
    if (out_value > 5.0f || out_value < 0.0f)
    {
        CLOG("Invalid voltage output range!!!\n");
        dac_help();
        return;
    }

    // //Enable MUX and output power
    // digital_pin_out_write(config.analog_path_ch_sel_mux_en_pin, SET);
    spi_t *spi_dac = NULL;

    // Some delay if needed
    usleep(100); // delay in microsecond

    // Initialize the DAC
    if ((spi_dac = mcp48f_dac_init(config.mcp48f_dac_dev, SPI_MODE_3, DAC_SPI_FREQ)) == NULL)
    {
        WLOG("DAC Init failed!!! ");
    }

    CLOG("DAC Output voltage: %0.3f\n", out_value);
    // Set the output out_value to the DAC channel
    if (mcp48f_dac_set_output(spi_dac, channel, out_value) > -1)
    {
        CLOG("Successfully Updated the DAC Output\n");
    }
    else
    {
        WLOG("Fail to update the DAC Output!!!\n");
    }

    mcp48f_dac_deinit(spi_dac); // frees spaces and peripheral after reading
}