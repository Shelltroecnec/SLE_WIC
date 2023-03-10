/**
 * @file analog_cli.c
 * @author Fakhruddin Khan (f.khan@acevin.com)
 * @brief  This file contains the CLI functionality for Analog input sensors
 * @version 1.1
 * @date 2021-08-11
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#include "ad7988.h"
#include "headers.h"

extern int Sample_count;

extern bool loop;
// struct timeval tv;
st_ain_sample_t st_ain_sample;

char sensor_string[][20] = {
    "4-20mA_PATH",
    "0-10V Path",
    "IEPE_PATH",
    "MSUPLY_VMON",
    "LOAD_CELL",
    // "TDC_ANALOG",
    "MOTOR2_CS_B",
    "MOTOR1_CS_B"};

/**
 * @brief       Analog in Helper function
 * 
 */
void ain_help(void) {
    CLOG("AI command Usage:\n");
    loopfeature(config.cli_ain_scan_period);
    CLOG("    %s -c AI [CH] [TYPE] [PowerSel] [Samples] [SamplingFreq] [RawDisplay] [SettlingTime]\n", exec_filename);
    CLOG("       CH - Channel selection, This is applicable for first 5(0-4) sensor types and don't care for others\n");
    CLOG("            Number of channels available:\n");
    CLOG("            0 - N/A\n");
    CLOG("            1 - Channel 1\n");
    CLOG("            2 - Channel 2\n");
    CLOG("            3 - Channel 3\n");
    CLOG("            4 - Channel 4\n");
    CLOG("            5 - Channel 5\n");
    CLOG("            6 - Channel 6\n");
    CLOG("            7 - Channel 7\n");
    CLOG("            8 - Channel 8\n");
    CLOG("       TYPE - type of the sensor connected to ADC MUX\n");
    CLOG("            Available sensor types:\n");
    CLOG("            0 - 4-20mAmp sensor\n");
    CLOG("            1 - 0-10V sensor \n");
    CLOG("            2 - IEPE Senor\n");
    CLOG("            3 - MSUPLY_VMON\n");
    // CLOG("            4 - TDC Analog\n");
    CLOG("            5 - LOAD_CELL\n");
    CLOG("            6 - MOTOR2 Current Sensor\n");
    CLOG("            7 - MOTOR1 Current Sensor\n");
    CLOG("       PowerSel - Select power supply\n");
    CLOG("            0 - No power supply\n");
    CLOG("            1 - 24V-40mA power supply\n");
    CLOG("            2 - 12V power supply\n");
    CLOG("       Samples - Provide number of samples to be read [Max - 10000]:\n");
    CLOG("       SamplingFreq - Sampling Frequency of ADC [0 for default Fs %d]:\n",config.cli_ain_sample_freq);
    CLOG("       RawDisplay:\n");
    CLOG("            0 - Do not show raw samples\n");
    CLOG("            1 - Show raw samples\n");
    CLOG("       SettlingTime - Provide settling time delay for the sensor\n");
    // CLOG("       BiasSel:\n");
    // CLOG("            1 - AC BIAS\n");
    // CLOG("            2 - DC BIAS\n");
    // CLOG("       Vref - Reference Voltage\n");
}


