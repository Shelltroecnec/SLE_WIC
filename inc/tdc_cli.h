#ifndef TDC_CLI_H
#define TDC_CLI_H

#define TDC_MIN_ARG_REQ                 5
#define TDC_MAX_ARG_REQ                 5


#define TDC_MAX_DAC_OUT                 5.0


void tdc_help(void);
void tdc_cmd(int argc, char **argv, void *additional_arg);

#endif