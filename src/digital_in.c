#include "headers.h"

extern bool loop;
// struct timeval tv;

/**
 * @brief Digital In Helper function
 * 
 */
void din_help(void)
{
    CLOG("   DI command Usage:\n");
    loopfeature(config.cli_din_op_prd);
    CLOG("       %s -c DI [CH]\n", exec_filename);
    CLOG("          CH - Digital input channels\n");
    CLOG("          Available Digital inputs:\n");
    CLOG("                1 -  Digital input channel 1\n");
    CLOG("                2 -  Digital input channel 2\n");
    CLOG("                3 -  Digital input channel 3\n");
    CLOG("                4 -  Digital input channel 4\n");
    CLOG("                5 -  Digital input channel 5\n");
    CLOG("                6 -  Digital input channel 6\n");
    CLOG("                7 -  Digital input channel 7\n");
    CLOG("                8 -  Digital input channel 8\n");
    CLOG("                9 -  Digital input channel 9\n");
    CLOG("                10 - Digital input channel 10\n");
    CLOG("                11 - Digital input channel 11\n");
    CLOG("                12 - Digital input channel 12\n");
    CLOG("                13 - Digital input channel 13\n");
    CLOG("                14 - Digital input channel 14\n");
    CLOG("                15 - Digital input channel 15\n");
    CLOG("                16 - Digital input channel 16\n");
    CLOG("                0  -  All 8 digital input\n");
}

int sample_count = 0;

/**
 * @brief Function to loop continously untill keyboard interrupt provided
 * 
 * @param ptr Pass NULL to stay in loop or else it will exit
 * @param args arguments to be called back
 * @return int8_t return 0 on success, non 0 on failure
 */
