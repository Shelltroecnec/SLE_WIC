#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>

#include "headers.h"

/**
 * @brief       Emmc cli helper function
 * 
 */
void emmc_flash_help(void) {
    CLOG("EMMC command Usage:\n");
    CLOG("   %s -c EMMC [MODE]\n", exec_filename);
    CLOG("       MODE - Test mode\n");
    CLOG("            0 - Read only\n");
    CLOG("            1 - Write and Read EMMC flash\n");
    CLOG("       This CLI will write some data on EMMC flash and read it back to see if it is working\n");
}

/**
 * @brief                   EMMC CLI command
 * 
 * @param argc              (ARGument Count) is int and stores number of command-line arguments passed by the user including the name of the program. 
 *                          So if we pass a value to a program, value of argc would be 2 (one for argument and one for program name) 
 *                          The value of argc should be non negative.
 * @param argv              (ARGument Vector) is array of character pointers listing all the arguments.
                            If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
                            Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.
 * @param additional_arg    Any additional arguments required
 */
void emmc_flash_cmd(int argc, char **argv, void *additional_arg)
{
    if (argc < EMMC_FLASH_ARG_REQ) {
        emmc_flash_help();
        return;
    }
    FILE *fw, *f;
    int len = 0, read_len =0;
    char *data;
    char command[256];
    char test_string[] = "This is test file for EMMC FLASH test\n";
    int read_only = 0;
    read_only = atoi(argv[1]);
    char out_buff[1024] = {0};

    if (read_only) {
        sprintf(command, "rm %s", config.emmc_flash_test_file);
        exec_sys(command, out_buff);
        fw = fopen(config.emmc_flash_test_file, "w");
        fwrite(test_string, 1, sizeof(test_string), fw);
        fclose(fw);
    }
    //Open for reading and appending (writing at end of file).The file is created if it does not exist
    f = fopen(config.emmc_flash_test_file, "a+");
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    fseek(f, 0, SEEK_SET);

    data = (char *)malloc(len + 1);
    read_len = fread(data, 1, len, f);
    data[read_len] = '\0';
    fclose(f);

    if (strstr(data, test_string) != NULL) {
        CLOG("Result: EMMC Test Pass\n\r");
    } else {
        CLOG("Result: EMMC Test Fail\n\r");
    }
    free(data);      
}