#include "headers.h"
#include "spi.h"
#include <linux/ioctl.h>
#include <linux/spi/spidev.h>
#include <stdbool.h>
#include <sys/ioctl.h>

extern bool loop;
// struct timeval tv;

int Sample_count = 1;
st_rtd_sample_t st_rtd_read;

/**
 * @brief RTD helper function
 * 
 */
void rtd_help(void) {
    CLOG("   RTD command Usage:\n");
    loopfeature(config.cli_rtd_op_prd);
    CLOG("       Use -l to Loop continously every %d seconds\n", config.cli_rtd_op_prd);
    CLOG("       %s -c RTD [CH] [SAMP]\n", exec_filename);
    CLOG("          CH - Channel selection\n");
    CLOG("                Number of channels available:\n");
    // CLOG("                0 - Dont care\n");
    CLOG("                1 - Channel 1\n");
    CLOG("                2 - Channel 2\n");
    CLOG("                3 - Channel 3\n");
    CLOG("                4 - Channel 4\n");
    CLOG("          SAMP - Number of samples to be read and averaged\n");
    CLOG("                 0 - don't care\n");
    CLOG("                 Min 1 - Max %d count\n", RTD_MAX_SAMP_CNT);
}

/**
 * @brief               Function to loop continously untill keyboard interrupt provided
 * 
 * @param ptr           Pass NULL to stay in loop or else it will exit
 * @param args          arguments to be called back
 * @return int8_t       return 0 on success, non 0 on failure
 */
