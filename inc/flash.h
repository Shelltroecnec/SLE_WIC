/**
 * @file flash.h
 * @author Shahid Hashmi
 * @brief 
 * @version 0.1
 * @date 2023-02-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef FLASH_H
#define FLASH_H
/* Library */

/* General */
#define FLASH_MIN_ARG_REQ   1
#define FLASH_MAX_ARG_REQ   2


/* Function */
void flash_help(void);
void flash_cmd(int argc, char **argv, char *additionalArgv);
void dump(uint8_t *dt, int n);
#endif