int din_periodic_callback(char *ptr, void *args)
{

    if (ptr != NULL)
    {
        ILOG("exiting from digital input gpio test %s\n", ptr);
        return 1;   
    }
    char ch = 0;
    if (args != NULL){
        ch = *(int *)args;
    }
    DLOG("Number of samples read: %d, ch: %d\n", ++sample_count, ch);

    if(ch == 0 || ch == 1) CLOG("Digital channel 1: %s\n", (digital_pin_in_read(config.digital_in_1_pin) & 0x01) ? "High" : "Low");
    if(ch == 0 || ch == 2) CLOG("Digital channel 2: %s\n", (digital_pin_in_read(config.digital_in_2_pin) & 0x01) ? "High" : "Low");
    if(ch == 0 || ch == 3) CLOG("Digital channel 3: %s\n", (digital_pin_in_read(config.digital_in_3_pin) & 0x01) ? "High" : "Low");
    if(ch == 0 || ch == 4) CLOG("Digital channel 4: %s\n", (digital_pin_in_read(config.digital_in_4_pin) & 0x01) ? "High" : "Low");
    if(ch == 0 || ch == 5) CLOG("Digital channel 5: %s\n", (digital_pin_in_read(config.digital_in_5_pin) & 0x01) ? "High" : "Low");
    if(ch == 0 || ch == 6) CLOG("Digital channel 6: %s\n", (digital_pin_in_read(config.digital_in_6_pin) & 0x01) ? "High" : "Low");
    if(ch == 0 || ch == 7) CLOG("Digital channel 7: %s\n", (digital_pin_in_read(config.digital_in_7_pin) & 0x01) ? "High" : "Low");
    if(ch == 0 || ch == 8) CLOG("Digital channel 8: %s\n", (digital_pin_in_read(config.digital_in_8_pin) & 0x01) ? "High" : "Low");
    if(ch == 0 || ch == 9) CLOG("Digital channel 9: %s\n", (digital_pin_in_read(config.digital_in_9_pin) & 0x01) ? "High" : "Low");
    if(ch == 0 || ch == 10) CLOG("Digital channel 10: %s\n", (digital_pin_in_read(config.digital_in_10_pin) & 0x01) ? "High" : "Low");
    if(ch == 0 || ch == 11) CLOG("Digital channel 11: %s\n", (digital_pin_in_read(config.digital_in_11_pin) & 0x01) ? "High" : "Low");
    if(ch == 0 || ch == 12) CLOG("Digital channel 12: %s\n", (digital_pin_in_read(config.digital_in_12_pin) & 0x01) ? "High" : "Low");
    if(ch == 0 || ch == 13) CLOG("Digital channel 13: %s\n", (digital_pin_in_read(config.digital_in_13_pin) & 0x01) ? "High" : "Low");
    if(ch == 0 || ch == 14) CLOG("Digital channel 14: %s\n", (digital_pin_in_read(config.digital_in_14_pin) & 0x01) ? "High" : "Low");
    if(ch == 0 || ch == 15) CLOG("Digital channel 15: %s\n", (digital_pin_in_read(config.digital_in_15_pin) & 0x01) ? "High" : "Low");
    if(ch == 0 || ch == 16) CLOG("Digital channel 16: %s\n", (digital_pin_in_read(config.digital_in_16_pin) & 0x01) ? "High" : "Low");

    ILOG(" ");
    return 0;
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
void din_cmd(int argc, char **argv, void *additional_arg)
{
    struct timeval tv;
    memset(config.IOExpDev, 0, sizeof(config.IOExpDev));
    memcpy(config.IOExpDev, "/dev/i2c-3", sizeof(config.IOExpDev));
    // CLOG("Default IO scan period is %d seconds. Would you like to modify it? [Y/n] ", config.cli_din_op_prd);

    int ch = 0;

    if (argc < DIN_MIN_ARG_REQ) {
        // CLOG("[AI] Less number of argument/s provided for AI Cmd!!\n");
        din_help();
        return;
    }
    ch = atoi(argv[1]);

    if(ch > 16 && ch < 0){
        CLOG("Invalid input selection!!!\n");
        din_help();
        return;
    }
    if (loop == true) {
        tv.tv_sec = config.cli_din_op_prd;
        tv.tv_usec = 0;
        check_stdin_cb(&tv, &din_periodic_callback, &ch);
    }
    else {
        din_periodic_callback(NULL, &ch);
    }

    // din_periodic_callback(NULL, &ch);
    // uint16_t ch = getchar();
    // if ((ch == 'Y') || (ch == 'y'))
    // {
    //     while ((ch = getchar()) != '\n' && ch != EOF)
    //         ;
    //     CLOG("Enter scan period or delay between two sets of readings: Limit[%d-%d] sec: ", MIN_CLI_DIN_OP_PRD, MAX_CLI_DIN_OP_PRD);
    //     int period = 0;
    //     scanf("%d", &period);
    //     if ((period <= MAX_CLI_DIN_OP_PRD) && (period >= MIN_CLI_DIN_OP_PRD))
    //     {
    //         config.cli_din_op_prd = period; //overwriting default period
    //         CLOG("Setting Digital IO scan period set to %d sec\r\n", config.cli_din_op_prd);
    //     }
    //     else
    //     {
    //         DLOG("Invalid scan period entered, It shoulb be between [%d-%d] sec\r\n", MIN_CLI_DIN_OP_PRD, MAX_CLI_DIN_OP_PRD);
    //         return;
    //     }
    // }
    // else
    // {
    //     CLOG("Using default digital IO scan period\r\n");
    // }
    // DLOG("Scanning Started.....\n");

    // //populating period
    // tv.tv_sec = config.cli_din_op_prd;
    // tv.tv_usec = 0;
    // check_stdin_cb(&tv, &din_periodic_callback, NULL);
}



//Thread for monitoring the GPIO as of now
uint64_t digital_last_global_tmst = 0, digital_debouncing_tmst = 0;
void *digital_monitoring_thread(void *vargp){
    uint64_t digital_global = 0;
    int last_stat = 0;
    int cur_state = 0;

    while(1){
        digital_global = getCurrentTimeMs();
        if(digital_global - digital_last_global_tmst > 10){
            getCurrentTimeMsStr(ts);
            // DLOG("[%s] [Main] thread R: %d, G: %u\r\n", ts, ranNum, digital_global);
            digital_last_global_tmst = digital_global;

            //
            cur_state = digital_pin_in_read(config.digital_in_1_pin);
            if(cur_state != last_stat){
                if(digital_global - digital_debouncing_tmst > 500){
                    last_stat = cur_state;
                    digital_debouncing_tmst = digital_global;

                    // int retval = 0;
                    char *outs;
                    cJSON *root;
                    /* create root node and array */
                    root = cJSON_CreateObject();

                    cJSON_AddItemToObject(root, "di_channel", cJSON_CreateNumber(1));
                    
                    cJSON_AddItemToObject(root, "method", cJSON_CreateString("STATUS"));

                    cJSON_AddItemToObject(root, "peripheral", cJSON_CreateString("DI"));

                    cJSON_AddItemToObject(root, "DI_1", cJSON_CreateString((((digital_pin_in_read(config.digital_in_1_pin) & 0x01) == 0)? "OFF": "ON")));
                    char ts[30];
                    getCurrentTimeMsStr(ts);
                    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(ts));

                    
                    /* print everything */
                    outs = cJSON_PrintUnformatted(root);
                    send_to_clients(-1, outs, strlen(outs)); // send this to MQTT
                    DLOG("payload[%d]%s\n",(int)strlen(outs), outs);
                    free(outs);

                    /* free all objects under root and root itself */
                    cJSON_Delete(root);

                }
            }
        }
    }
	
}