/**
 * @brief This function will configure the select the HW Mux,lines output etc based on the user selection
 *
 * Steps involved
 *        TURN OFF ALL CONNECTIONS - its default state - mostly Mux disable should work
 *        (added new feature to not disable Power supply MUX it is same type that was selected last time to avoid power cycling/fluctuation of sensor)
 *        SETUP CHOSEN SENSOR TYPE - select channels first 
 *        TURN ON ALL CONNECTIONS
 *        START MEASUREMENT AFTER SETTLING TIME OF SENSOR PER SENSOR SPECS
 *
 *    Parameters needed for selecting right channels and sensors
 *
 *    IOS_channel: select the 4 channels S1/S2/S3/S4 analog input as well as the output power supply
 *        S0 S1 pins --- channel select pin common for both the Mux analog input and supply output
 *        input_mux_enable_pin
 *        supply_mux_enable_pin
 *        supply_mux_PWR_enable_pin
 *
 *    Power_select:
 *        supply_mux_PWR_enable_pin should be enable at the end when selection is completed.
 *
 *        Pins           Value      PWR_Select            420_PWR_CTRL
 *        --------------------------------------------------------------
 *        None             0             0                       0  (It won enable the power enable Mux)
 *        24V-420 mAmps    1             0                       1
 *        12V supply       2             1                       X
 *
 *    Sensor Connected to ADC:
 *          Value    SENSOR TYPE PROCESSING       A2      A1      A0  
 *          --------------------------------------------------------- 
 *            0        420mAmp 		            0   	0   	0	 
 *            1        0-10V path        		    0   	0   	1	 
 *            2        IEPE path       		    0   	1   	0	 
 *            3        MSUPLY_VMON		        0   	1   	1	 
 *            4        TDC_ANALOG		            1   	0   	0	 
 *            5        LOAD_CELL		            1   	0   	1	 
 *            6        MOTOR2_CS_B		        1   	1   	0	 
 *            7        MOTOR1_CS_B		        1   	1   	1	 
 *
 *    When sensor type is selected as IEPE, Then below two more informations are needed:
 *    IEPE Vref setting via AO 
 *    IEPE_CTL should be 1 for IEPE type sensor to provide 3.5mAmp current to IEPE sensor
 *         Type               IEPE_GAIN_CNTL 
 *        -----------------------------------  
 *        None                   0            
 *        AC BIAS                1            
 *        DC BIAS                2            
 *
 * @param pst_ai_sensor Structure to the user selection.
 */
void analog_sensor_config(st_ain_sample_t *pst_ai_sensor) {

    // void *pAddr;
    // int ret = -1;

    CLOG("read pselec: %d, cur pselect: %d\n", pst_ai_sensor->last_power_selected, pst_ai_sensor->ai_pwr_supply_src_sel);
    //Sensor selections to ADC input
    CLOG("[AI] Selected, IO Channel: %d, Sensor: %s, Power select: %d, pst_ai_sensor->last_power_selected: %d,\n", pst_ai_sensor->IO_channel, sensor_string[pst_ai_sensor->sensor_type], pst_ai_sensor->ai_pwr_supply_src_sel, pst_ai_sensor->last_power_selected);
    
    // switching off the mux to default state only if no supply selected or it is different from the previous one
    digital_pin_out_write(config.analog_pwr_sel_pin, RESET);
    
    if(pst_ai_sensor->last_power_selected != pst_ai_sensor->ai_pwr_supply_src_sel || pst_ai_sensor->ai_pwr_supply_src_sel == OUTPUT_POWER_SUPPLY_SELECT_NONE){
        digital_pin_out_write(config.analog_pwr_sel_mux_en_pin, RESET);
        digital_pin_out_write(config.analog_12v_pwr_en_pin, RESET); //should be 0 in case of Analog

        //update the last power select option to shared memory
	    stMy_data->st_shm_data.last_power_selected = pst_ai_sensor->ai_pwr_supply_src_sel;
        CLOG("updating pselec: %d, \n", pst_ai_sensor->ai_pwr_supply_src_sel);

    }
    
    //Analog path selection 
    //For line A0
    digital_pin_out_write(config.analog_path_sel_a0_pin, ((pst_ai_sensor->sensor_type & 0x01)?  SET :RESET));      
    //For line A1
    digital_pin_out_write(config.analog_path_sel_a1_pin, (((pst_ai_sensor->sensor_type >> 1) & 0x01)?  SET :RESET));      
    //for line A2
    digital_pin_out_write(config.analog_path_sel_a2_pin, (((pst_ai_sensor->sensor_type >> 2) & 0x01)?  SET :RESET));      
    

    //Ouput power supply selection
    if(pst_ai_sensor->ai_pwr_supply_src_sel == OUTPUT_POWER_SUPPLY_SELECT_420MA_24V){
        // digital_pin_out_write(config.analog_pwr_sel_s0_pin, RESET);
        digital_pin_out_write(config.ops_420m_pwr_ctrl_pin, SET);  
        digital_pin_out_write(config.analog_pwr_sel_mux_en_pin, SET);
        digital_pin_out_write(config.analog_pwr_sel_pin, RESET);
    } else if(pst_ai_sensor->ai_pwr_supply_src_sel == OUTPUT_POWER_SUPPLY_SELECT_12V){
        // digital_pin_out_write(config.analog_pwr_sel_s0_pin, RESET); //Dont care here
        digital_pin_out_write(config.analog_pwr_sel_mux_en_pin, SET);
        digital_pin_out_write(config.analog_pwr_sel_pin, SET); 
    } else {
        //none
        digital_pin_out_write(config.analog_pwr_sel_mux_en_pin, RESET);
        // digital_pin_out_write(config.analog_pwr_sel_s0_pin, SET); //Dont care here
        // digital_pin_out_write(config.analog_pwr_sel_pin, SET); //Dont care here
    }

    if(pst_ai_sensor->IO_channel < 8){
        digital_pin_out_write(config.analog_ch_sel_s0_pin, ((pst_ai_sensor->IO_channel & 0x01)?  SET :RESET));
        digital_pin_out_write(config.analog_ch_sel_s1_pin, (((pst_ai_sensor->IO_channel >> 1) & 0x01)?  SET :RESET));
        digital_pin_out_write(config.analog_ch_sel_s2_pin, (((pst_ai_sensor->IO_channel >> 2) & 0x01)?  SET :RESET));
    }
    
    // if(pst_ai_sensor->sensor_type == AI_IEPE_PATH){
    //     digital_pin_out_write(config.IEPE_ctl_pin, SET);
    //     if(pst_ai_sensor->IEPE_bias_type == AI_IEPE_AC_BIASE){ //AC Biase
    //         digital_pin_out_write(config.IEPE_gain_ctl_pin, RESET);
    //     } else if(pst_ai_sensor->IEPE_bias_type == AI_IEPE_DC_BIASE){ 
    //         digital_pin_out_write(config.IEPE_gain_ctl_pin, SET);
    //     } else {
    //         DLOG("[AI] IEPE Bias is not configure, using AC BIAS only\r\n");
    //         digital_pin_out_write(config.IEPE_gain_ctl_pin, RESET);
    //     }
    // } else {
    //     digital_pin_out_write(config.IEPE_gain_ctl_pin, RESET);
    //     digital_pin_out_write(config.IEPE_ctl_pin, RESET);
    // }

    DLOG("Enabling Mux enable pins.....\n");
    
    // Enable MUX and output power
    digital_pin_out_write(config.analog_pwr_sel_pin, SET);

    //Check if the output power supply is selected if not selected then MUX_PWR_EN
    if(pst_ai_sensor->ai_pwr_supply_src_sel != OUTPUT_POWER_SUPPLY_SELECT_NONE){
        digital_pin_out_write(config.analog_pwr_sel_mux_en_pin, SET);
    }

    pst_ai_sensor->last_power_selected = pst_ai_sensor->ai_pwr_supply_src_sel; //Saving this information so that it will not reset the power supply while operating on the same type of output
}


