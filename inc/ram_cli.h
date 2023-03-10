
#ifndef _RAM_CLI_H
#define _RAM_CLI_H

#define IMX_RAM_MIN_ARG_REQ   1
#define MAX_BYTE_DYNAMIC_ALLOCATE   100

void imx_ram_help(void);
void imx_ram_cmd(int argc, char **argv, void *additional_arg);


#endif 
