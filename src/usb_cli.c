#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>

#include "headers.h"
/**
 * @brief USB cli helper function
 *
 */
void usb_help(void)
{
    CLOG("USB command Usage:\n");
    CLOG("   %s -c USB\n", exec_filename);
    CLOG("       This CLI test the USB devices connected on HUB\n");
}

/**
 * @brief                   USB CLI command
 *
 * @param argc              (ARGument Count) is int and stores number of command-line arguments passed by the user including the name of the program.
 *                          So if we pass a value to a program, value of argc would be 2 (one for argument and one for program name)
 *                          The value of argc should be non negative.
 * @param argv              (ARGument Vector) is array of character pointers listing all the arguments.
                            If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
                            Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.
 * @param additional_arg    Any additional arguments required
 */
void usb_cmd(int argc, char **argv, void *additional_arg)
{
    char ch = 0, ch1 = 0;
    if (argc < USB_MIN_ARG_REQ)
    {
        usb_help();
        return;
    }
    char out_buff[20][20] = {0};

    /*this logic will search for SDA/SDB in the devices folder and return pass/fail based on search result
    example: The USB which is installed first will be considered USB Drive 1, irrespectively
    of the slot to which it is installed*/
    // To Do:- Modify logic by mounting the drives if you want the results based on slot number
    if (exec_sys("lsusb -t", out_buff))
    {
        CLOG("lsusb results: %s\n", out_buff);

        CLOG("Are you able to view: \n");
        CLOG("      %d devices on bus 1?\n", config.bus1devices);
        CLOG("      %d devices on bus 2?\n", config.bus2devices);
        CLOG("      (Y/y) = Yes, (N/n) = No\n");
        // check tree and validate vendor id for fixed deivces
        // CLOG("Result: USB Device Pass\n");
        ch = getchar();
        while ((ch != '\n') && ((ch1 = getchar()) != '\n') && ch1 != (char)EOF)
            ;

        if ((ch == 'Y') || (ch == 'y'))
        {
            CLOG("USB test pass\n");
        }
        else if ((ch == 'N') || (ch == 'n'))
        {
            CLOG("USB test fail\n");
        }
        else
        {
            CLOG("invalid option entered\r\n");
        }
    }
    else
    {
        CLOG("Result: USB Device Fail\n");
    }
}

/*
void usb_help(void) {
    CLOG("USB command Usage:\n");
    CLOG("   %s -c USB\n", exec_filename);
    CLOG("       This CLI test the USB devices connected on HUB also it will write and read data from the USB stick if connected\n");
}


void usb_cmd(int argc, char **argv, void *additional_arg)
{
    char usb_location[128];
    char ch = 0, ch1 = 0;
    if (argc < USB_MIN_ARG_REQ) {
        usb_help();
        return;
    }

    char out_buff[1024] = {0};
    int len = 0;
    len = exec_sys("lsusb", out_buff);
    CLOG("# lsusb\n");
    ILOG("out_buff : %s", out_buff);
    CLOG("Is your device/s listed above? [Y/n] ", config.cli_aout_dac_wr_prd);

    ch = getchar();
    while ((ch != '\n') && ((ch1 = getchar()) != '\n') && ch1 != (char)EOF);

    if ((ch == 'Y') || (ch == 'y')) {
        CLOG("USB device detection pass, Would you like to test USB stick read/write? [Y/n] ");

        ch = getchar();
        while ((ch != '\n') && ((ch1 = getchar()) != '\n') && ch1 != (char)EOF);
        if ((ch == 'Y') || (ch == 'y')) {
            CLOG("Enter the USB device mount point: ");
            scanf("%s",usb_location);
            while (((ch1 = getchar()) != '\n') && ch1 != (char)EOF);

            DIR  *dip;
            if ((dip = opendir((const char *)usb_location)) == NULL)
            {
                ILOG("%s is not a directory or not found\r\n", usb_location);
            } else {
                closedir(dip);
                FILE *fw, *f;
                int len = 0;
                char *data;
                char test_string[] = "This is test data from USB CLI test\n";
                char test_file_name[] = "/usb_cli_test.txt";
                strcat(usb_location, test_file_name);
                fw = fopen(usb_location, "w");
                fwrite(test_string, 1, sizeof(test_string), fw);
                fclose(fw);

                f = fopen(usb_location, "r");
                // get the length
                fseek(f, 0, SEEK_END);
                len = ftell(f);
                fseek(f, 0, SEEK_SET);

                data = (char *)malloc(len + 1);

                fread(data, 1, len, f);
                data[len] = '\0';

                fclose(f);

                if (strstr(data, test_string) != NULL) {
                    ILOG("Successfully write/read to USB drive %s\n", usb_location);
                } else {
                    ILOG("Failed to write/read to USB drive %s\n", usb_location);
                }
            }
        }
    } else  {
        CLOG("USB device detection failed\r\n");
    }


    return;
}
*/