/**
 * @brief       This Function is to loop continuously until keyboard interrupt provided 
 *              This will sample the 
 * 
 * @param ptr   Pass NULL to stay in loop or else it will exit
 * @param args  arguments to be called back
 * @return      int8_t return 0 on success, non 0 on failure
 */
int  ain_periodic_callback(char *ptr, void *args) {
    // uint16_t max_samp = 0, min_samp = 0, avg_cnt = 0;
    
    st_ain_sample_t *pst_ain_samp = (st_ain_sample_t *)args;
    uint16_t read_adc_samples[AIN_MAX_ADC_SAMPLES_CNT + 1]; //Max 1K sample here

    CLOG("ch: %d\n", pst_ain_samp->IO_channel);
    CLOG("type: %d\n",  pst_ain_samp->sensor_type);
    // CLOG("bias type: %d\n", pst_ain_samp->IEPE_bias_type);
    CLOG("pwr_select: %d\n",pst_ain_samp->ai_pwr_supply_src_sel);
    // CLOG("Bias voltage: %f\n",  pst_ain_samp->IEPE_Vref);
    CLOG("samp_cnt: %d\n",pst_ain_samp->samp_cnt);
    CLOG("scan_period: %d\n",pst_ain_samp->scan_period);
    CLOG("sampling_rate: %d\n",pst_ain_samp->sampling_rate);
    CLOG("show_raw_samples: %d\n",pst_ain_samp->show_raw_samples);

    //Sample the data here
    pst_ain_samp->settling_time_ms = 0;//use default settling time from config
    read_analog_in(pst_ain_samp, read_adc_samples);

    //Neglect first and last samples
    pst_ain_samp->avg_cnt = avg_samples_integer((uint16_t*)&read_adc_samples[1], pst_ain_samp->samp_cnt - 2, &pst_ain_samp->max_samp, &pst_ain_samp->min_samp);
    DLOG("Number of samples read: %d\n", Sample_count++);
    if (pst_ain_samp->show_raw_samples == 1) {
        CLOG("Raw samples: ");
        for (uint32_t samp_index = 0; samp_index < pst_ain_samp->samp_cnt; samp_index++) {
            printf("%d ", read_adc_samples[samp_index + 1]);
            // if (((samp_index + 1) % 10) == 0)
            //     CLOG("\r");
        }
        printf("\r\n");
    }
    CLOG("Min = %d, Max = %d, Avg = %d, delta = %d\n", pst_ain_samp->min_samp, pst_ain_samp->max_samp, pst_ain_samp->avg_cnt, pst_ain_samp->max_samp - pst_ain_samp->min_samp);
    
    if (ptr != NULL) {
        DLOG("exiting from analog input periodic read %s\n", ptr);
        return 1;
    }
    return 0;

}


