#ifndef _QSPI_FLASH_H
#define _QSPI_FLASH_H

#define QSPIF_MIN_ARG_REQ   2

void qspi_flash_help(void);
void qspi_flash_cmd(int argc, char **argv, void *additional_arg);


#endif 
