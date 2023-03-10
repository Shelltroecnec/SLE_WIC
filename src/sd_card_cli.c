/**
 * @file sd_card_cli.c
 * @author Fakhruddin Khan (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-03-06
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "headers.h"

// if earthbreak && volt < 70  --- no input supply
// if earthbreak && volt < 180 --- no input supply

// if volt_NE > 80
//     earthbreak = SET;
/**
 * @brief SD card cli helper function
 * 
 */
void sd_card_help(void) {
    CLOG("SD command Usage:\n");
    CLOG("    %s -c SD [MODE] [MOUNT_POINT]\n", exec_filename);
    CLOG("       MODE - Test mode\n");
    CLOG("            0 - Read only\n");
    CLOG("            1 - Write and Read QSPI flash\n"); 
    CLOG("       MOUNT_POINT - SD card mount point\n");
    CLOG("       It will test the presence of SD card, and data integrity by writing and reading a file\n");
}

/**
 * @brief                   SD card CLI command
 * 
 * @param argc              (ARGument Count) is int and stores number of command-line arguments passed by the user including the name of the program. 
 *                          So if we pass a value to a program, value of argc would be 2 (one for argument and one for program name) 
 *                          The value of argc should be non negative.
 * @param argv              (ARGument Vector) is array of character pointers listing all the arguments.
                            If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
                            Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.
 * @param additional_arg    Any additional arguments required
 */
void sd_card_cmd(int argc, char **argv, void *additional_arg) {
    if (argc < SD_MIN_ARG_REQ) {
        sd_card_help();
        return;
    }

    FILE *fw, *f;
    int len = 0;
    char *data;
    char test_filename[256], cmd[260];
    char test_string[] = "This is test file for SD card test\n";
    char out_buff[1024] = {0};
    int read_only = 0, ret_val = 0;
    read_only = atoi(argv[1]);
    ILOG("Arg2: %s\n", argv[2]);
    if(strstr(argv[2], "/mnt/")){
        strcpy(config.sd_card_mount_path , argv[2]);
        CLOG("Using given mount point: %s\n", config.sd_card_mount_path);
    } else {
        CLOG("Using default SD card mount point: %s\n", config.sd_card_mount_path);
    }
    sprintf(test_filename, "%s/%s", config.sd_card_mount_path, config.sd_card_test_file);
    CLOG("Test file: %s\n", test_filename);


    if (exec_sys("ls /dev/ | grep mmcblk1", out_buff)) {
        if (read_only) {
            // exec_sys(config.sd_card_mount_path, out_buff); //ToDo :- write condition to verify if mount is passed/failed
            sprintf(cmd, "rm %s", test_filename);
            exec_sys(cmd, out_buff);

            fw = fopen(test_filename, "w");
            fwrite(test_string, 1, sizeof(test_string), fw);
            fclose(fw);
            DLOG("SD card is detected, file created at location: %s\n\r", test_filename);            
        }
        //Open for reading and appending (writing at end of file).The file is created if it does not exist
        f = fopen(test_filename, "a+"); //open for reading
        fseek(f, 0, SEEK_END);
        len = ftell(f);
        fseek(f, 0, SEEK_SET);

        data = (char *)malloc(len + 1);
        ret_val = fread(data, 1, len, f);
        data[ret_val] = '\0';

        fclose(f);

        if (strstr(data, test_string) != NULL) {
            CLOG("Result: SD_Card Test Pass\n\r");
        } else {
            CLOG("Result: SD_Card Test Fail\n\r");
        }
        free(data);        
    } else {
        CLOG("NO SD CARD FOUND!!!\n");
        CLOG("Result: SD_Card Test Fail\n");
    }
}