int rtd_periodic_callback(char *ptr, void *args) {

    if (ptr != NULL) {
        ILOG("exiting from rtd sensor periodic read %s\n", ptr);
        return 1;
    }

    uint8_t index = 0;
    // int test = 0;
    // char print_str[80] = {0};
    spi_t *spi = max31865_init(config.rtd_spi_device, SPI_MODE_1, MAX31865_SPI_CLOCK_FREQ);

    st_rtd_sample_t *st_rtd_read_loc = (st_rtd_sample_t *)args;
    int channel = 0;

    // DLOG("Number of samples read: %d\n", Sample_count++);

    for (channel = 1; channel <= 4; channel++) {
        if(st_rtd_read_loc->IO_channel == 0 || st_rtd_read_loc->IO_channel == channel){
             CLOG("Reading channel: %d, S count: %d\n", channel, st_rtd_read_loc->rtd_sample_cnt);

            digital_pin_out_write(config.rtd_mux_en_pin, RESET); // Disable MUX
            if (channel == 1) {                                  //RTD channels selectio
                digital_pin_out_write(config.rtd_ch_sel_a0_pin, RESET);
                digital_pin_out_write(config.rtd_ch_sel_a1_pin, RESET);
            } else if (channel == 2) {
                digital_pin_out_write(config.rtd_ch_sel_a0_pin, SET);
                digital_pin_out_write(config.rtd_ch_sel_a1_pin, RESET);
            } else if (channel == 3) {
                digital_pin_out_write(config.rtd_ch_sel_a0_pin, RESET);
                digital_pin_out_write(config.rtd_ch_sel_a1_pin, SET);
            } else if (channel == 4) {
                digital_pin_out_write(config.rtd_ch_sel_a0_pin, SET);
                digital_pin_out_write(config.rtd_ch_sel_a1_pin, SET);
            }
            //usleep(50000);
            digital_pin_out_write(config.rtd_mux_en_pin, SET); //Enable MUX

            //reseting all rtd parameters
            st_rtd_read_loc->counter = 0;
            st_rtd_read_loc->rtd_max_temp[channel - 1] = -1000.0f;
            st_rtd_read_loc->rtd_min_temp[channel - 1] = 1000.0f;
            st_rtd_read_loc->rtd_avg_temp[channel - 1] = -1.0f;
            st_rtd_read_loc->rtd_fault_status[channel - 1] = 0;

            if(st_rtd_read_loc->rtd_sample_cnt < 1){
                st_rtd_read_loc->rtd_sample_cnt = 5; ///Read 5 samples if it is not mentioned
            } 

            // ILOG("Fault Reg before : %d, ", st_rtd_read_loc->rtd_fault_status[channel - 1]);

            CLOG("RTD Result %d:- ", channel);

            rtd_write_byte(spi, WRITE_CONFIG_ADDR, 0x00);

            if (!(rtd_sensor_scan(spi, st_rtd_read_loc, channel))) {
                CLOG("Res: %0.2f ohms, Avg: %d, Min: %d, Max: %d, Delta: %d AvgTemp: %0.2f DegC, MinTemp: %0.2f DegC, MaxTemp: %0.2f DegC, DeltaTemp: %0.2f\n",
                    RTD_COUNT_TO_RESISTANCE(st_rtd_read_loc->avg_adc_count[channel - 1]), st_rtd_read_loc->avg_adc_count[channel - 1],
                    st_rtd_read_loc->min_adc_count[channel - 1], st_rtd_read_loc->max_adc_count[channel - 1], 
                    (st_rtd_read_loc->max_adc_count[channel - 1] - st_rtd_read_loc->min_adc_count[channel - 1]),
                    st_rtd_read_loc->rtd_avg_temp[channel - 1], st_rtd_read_loc->rtd_min_temp[channel - 1], st_rtd_read_loc->rtd_max_temp[channel - 1],
                    (st_rtd_read_loc->rtd_max_temp[channel - 1] - st_rtd_read_loc->rtd_min_temp[channel - 1]));
            } else {
                st_rtd_read_loc->rtd_fault_status[channel - 1] = rtd_read_fault_status_reg(spi);
                ILOG("Error Occured While Reading RTD Sensor\r\n");
                CLOG("Fault Reg : %d ", st_rtd_read_loc->rtd_fault_status[channel - 1]);
                CLOG("=> b\'");
                for (index = 0; index < 8; index++) {
                    WLOG("%d", ((st_rtd_read_loc->rtd_fault_status[channel - 1] >> (7 - index)) & 0x01));
                }

                if (st_rtd_read_loc->rtd_fault_status[channel - 1] & 0x80) {
                    WLOG("Bit D7 is Set\r\n");
                    WLOG("Condition Detected :- Measured Resistance Greater than High Fault Threshold Value\n");
                    WLOG("Possible Causes: \n\t1. Open RTD Element \n\t2.RTD+ shorted high and not connected to RTD \n\t3.FORCE+ shorted high and connected to RTD\n");
                }
                if (st_rtd_read_loc->rtd_fault_status[channel - 1] & 0x40) {
                    WLOG("Bit D6 is Set\r\n");
                    WLOG("Condition Detected :- Measured Resistance less than low fault threshold value\r\n");
                    WLOG("Possible Causes:- \n\t1. RTD+ shorted to RTD- \n\t2. RTD+ shorted low and not connected to RTD \n\t3. FORCE+ shorted low\r\n");
                }
                if (st_rtd_read_loc->rtd_fault_status[channel - 1] & 0x20) {
                    WLOG("Bit D5 is Set\r\n");
                    WLOG("Condition Detected :- V(REFIN-) > 0.85*V(BIAS)\r\n");
                    WLOG("Possible Causes:- \n\t1. Open RTD element \n\t2. FORCE+ shorted high and connected to RTD \n\t3. FORCE+ unconnected \n\t4. FORCE+ shorted high and not connected to RTD \n\t5. RTDIN- shorted high\r\n");
                }
                if (st_rtd_read_loc->rtd_fault_status[channel - 1] & 0x10) {
                    WLOG("Bit D4 is Set\r\n");
                    WLOG("Condition Detected :- V(REFIN-) < 0.85*V(BIAS). FORCE- open\r\n");
                    WLOG("Possible Causes:- \n\t1. RTD- shorted low\r\n");
                }
                if (st_rtd_read_loc->rtd_fault_status[channel - 1] & 0x08) {
                    WLOG("Bit D3 is Set\r\n");
                    WLOG("Condition Detected :- V(RTDIN-) < 0.85*V(BIAS). FORCE- open\r\n");
                    WLOG("Possible Causes:- \n\t1. FORCE+ shorted low \n\t2. RTDIN+ shorted low and connected to RTD \n\t3. RTDIN- shorted low\r\n");
                }
                if (st_rtd_read_loc->rtd_fault_status[channel - 1] & 0x04) {
                    WLOG("Bit D2 is Set\r\n");
                    WLOG("Condition Detected :- OverVoltage or UnderVoltage Fault\r\n");
                    WLOG("Possible Causes:- \n\t\t1. Any protected input voltage > VDD or <GND1\r\n");
                }
            }
        }
    } 
    ILOG(" ");
    max31865_deinit(spi);
    return 0;
}