/**
 * @brief                   Reads analog sensor inputs
 * 
 * @param pst_ain_sample    structure of analog parameters
 * @param buff              buffer to read samples
 * @return int8_t           return 0 on success, non 0 on failure
 */
int read_analog_in(st_ain_sample_t *pst_ain_sample,  uint16_t *buff){//uint8_t sensor_type, uint8_t channel, uint16_t samp_cnt, uint32_t sampling_freq,) {
    
    shared_mem_mutex_lock(&stMy_data->stAnalog_lock);

    //Read the last supply value and update to the structure here
	pst_ain_sample->last_power_selected = stMy_data->st_shm_data.last_power_selected;
    
    //Select analog sensors processing path
    analog_sensor_config(pst_ain_sample);

    DLOG("[%ld] delay starting..\n", GetCurrentTimeUs());
    usleep(config.sensor_settling_time_ms * 1000); //delay in microsecond
    DLOG("[%ld] delay expired\n", GetCurrentTimeUs());
    if(pst_ain_sample->settling_time_ms > 0){
        config.sensor_settling_time_ms = pst_ain_sample->settling_time_ms;
    }
    DLOG("settling time provided: %d\n", config.sensor_settling_time_ms);

    spi_t *spi_ad7988 = NULL;
    
    //Read ADC
    spi_ad7988 = ad7988_init(config.ad7988_dev, SPI_MODE_0, SPI_FREQ_2M);

    //Printf("SPI location: %x\n", spi_ad7988);

    AD7988_ReadData(spi_ad7988, buff, pst_ain_sample->samp_cnt);

    //float read_volt = ad7988_voltage_conv(readADC, samp_cnt);

    //CLOG("Read adc value: %d, volts: %0.3f\n", readADC, read_volt);

    //free spaces and peripheral after reading
    ad7988_deinit(spi_ad7988);

    shared_mem_mutex_unlock(&stMy_data->stAnalog_lock);
    return 0;
}


/**
 * @brief                   Analog in CLI command
 * 
 * @param argc              (ARGument Count) is int and stores number of command-line arguments passed by the user including the name of the program. 
 *                          So if we pass a value to a program, value of argc would be 2 (one for argument and one for program name) 
 *                          The value of argc should be non negative.
 * @param argv              (ARGument Vector) is array of character pointers listing all the arguments.
                            If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
                            Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.
 * @param additional_arg    Any additional arguments required
 */
