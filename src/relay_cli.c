    /**
 * @file relay_cli.c
 * @author Fakhruddin Khan
 * @brief 
 * @version 0.1
 * @date 2021-04-19
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "headers.h"
#include <fcntl.h>

//Error Handling.....and if the pin is not default as gpio...what?

#define IMX_GPIO_NR(port, index) ((((port)-1) * 32) + ((index)&31))
#define MAX_BUF 100


void test_Function(void){
    DLOG("In test fucntion call\n");
}

/**
 * @brief                   Relay operation
 *  
 * @param relay_selected    Select the relay
 * @param relay_state       State of relay
 * @param operation         Operation to be performed, either one at a time or bulk operation
 * @return                  uint8_t value is returned
 */
int32_t relay_operation(int32_t relay_selected, int32_t relay_state, int32_t operation){
    uint8_t state = 0;
    uint8_t retval = 0;
    ILOG("ssr : %d, state: %d, op: %d\n", relay_selected, relay_state, operation);
    for (int32_t i = 0; i < NUM_MAX_RLY; i++) {
        if (operation == RLY_SINGLE_OP_MODE && relay_selected == i) {
            digital_pin_out_write(SELECT_RELAY_IO(i), ((relay_state == 0) ? RELAY_OFF_STATE(i) : RELAY_ON_STATE(i)));
            //CLOG("[RLY] updating RLY[%d], IO: %s with %d\n", i, SELECT_RELAY_IO(i), ((relay_state == 0) ? 0 : 01));
        } else if (operation == RLY_BULK_OP_MODE || operation == RLY_INDVIDUAL_BULK_OP_MODE) {
            state = ((operation == RLY_INDVIDUAL_BULK_OP_MODE)? (((relay_state >> i) & 0x01)?  RELAY_ON_STATE(i) : RELAY_OFF_STATE(i)): ((relay_state == 0) ? RELAY_OFF_STATE(i) : RELAY_ON_STATE(i)));
            digital_pin_out_write(SELECT_RELAY_IO(i), state);
            //CLOG("[RLY] updating RLY[%d], IO: %s with %d\n", i, SELECT_RELAY_IO(i), ((operation == RLY_INDVIDUAL_BULK_OP_MODE) ? ((relay_state >> i) & 0x01) : ((relay_state == 0) ? 0 : 01)));
        }

        if(i == 0){ //DCR is active low
            retval = (retval  | (((~digital_pin_read(SELECT_RELAY_IO(i))) & 0x01) << i)) ;
        } else {
            retval = (retval  | ((digital_pin_read(SELECT_RELAY_IO(i)) & 0x01) << i)) ;
        }
        CLOG("%s[%s]: %s, 0x%02x\n", RELAY_STRING(i), SELECT_RELAY_IO(i), RELAY_STATE_STR(i,digital_pin_read(SELECT_RELAY_IO(i))), retval);
    }

    return retval;
}

/**
 * @brief           Relay helper function
 * 
 */
void relay_help(void) {
    CLOG("RLY command Usage:\n");
    CLOG("    %s -c RLY [CH] [STATE]\n", exec_filename);
    CLOG("       CH - RLY Channel selection:\n");
    CLOG("           0 - 24V dry contact relay\n");
    CLOG("           1 - Select relay 1 \n");
    CLOG("           2 - Select relay 2 \n");
    CLOG("           3 - Select relay 3 \n");
    CLOG("           4 - Select relay 4 \n");
    CLOG("           5 - Select relay 5 \n");
    CLOG("           6 - Select relay 6 \n");
    CLOG("           0xFE - Bulk operation, ON/OFF each relay based on each bits in STATE\n");
    CLOG("           0xFF - Bulk operation\n");
    CLOG("       STATE -  state of relay\n");
    CLOG("           0 -  Turn OFF the selected relay\n");
    CLOG("           1 -  Turn ON the selected relay\n");
    CLOG("           xx - Hex value accepted only if selected channel is 0xFE Turn ON the selected relay\n");
}


/**
 * @brief                   Relay CLI command
 * 
 * @param argc              (ARGument Count) is int and stores number of command-line arguments passed by the user including the name of the program. 
 *                          So if we pass a value to a program, value of argc would be 2 (one for argument and one for program name) 
 *                          The value of argc should be non negative.
 * @param argv              (ARGument Vector) is array of character pointers listing all the arguments.
                            If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
                            Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.
 * @param additional_arg    Any additional arguments required
 */
void relay_cmd(int argc, char **argv, void *additional_arg) {
    if (argc < RLY_MIN_ARG_REQ) {
        // CLOG("Less number of argument/s provided!!\n");
        relay_help();
        return;
    }

    // int fd;
    // int retval = 0;
    int32_t relay_selected = htoi(argv[1]);
    int32_t relay_state = htoi(argv[2]);
    int32_t operation = ((relay_selected == 0xFF) ? RLY_BULK_OP_MODE : (relay_selected == 0xFE) ? RLY_INDVIDUAL_BULK_OP_MODE
                                                                                            : RLY_SINGLE_OP_MODE);

    //As it is unsigned value hence not checked if it is less than 0
    if ((relay_selected > 6) && relay_selected != 0xFF && relay_selected != 0xFE) {
        CLOG("Invalid RLY selection!!!\n");
        relay_help();
        return;
    }

    relay_operation(relay_selected, relay_state, operation);

   
}