/**
 * @brief                   RTD CLI command
 * 
 * @param argc              (ARGument Count) is int and stores number of command-line arguments passed by the user including the name of the program. 
 *                          So if we pass a value to a program, value of argc would be 2 (one for argument and one for program name) 
 *                          The value of argc should be non negative.
 * @param argv              (ARGument Vector) is array of character pointers listing all the arguments.
                            If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
                            Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.
 * @param additional_arg    Any additional arguments required
 */
void rtd_cmd(int argc, char **argv, void *additional_arg) {
    struct timeval tv;

    // validate number of arguments
    if (argc < RTD_MIN_ARG_REQ) {
        rtd_help();
        return;
    }
    //parsing and store the inputs
    st_rtd_read.IO_channel = atoi(argv[1]);
    st_rtd_read.rtd_sample_cnt = atoi(argv[2]);

    // Validate Channel Number
    DLOG("Selected channel: %d\r\n", st_rtd_read.IO_channel);

    if ((st_rtd_read.IO_channel > 4 || st_rtd_read.IO_channel < 1)){
        CLOG("Invalid channel!!!\n");
        rtd_help();
        return;
    }
    // Validate Sample Count
    if (st_rtd_read.rtd_sample_cnt > RTD_MAX_SAMP_CNT) {
        CLOG("Invalid sample counts, current max limit is %d\n", RTD_MAX_SAMP_CNT);
        st_rtd_read.rtd_sample_cnt = RTD_MAX_SAMP_CNT;
    }
   
    if (loop == true) {
        tv.tv_sec = config.cli_rtd_op_prd;
        tv.tv_usec = 0;
        check_stdin_cb(&tv, &rtd_periodic_callback, &st_rtd_read);
    }
    else {
        rtd_periodic_callback(NULL, &st_rtd_read);
    }

   rtd_periodic_callback(NULL, &st_rtd_read);
    // DLOG("Scanning Started.....\n");

    // //populating period
    // tv.tv_sec = config.cli_rtd_op_prd;
    // tv.tv_usec = 0;
    // check_stdin_cb(&tv, &rtd_periodic_callback, &st_rtd_read);


    char *outs;
	cJSON *root;
    

	/* create root node and array */
	root = cJSON_CreateObject();
    
	if(st_rtd_read.IO_channel == 1 ||  st_rtd_read.IO_channel == 0){ //as of now we are sending data of first channel only
        cJSON_AddItemToObject(root, "peripheral", cJSON_CreateString("RTD"));
        cJSON_AddItemToObject(root, "response", cJSON_CreateString("ACK"));
        cJSON_AddItemToObject(root, "rtd_channel_sel", cJSON_CreateNumber(st_rtd_read.channel_scan));
        cJSON_AddItemToObject(root, "rtd_min_adc", cJSON_CreateNumber(st_rtd_read.min_adc_count[0]));
        cJSON_AddItemToObject(root, "rtd_max_adc", cJSON_CreateNumber(st_rtd_read.max_adc_count[0]));
        cJSON_AddItemToObject(root, "rtd_avg_adc", cJSON_CreateNumber(st_rtd_read.avg_adc_count[0]));
        cJSON_AddItemToObject(root, "rtd_delta_adc", cJSON_CreateNumber(st_rtd_read.max_adc_count[0] - st_rtd_read.min_adc_count[0]));
        cJSON_AddItemToObject(root, "rtd_avg_temp", cJSON_CreateNumber(st_rtd_read.rtd_avg_temp[0]));
        char ts[30];
        getCurrentTimeMsStr(ts);
        cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(ts));
        
        /* print everything */
        outs = cJSON_PrintUnformatted(root);
        CLOG("Results: %s\n",outs);
        free(outs);

        /* free all objects under root and root itself */
        cJSON_Delete(root);
    }
}