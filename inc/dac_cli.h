#ifndef DAC_CLI_H
#define DAC_CLI_H

#define DAC_MIN_ARG_REQ                 3
#define DAC_MAX_ARG_REQ                 3

#define DAC_SPI_FREQ                    1000000

#define OUTPUT_VOLTAGE                  0
#define OUTPUT_CURRENT                  1

#define OUT_MIN_RANGE                   0  //0
#define OUT_MAX_RANGE                   2.5 //2.5
#define IN_MIN_RANGE_V                  0
#define IN_MAX_RANGE_V                  5

#define OUT_MIN_RANGE_I                 1 //0.48
#define OUT_MAX_RANGE_I                 5 //2.4 //4.39
#define IN_MIN_RANGE_I                   4
#define IN_MAX_RANGE_I                   20

#define DAC_OUT_UNIT(x)                   ((x == OUTPUT_VOLTAGE)? "Volt": "mA")

#define DAC_STRING(x)                   ((x == 0)? "DAC1": "DAC2")
#define DAC_MUX_RON_CAL_VAL             3 //additional(or calibration) resistance value due to MUX lines  
        
void dac_help(void);
void dac_cmd(int argc, char **argv, void *additional_arg);

#endif