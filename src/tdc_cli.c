#include "ad7988.h"
#include "headers.h"

extern bool loop;


/**
 * @brief TDC Cli helper function
 * 
 */
void tdc_help(void) {
    CLOG("   TDC command Usage:\n");
    loopfeature(config.cli_ain_scan_period);
    CLOG("       Use -l to Loop continously every %d seconds\n", config.cli_ain_scan_period);
    CLOG("       %s -c TDC [VRef] [Polarity] [RawDisp] [SampleCnt]\n", exec_filename);
    CLOG("          VRef - Refference Voltage [0-5V]\n");
    CLOG("          Polarity:\n");
    CLOG("                0 - Positive Polarity\n");
    CLOG("                1 - Negative Polarity\n");
    CLOG("          RawDisp - Raw Samples Display:\n");
    CLOG("                0 - Do not show raw Samples\n");
    CLOG("                1 - Show raw Samples\n");
    CLOG("          SampleCnt - Provide number of samples to be read [Max - 10000]:\n");
}

/**
 * @brief                   TDC CLI command
 * 
 * @param argc              (ARGument Count) is int and stores number of command-line arguments passed by the user including the name of the program. 
 *                          So if we pass a value to a program, value of argc would be 2 (one for argument and one for program name) 
 *                          The value of argc should be non negative.
 * @param argv              (ARGument Vector) is array of character pointers listing all the arguments.
                            If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
                            Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.
 * @param additional_arg    Any additional arguments required
 */
void tdc_cmd(int argc, char **argv, void *additional_arg) {

    spi_t *spi_dac = NULL;
    uint8_t polarity = 0;
    struct timeval tv;
    if (argc < TDC_MIN_ARG_REQ) {
        tdc_help();
        return;
    }
    st_ain_sample.IEPE_Vref = atof(argv[1]);
    polarity = atoi(argv[2]);
    st_ain_sample.show_raw_samples = atoi(argv[3]);
    st_ain_sample.samp_cnt = atoi(argv[4]);
    st_ain_sample.IO_channel = 0;
    st_ain_sample.sensor_type = AI_TDC_ANALOG;
    DLOG("Vref= %f, pola = %d, raw cnt = %d, samp cnt = %d \r\n",st_ain_sample.IEPE_Vref, polarity, st_ain_sample.show_raw_samples, st_ain_sample.samp_cnt);

    if ((st_ain_sample.IEPE_Vref > DUAL_VIB_MAX_DAC_OUT) || (st_ain_sample.IEPE_Vref < 0)) {
        CLOG("Invalid VRef value\n");
        tdc_help();
        return;
    }
    else{
        config.ain_iepe_vref = st_ain_sample.IEPE_Vref / IEPE_DAC_OUT_AMP_GAIN;
        if (config.ain_iepe_vref > DUAL_VIB_MAX_DAC_OUT) {
            CLOG("Invalid IEPE Vref input. Max allowed voltage is %0.3lf\n", DUAL_VIB_MAX_DAC_OUT);
            return;
        } 
        else {
            if ((spi_dac = mcp48f_dac_init(config.dac1_dev, SPI_MODE_3, DAC_SPI_FREQ)) == NULL) {
                WLOG("DAC1 Init failed!!! ");
            }
            if (mcp48f_dac_set_output(spi_dac, DAC_DUAL_VIB_CH, config.ain_iepe_vref) > -1) {
                CLOG("IEPE Vref update success to %0.3f\n", config.ain_iepe_vref * IEPE_DAC_OUT_AMP_GAIN);
            } else {
                WLOG("IEPE Vref update Fail\n");
            }
            mcp48f_dac_deinit(spi_dac); //frees spaces and peripheral after reading
        }
    }
    if (/*polarity < 0 ||*/ polarity > 1) {
        tdc_help();
        CLOG("Invalid Polarity option selected!\n");
    }
    else{
        config.cli_ain_tdc_polarity = polarity;
    }

    if ((st_ain_sample.show_raw_samples > 1)/* || (st_ain_sample.show_raw_samples < 0) */) {
        CLOG("Invalid option selected for raw samples display!\n");
        tdc_help();
        return;
    }

    //Check the limit here it should not be too high or too less
    if (st_ain_sample.samp_cnt > AIN_MAX_ADC_SAMPLES_CNT) {
        CLOG("Invalid sample counts, current max limit is %d\n", AIN_MAX_ADC_SAMPLES_CNT);
        tdc_help();
        return;
    }
    
    DLOG("Scanning Started.....\n");

    if (loop == true) {
        tv.tv_sec = config.cli_ain_scan_period;
        tv.tv_usec = 0;
        check_stdin_cb(&tv, &ain_periodic_callback, &st_ain_sample);
    }
    else {
        ain_periodic_callback(NULL, &st_ain_sample);
    }
    // tv.tv_sec = config.cli_ain_scan_period;
    // tv.tv_usec = 0;
    // check_stdin_cb(&tv, &ain_periodic_callback, &st_ain_sample);
}