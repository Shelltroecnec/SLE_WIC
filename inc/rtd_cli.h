#ifndef RTD_CLI_H
#define RTD_CLI_H

#define RTD_MIN_ARG_REQ                 3
#define RTD_MAX_ARG_REQ                 3

#define RTD_MIN_SCAN_PERIOD             1
#define RTD_MAX_SCAN_PERIOD             60

#define RTD_MAX_SAMPLES_CNT         10000

#define RTD_MUX_RON_CAL_VAL             3 //additional(or calibration) resistance value due to MUX lines  


#define RTD_COUNT_TO_RESISTANCE(x)      ((float)(x * RTD_REF_RESISTOR) / ADC_BIT_CONV_COUNT)
#define RTD_MAX_SAMP_CNT                10              //Max buffer size for RTD Samples
#define MAX_CLI_RTD_OP_PRD              60
#define MIN_CLI_RTD_OP_PRD              1
#define RTD_MAX_CHANNEL                 4


typedef struct {
    //Sensor selection configuration
    int IO_channel;                 // Input and output channel selection option 1-4
    uint8_t  rtd_fault:1;
    uint8_t  counter;
    uint8_t  rtd_sample_cnt;
    uint8_t  channel_scan; // 1-4 for RTD channel 1 to 4 selection and 255 for all channel read
    uint8_t  rtd_fault_status[RTD_MAX_CHANNEL];
    uint16_t avg_adc_count[RTD_MAX_CHANNEL];
    uint16_t min_adc_count[RTD_MAX_CHANNEL];
    uint16_t max_adc_count[RTD_MAX_CHANNEL];
    uint16_t adc_count[RTD_MAX_CHANNEL][RTD_MAX_SAMP_CNT];
    float    rtd_min_temp[RTD_MAX_CHANNEL];
    float    rtd_max_temp[RTD_MAX_CHANNEL];
    float    rtd_avg_temp[RTD_MAX_CHANNEL];
    uint32_t scan_period;
} st_rtd_sample_t;
extern st_rtd_sample_t st_rtd_read;


void rtd_help(void);
void rtd_cmd(int argc, char **argv, void *additional_arg);
int rtd_periodic_callback(char *ptr, void *args);


#endif