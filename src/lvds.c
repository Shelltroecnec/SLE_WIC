#include "headers.h"

// sti2c_t *stI2c = NULL;
FILE *fp = NULL;
void LVDS_show_img(void)
{
    char out_buff[1024] = {0};
    char command[128];
    if (access(config.demoimg, F_OK) == 0)
    {
        sprintf(command, "cat %s > /dev/fb0", config.demoimg);
        exec_sys(command, out_buff); // todo
    }
    else
    {
        CLOG("Demo image file does not exist\n");
        exit(EXIT_FAILURE);
    }
    // check tree and validate vendor id for fixed deivces
}

void lvds_help(void)
{
    CLOG("LVDS Command Usage:\n");
    CLOG("    %s -c LVDS\n", exec_filename);
}

void lvds_cmd(int argc, char **argv, void *additional_arg)
{
    char ch = 0, ch1 = 0;
    if (argc > LVDS_MAX_ARG_REQ)
    {
        // WLOG("[IOEX] Less number of argument/s provided for IOExp Cmd!!\n");
        lvds_help();
        return;
    }
    LVDS_show_img();
    sleep(2);
    CLOG("Were you able to view the image on display? [Y/n] ");

    ch = getchar();
    while ((ch != '\n') && ((ch1 = getchar()) != '\n') && ch1 != (char)EOF)
        ;

    if ((ch == 'Y') || (ch == 'y'))
    {
        CLOG("LVDS test pass\n");
    }
    else if ((ch == 'N') || (ch == 'n'))
    {
        CLOG("LVDS test fail\n");
    }
    else
    {
        CLOG("invalid option entered\r\n");
    }
}