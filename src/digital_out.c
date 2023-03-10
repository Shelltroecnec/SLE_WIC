/**
 * @file digital_out.c
 * @author Shahid Hashmi
 * @brief
 * @version 0.1
 * @date 2023-02-03
 *
 * @copyright Copyright (c) 2023
 *
 */

/* Library */
#include "headers.h"

/* Global Variables */
extern bool loop;

/* Help function for Digital Out */
void digital_out_help(void)
{
    CLOG("DIGOUT command Usage:\n");
    CLOG("    %s -c DIGOUT [CH] [OPTION] [Value] \n", exec_filename);
    CLOG("       CH - DIGOUT Channel selection:\n");
    CLOG("           0 - Select Channel 0 \n");
    CLOG("           1 - Select Channel 1 \n");
    CLOG("           2 - Select Channel 2 \n");
    CLOG("           3 - Select Channel 3 \n");
    CLOG("           4 - Select Channel 4 \n");
    CLOG("           5 - Select Channel 5 \n");
    CLOG("           6 - Select Channel 6 \n");
    CLOG("           7 - Select Channel 7 \n");
    CLOG("       OPTION selection:\n");
    CLOG("           1 - Digout1 \n");
    CLOG("           2 - Dignout_DC1 \n");
    CLOG("           0 - Both \n");
    CLOG("       VALUE - Value selection:\n");
    CLOG("           0 - OFF \n");
    CLOG("           1 - ON \n");
    // CLOG("           0xFE - Bulk operation, ON/OFF each relay based on each bits in STATE\n");
    // CLOG("           0xFF - Bulk operation\n");
    // CLOG("       STATE -  state of relay\n");
    // CLOG("           0 -  Turn OFF the selected relay\n");
    // CLOG("           1 -  Turn ON the selected relay\n");
    // CLOG("           xx - Hex value accepted only if selected channel is 0xFE Turn ON the selected relay\n");
}

/* Periodic Callback Function */
int digout_periodic_callback(int ichannel, int ioption, int ivalue)
{

    // if (ptr != NULL)
    // {
    //     ILOG("exiting from digital output gpio test %s\n", ptr);
    //     return 1;
    // }
    // DLOG("Number of samples read: %d, ch: %d\n", ++sampleCnt, ch);

    if (ioption == 1 || ioption == 0)
    {
        if (ichannel == 0)
            CLOG("Digital channel 0-0: %s\n", (digital_pin_out_write(config.digital_out_1_pin, ivalue) & 0x01) ? "HIGH" : "LOW");
        if (ichannel == 1)
            CLOG("Digital channel 0-1: %s\n", (digital_pin_out_write(config.digital_out_2_pin, ivalue) & 0x01) ? "High" : "Low");
        if (ichannel == 2)
            CLOG("Digital channel 0-2: %s\n", (digital_pin_out_write(config.digital_out_3_pin, ivalue) & 0x01) ? "High" : "Low");
        if (ichannel == 3)
            CLOG("Digital channel 0-3: %s\n", (digital_pin_out_write(config.digital_out_4_pin, ivalue) & 0x01) ? "High" : "Low");
        if (ichannel == 4)
            CLOG("Digital channel 0-4: %s\n", (digital_pin_out_write(config.digital_out_5_pin, ivalue) & 0x01) ? "High" : "Low");
        if (ichannel == 5)
            CLOG("Digital channel 0-5: %s\n", (digital_pin_out_write(config.digital_out_6_pin, ivalue) & 0x01) ? "High" : "Low");
        if (ichannel == 6)
            CLOG("Digital channel 0-6: %s\n", (digital_pin_out_write(config.digital_out_7_pin, ivalue) & 0x01) ? "High" : "Low");
        if (ichannel == 7)
            CLOG("Digital channel 0-7: %s\n", (digital_pin_out_write(config.digital_out_8_pin, ivalue) & 0x01) ? "High" : "Low");
    }

    if (ioption == 2 || ioption == 0)
    {
        if (ichannel == 0)
            CLOG("Digital channel 1-0: %s\n", (digital_pin_out_write(config.digital_out_9_pin, ivalue) & 0x01) ? "High" : "Low");
        if (ichannel == 1)
            CLOG("Digital channel 1-1: %s\n", (digital_pin_out_write(config.digital_out_10_pin, ivalue) & 0x01) ? "High" : "Low");
        if (ichannel == 2)
            CLOG("Digital channel 1-2: %s\n", (digital_pin_out_write(config.digital_out_11_pin, ivalue) & 0x01) ? "High" : "Low");
        if (ichannel == 3)
            CLOG("Digital channel 1-3: %s\n", (digital_pin_out_write(config.digital_out_12_pin, ivalue) & 0x01) ? "High" : "Low");
        if (ichannel == 4)
            CLOG("Digital channel 1-4: %s\n", (digital_pin_out_write(config.digital_out_13_pin, ivalue) & 0x01) ? "High" : "Low");
        if (ichannel == 5)
            CLOG("Digital channel 1-5: %s\n", (digital_pin_out_write(config.digital_out_14_pin, ivalue) & 0x01) ? "High" : "Low");
        if (ichannel == 6)
            CLOG("Digital channel 1-6: %s\n", (digital_pin_out_write(config.digital_out_15_pin, ivalue) & 0x01) ? "High" : "Low");
        if (ichannel == 7)
            CLOG("Digital channel 1-7: %s\n", (digital_pin_out_write(config.digital_out_16_pin, ivalue) & 0x01) ? "High" : "Low");
    }

    ILOG(" ");
    return 0;
}

