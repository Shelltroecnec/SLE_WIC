#ifndef _EMCC_CLI_H
#define _EMCC_CLI_H

#define EMMC_FLASH_ARG_REQ   2

void emmc_flash_help(void);
void emmc_flash_cmd(int argc, char **argv, void *additional_arg);

#endif
