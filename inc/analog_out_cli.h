#ifndef ANALOG_OUT_CLI_H
#define ANALOG_OUT_CLI_H


void analog_out_cmd(int argc, char **argv, void *additional_arg);
void analog_out_help(void);
int aout_callback(char *ptr, void *args);

#define  ANO_MIN_ARG_REQ        3
#define  MIN_DAC_WR_PRD         1
#define  MAX_DAC_WR_PRD         10000
#define  MIN_DAC_BC_INC_PRD     1
#define  MAX_DAC_BC_INC_PRD     2047

#define OUTPUT_RAMP             0
#define OUTPUT_STATIC           1
#define DAC_OUT_STATE(x)        ((x == OUTPUT_RAMP)? "Ramp": "Static")
#define ANO_MIN_ARG_REQ         3
#define DAC_MIN_INPUT_BIT_CNT   0
#define DAC_MAX_VALID_BIT_CNT   4095

#define RAMP_UP                 1
#define RAMP_DOWN               0

#endif