/* Command Function for Digital Out */
void digital_out_cmd(int argc, char **argv, char *aditionalargv)
{

    struct timeval tv;
    memset(config.IOExpDev, 0, sizeof(config.IOExpDev));
    memcpy(config.IOExpDev, "/dev/i2c-2", sizeof(config.IOExpDev));

    // int sampleCnt = 0;
    int channel = 0;
    int option = 0;
    int value = 0;

    if (argc < DIGOUT_MIN_ARG_REQ || argc > DIGOUT_MAX_ARG_REQ)
    {
        // CLOG("Less number of argument/s provided!!\n");
        digital_out_help();
        return;
    }

    /* Parsing CLI arhuments */
    channel = atoi(argv[1]);
    option = atoi(argv[2]);
    value = atoi(argv[3]);
    
    if (channel > 7 || channel < 0)
    {
        CLOG("Invalid Channel Selection !!! \r\n");
        digital_out_help();
        return;
    }
    
    if(option > 2){
        CLOG("Invalid Option !!!\r\n");
        digital_out_help();
        return;
    }
    
    if (loop == true)
    {
        tv.tv_sec = config.cli_din_op_prd;
        tv.tv_usec = 0;
        check_stdin_cb(&tv, &digout_periodic_callback, channel);
    }
    else
    {
        digout_periodic_callback(channel, option, value);
    }
}

// uint64_t digout_last_global_tmst = 0, digout_debounce_tmst = 0;
// /* Digital Out Thread */
// void *digital_out_opr_thread(void *vargp)
// {
//     uint64_t digout_global = 0;
//     int digCurrState = 0;
//     int digLastState = 0;
//     uint8_t digRetValue = 0;

//     // IOEX_Init(config.IOExpDev2,SLAVE_ADDR1);

//     while (1)
//     {
//         digout_global = getCurrentTimeMs();
//         if (digout_global - digout_last_global_tmst > 10)
//         {
//             getCurrentTimeMsStr(ts);
//             // DLOG("[%s] [Main] thread R: %d, G: %u\r\n", ts, ranNum, digital_global);
//             digout_last_global_tmst = digout_global;

//             //
//             digCurrState = digital_pin_in_read(config.digital_out_1_pin);
//             if (digCurrState != digLastState)
//             {
//                 if (digout_global - digout_debounce_tmst > 500)
//                 {
//                     digLastState = digCurrState;
//                     digout_debounce_tmst = digout_global;

//                     // int retval = 0;
//                     char *outs;
//                     cJSON *root;
//                     /* create root node and array */
//                     root = cJSON_CreateObject();

//                     cJSON_AddItemToObject(root, "digout_channel", cJSON_CreateNumber(1));

//                     cJSON_AddItemToObject(root, "method", cJSON_CreateString("STATUS"));

//                     cJSON_AddItemToObject(root, "peripheral", cJSON_CreateString("DIGOUT"));

//                     cJSON_AddItemToObject(root, "OUT_1", cJSON_CreateString((((digital_pin_out_write(config.digital_out_1_pin, 1) & 0x01) == 0) ? "OFF" : "ON")));

//                     char ts[30];
//                     getCurrentTimeMsStr(ts);

//                     cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(ts));

//                     /* print everything */
//                     outs = cJSON_PrintUnformatted(root);
//                     send_to_clients(-1, outs, strlen(outs)); // send this to MQTT
//                     DLOG("payload[%d]%s\n", (int)strlen(outs), outs);
//                     free(outs);

//                     /* free all objects under root and root itself */
//                     cJSON_Delete(root);
//                 }
//             }
//         }
//     }
// }