void ain_cmd(int argc, char **argv, void *additional_arg) {
    struct timeval tv;
    spi_t *spi_dac = NULL;
    // double iepe_vref = 0;
    int samplingfreq = 0;
    char *outs;
	cJSON *root;

    if (argc < AIN_MIN_ARG_REQ) {
        // CLOG("[AI] Less number of argument/s provided for AI Cmd!!\n");
        ain_help();
        return;
    }
    // if ((st_ain_sample.sensor_type == AI_IEPE_PATH) && (argc < AIN_MAX_ARG_REQ)) {
    //     ain_help();
    //     return;
    // }

    // for(int i = 0; i < argc; i++){
    //     DLOG("argv[%d]: %s\r\n", i, argv[i]);
    // }

    //parsing the inputs
    st_ain_sample.IO_channel = atoi(argv[1]);
    st_ain_sample.sensor_type = atoi(argv[2]);
    st_ain_sample.ai_pwr_supply_src_sel = atoi(argv[3]);
    st_ain_sample.samp_cnt = atoi(argv[4]);
    samplingfreq = atoi(argv[5]);
    st_ain_sample.show_raw_samples = atoi(argv[6]);
   
    config.sensor_settling_time_ms = atoi(argv[7]);
    // if (st_ain_sample.sensor_type == AI_IEPE_PATH) {
    //     st_ain_sample.IEPE_bias_type = atoi(argv[8]);
    //     st_ain_sample.IEPE_Vref = atof(argv[9]);
    // }
    // //CLOG("Selected Sensor type[%d]: %s, channel num: %d\n", st_ain_sample.sensor_type, sensor_string[st_ain_sample.sensor_type], st_ain_sample.IO_channel);
    // DLOG("Sample Cnt: %d, Raw Sample Display: %d\n", st_ain_sample.samp_cnt, st_ain_sample.show_raw_samples);

    if ((st_ain_sample.sensor_type > MAX_ANALOG_SENSORS_SUPP_CLI) /*|| (st_ain_sample.sensor_type < 0)*/) {
        CLOG("Invalid sensor_type!!!\n");
        ain_help();
        return;
    }
    if ((st_ain_sample.IO_channel > 8 || st_ain_sample.IO_channel < 1)) {
        CLOG("Invalid channel!!!\n");
        ain_help();
        return;
    }
    st_ain_sample.IO_channel--; // as in current version channels are defined from 0-3 to use it as 2 bits value.
    if ((st_ain_sample.ai_pwr_supply_src_sel > MAX_POWER_SUPP_CLI) /*|| (st_ain_sample.ai_pwr_supply_src_sel < MIN_POWER_SUPP_CLI)*/) {
        CLOG("Invalid power_supply_type!!!\n");
        ain_help();
        return;
    }
    // if ((st_ain_sample.IEPE_bias_type > MAX_ANALOG_SENSORS_SUPP_CLI) /* || (st_ain_sample.IEPE_bias_type < 0)*/) {
    //     CLOG("Invalid bias_type!!!\n");
    //     ain_help();
    //     return;
    // }
    // if (st_ain_sample.sensor_type == AI_IEPE_PATH) {
    //     config.ain_iepe_vref = st_ain_sample.IEPE_Vref / IEPE_DAC_OUT_AMP_GAIN;

    //     //Check the limit here it should not be too high or too less
    //     if (config.ain_iepe_vref > DUAL_VIB_MAX_DAC_OUT) {
    //         CLOG("Invalid IEPE Vref input. Max allowed voltage is %0.3lf\n", DUAL_VIB_MAX_DAC_OUT);
    //         return;
    //     } else {
    //         if ((spi_dac = mcp48f_dac_init(config.dac1_dev, SPI_MODE_3, DAC_SPI_FREQ)) == NULL) {
    //             WLOG("DAC1 Init failed!!! ");
    //         }
    //         if (mcp48f_dac_set_output(spi_dac, DAC_DUAL_VIB_CH, config.ain_iepe_vref) > -1) {
    //             CLOG("IEPE Vref update success to %0.3f\n", config.ain_iepe_vref * IEPE_DAC_OUT_AMP_GAIN);
    //         } else {
    //             WLOG("IEPE Vref update Fail\n");
    //         }
    //         mcp48f_dac_deinit(spi_dac); //frees spaces and peripheral after reading
    //     }
    
    // }
    if ((samplingfreq > AIN_MAX_SCAN_PERIOD) || (samplingfreq < AIN_MIN_SCAN_PERIOD)) {
        DLOG("Invalid scan period, using the default period\n");
    } else {
        config.cli_ain_sample_freq = samplingfreq;
    }
    //Check the limit here it should not be too high or too less
    if (st_ain_sample.samp_cnt > AIN_MAX_ADC_SAMPLES_CNT) {
        CLOG("Invalid sample counts, current max limit is %d\n", AIN_MAX_ADC_SAMPLES_CNT);
        return;
    }
    DLOG("Scanning Started.....\n");
    if ((st_ain_sample.show_raw_samples > 1) /*|| (st_ain_sample.show_raw_samples < 0)*/) {
        CLOG("Invalid option selected for raw samples display!!!\n");
        ain_help();
        return;
    }
    //channel selection will not used from MSUPPLY_VMON
    if (st_ain_sample.sensor_type >= AI_MSUPPLY_VMON) {
        st_ain_sample.IO_channel = 0;
    }

    if (loop == true) {
        tv.tv_sec = config.cli_ain_scan_period;
        tv.tv_usec = 0;
        check_stdin_cb(&tv, &ain_periodic_callback, &st_ain_sample);
    }
    else {
        ain_periodic_callback(NULL, &st_ain_sample);
    }

   	/* create root node and array */
	root = cJSON_CreateObject();

    //CLOG("[AI] output, Sensor type[%d], Scaling: %f\r\n", st_ain_sample.sensor_type, AI_SCALING_FACTOR(st_ain_sample.sensor_type, st_ain_sample.avg_cnt));
    
    /* add elements root object */
	cJSON_AddItemToObject(root, "peripheral", cJSON_CreateString("ANALOG_INPUT"));
	cJSON_AddItemToObject(root, "response", cJSON_CreateString("ACK"));
	cJSON_AddItemToObject(root, "ai_sensor_sel", cJSON_CreateNumber(st_ain_sample.sensor_type));
	cJSON_AddItemToObject(root, "sample_count", cJSON_CreateNumber(st_ain_sample.samp_cnt));
	cJSON_AddItemToObject(root, "ai_io_channel", cJSON_CreateNumber(st_ain_sample.IO_channel));
	cJSON_AddItemToObject(root, "min", cJSON_CreateNumber(st_ain_sample.min_samp));
	cJSON_AddItemToObject(root, "max", cJSON_CreateNumber(st_ain_sample.max_samp));
	cJSON_AddItemToObject(root, "avg", cJSON_CreateNumber(st_ain_sample.avg_cnt));
	cJSON_AddItemToObject(root, "delta", cJSON_CreateNumber(st_ain_sample.max_samp - st_ain_sample.min_samp));
	char ts[30];
	getCurrentTimeMsStr(ts);
	cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(ts));
	//TODO:: add raw data in the response frames
	
    // DLOG("min_con = %f, min =  %f\n", AI_SCALING_FACTOR(st_ain_sample.sensor_type, st_ain_sample.min_samp), st_ain_sample.min_samp);
    // DLOG("max_con = %f, max =  %f\n", AI_SCALING_FACTOR(st_ain_sample.sensor_type, st_ain_sample.max_samp), st_ain_sample.max_samp);
    // DLOG("avg_con = %f, avg =  %f\n", AI_SCALING_FACTOR(st_ain_sample.sensor_type, st_ain_sample.avg_cnt), st_ain_sample.avg_cnt);
	/* print everything */
	outs = cJSON_PrintUnformatted(root);
	CLOG("Results: %s\n", outs);
	free(outs);
    
	/* free all objects under root and root itself */
	cJSON_Delete(root);
}

