/**
 * @file digital_out.h
 * @author Shahid Hashmi
 * @brief 
 * @version 0.1
 * @date 2023-02-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef DIGITAL_OUT_H
#define DIGITAL_OUT_H

/* General */
#define DIGOUT_MIN_ARG_REQ  4
#define DIGOUT_MAX_ARG_REQ  4
#define MAX_CLI_DIGOUT_OP_PRD   60
#define MIN_CLI_DIGOUT_OP_PRD   1

/* Functions */
void digital_out_help(void);
void digital_out_cmd(int argc, char **argv, char *aditionalargv);
//void *digital_out_opr_thread(void *vargp);
int digout_periodic_callback(int ichannel, int ioption, int ivalue);

#endif