/**
 * @brief               Set the tdc vref object
 * 
 * @param iepe_Vref     Value of Vref to be passed
 */
// void set_tdc_vref(float iepe_Vref){

//     spi_t *spi_dac = NULL;

//     config.ain_iepe_vref = iepe_Vref / IEPE_DAC_OUT_AMP_GAIN;

//     //Check the limit here it should not be too high or too less
//     if (config.ain_iepe_vref > DUAL_VIB_MAX_DAC_OUT) {
//         CLOG("Invalid IEPE Vref, Max allowed voltage is %0.3lf\n", DUAL_VIB_MAX_DAC_OUT);
//     } else {
//         if ((spi_dac = mcp48f_dac_init(config.dac1_dev, SPI_MODE_3, DAC_SPI_FREQ)) == NULL) {
//             WLOG("DAC1 Init failed!!! ");
//         }
//         if (mcp48f_dac_set_output(spi_dac, DAC_DUAL_VIB_CH, config.ain_iepe_vref) > -1) {
//             CLOG("IEPE Vref update success to %0.3f\n", config.ain_iepe_vref * IEPE_DAC_OUT_AMP_GAIN);
//         } else {
//             WLOG("IEPE Vref update Fail\n");
//         }
//         mcp48f_dac_deinit(spi_dac); //frees spaces and peripheral after reading
//     }
    
// }
